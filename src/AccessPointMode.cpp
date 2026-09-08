#include "AccessPointMode.h"

#include <WiFi.h>

#include "Config.h"

AccessPointMode::AccessPointMode(
    KeyerSettingsStore& settingsStore
)
    : settingsStore(settingsStore),
      server(80),
      running(false)
{
}

bool AccessPointMode::begin()
{
    WiFi.mode(WIFI_AP);

    if (!WiFi.softAP(AP_SSID, nullptr, AP_CHANNEL))
    {
        return false;
    }

    dnsServer.start(
        53,
        "*",
        WiFi.softAPIP()
    );

    server.on(
        "/",
        HTTP_GET,
        [this]()
        {
            handleRoot();
        }
    );

    server.on(
        "/save",
        HTTP_POST,
        [this]()
        {
            handleSave();
        }
    );

    server.onNotFound(
        [this]()
        {
            handleNotFound();
        }
    );

    server.begin();

    running = true;

    Serial.println();
    Serial.println("Configuration mode AP active");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    return true;
}

void AccessPointMode::loop()
{
    if (running)
    {
        dnsServer.processNextRequest();
        server.handleClient();
    }
}

bool AccessPointMode::isRunning() const
{
    return running;
}

void AccessPointMode::handleRoot()
{
    server.send(
        200,
        "text/html; charset=utf-8",
        makeConfigurationPage()
    );
}

void AccessPointMode::handleSave()
{
    if (!server.hasArg("left") ||
        !server.hasArg("right") ||
        !server.hasArg("brightness"))
    {
        server.send(
            400,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "Missing parameters."
            )
        );

        return;
    }

    const String leftValue = server.arg("left");
    const String rightValue = server.arg("right");
    const String brightnessValue = server.arg("brightness");

    if (leftValue.length() != 1 ||
        rightValue.length() != 1)
    {
        server.send(
            400,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "One symbol per paddle only."
            )
        );

        return;
    }

    const int brightness =
        brightnessValue.toInt();

    if (brightness < 0 || brightness > 100)
    {
        server.send(
            400,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "Brightness must be between 0 and 100."
            )
        );

        return;
    }

    const char leftCharacter =
        leftValue.charAt(0);

    const char rightCharacter =
        rightValue.charAt(0);

    if (!settingsStore.setCharacters(
            leftCharacter,
            rightCharacter
        ))
    {
        server.send(
            500,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "Invalid paddle values."
            )
        );

        return;
    }

    settingsStore.setLedBrightnessPercent(
        static_cast<uint8_t>(brightness)
    );

    if (!settingsStore.save())
    {
        server.send(
            500,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "Settings could not be saved."
            )
        );

        return;
    }

    server.send(
        200,
        "text/html; charset=utf-8",
        makeConfigurationPage(
            "Settings saved. Rebooting..."
        )
    );

    Serial.println("Settings saved.");
    Serial.print("Left paddle character: ");
    Serial.println(leftCharacter);
    Serial.print("Right paddle character: ");
    Serial.println(rightCharacter);
    Serial.print("LED brightness: ");
    Serial.print(brightness);
    Serial.println("%");

    server.client().flush();

    delay(500);

    ESP.restart();
}

void AccessPointMode::handleNotFound()
{
    server.sendHeader(
        "Location",
        String("http://") + WiFi.softAPIP().toString(),
        true
    );

    server.send(
        302,
        "text/plain",
        ""
    );
}

String AccessPointMode::makeConfigurationPage(
    const String& message
) const
{
    const KeyerSettings& settings =
        settingsStore.get();

    String page;

    page.reserve(3000);

    page += F(
        "<!doctype html>"
        "<html lang='en'>"
        "<head>"
        "<meta charset='utf-8'>"
        "<meta name='viewport' "
        "content='width=device-width,initial-scale=1'>"
        "<title>Morse Keyer Configuration</title>"
        "<style>"
        "body{font-family:sans-serif;max-width:500px;"
        "margin:40px auto;padding:0 20px}"
        "label{display:block;margin-top:20px}"
        "input[type=text]{font-size:1.4em;width:80px;padding:8px}"
        "input[type=range]{width:100%;margin-top:10px}"
        "button{margin-top:25px;padding:12px 25px;"
        "font-size:1em}"
        ".message{padding:12px;background:#e4f4e4;"
        "margin-bottom:20px}"
        ".brightness-value{font-weight:bold}"
        "</style>"
        "</head>"
        "<body>"
        "<h1>Morse Keyer Configuration</h1>"
        "<p style='font-style:italic;font-size:0.75em;color:#888'>"
        "Courtesy of Jim @ SapereAudeLabs "
        "(<a href='https://github.com/SapereAudeLabs'>"
        "github.com/SapereAudeLabs</a>)"
        "</p>"
    );

    if (message.length() > 0)
    {
        page += F("<div class='message'>");
        page += htmlEscape(message);
        page += F("</div>");
    }

    page += F(
        "<form method='post' action='/save'>"

        "<label for='left'>"
        "Left Paddle character"
        "</label>"
        "<input type='text' id='left' name='left' "
        "maxlength='1' required value='"
    );

    page += htmlEscape(
        String(settings.leftPaddleCharacter)
    );

    page += F(
        "'>"

        "<label for='right'>"
        "Right Paddle character"
        "</label>"
        "<input type='text' id='right' name='right' "
        "maxlength='1' required value='"
    );

    page += htmlEscape(
        String(settings.rightPaddleCharacter)
    );

    page += F(
        "'>"

        "<label for='brightness'>"
        "LED brightness: "
        "<span id='brightnessValue' "
        "class='brightness-value'>"
    );

    page += String(
        settings.ledBrightnessPercent
    );

    page += F(
        "</span>%"
        "</label>"

        "<input type='range' "
        "id='brightness' "
        "name='brightness' "
        "min='0' "
        "max='100' "
        "value='"
    );

    page += String(
        settings.ledBrightnessPercent
    );

    page += F(
        "' "
        "oninput='brightnessValue.textContent=this.value'>"

        "<br>"
        "<button type='submit'>Save and Reboot</button>"
        "</form>"
        "</body>"
        "</html>"
    );

    return page;
}

String AccessPointMode::htmlEscape(
    const String& value
) const
{
    String escaped = value;

    escaped.replace("&", "&");
    escaped.replace("<", "<");
    escaped.replace(">", ">");
    escaped.replace(String((char)34), "\"");
    escaped.replace(String((char)39), "'");

    return escaped;
}