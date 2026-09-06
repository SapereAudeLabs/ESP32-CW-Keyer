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
    Serial.println("Mode configuration AP actif");
    Serial.print("SSID : ");
    Serial.println(AP_SSID);
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.softAPIP());

    return true;
}

void AccessPointMode::loop()
{
    if (running)
    {
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
        !server.hasArg("right"))
    {
        server.send(
            400,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "Paramètres manquants."
            )
        );

        return;
    }

    const String leftValue = server.arg("left");
    const String rightValue = server.arg("right");

    if (leftValue.length() != 1 ||
        rightValue.length() != 1)
    {
        server.send(
            400,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "Chaque caractère doit contenir exactement "
                "un symbole."
            )
        );

        return;
    }

    const char leftCharacter = leftValue.charAt(0);
    const char rightCharacter = rightValue.charAt(0);

    if (!settingsStore.setCharacters(
            leftCharacter,
            rightCharacter
        ))
    {
        server.send(
            500,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "Valeurs invalides."
            )
        );

        return;
    }

    if (!settingsStore.save())
    {
        server.send(
            500,
            "text/html; charset=utf-8",
            makeConfigurationPage(
                "Impossible d'enregistrer les réglages."
            )
        );

        return;
    }

    server.send(
        200,
        "text/html; charset=utf-8",
        makeConfigurationPage(
            "Réglages enregistrés. Redémarrage en cours..."
        )
    );

    Serial.println("Réglages enregistrés.");
    Serial.print("Left Paddle character : ");
    Serial.println(leftCharacter);
    Serial.print("Right Paddle character : ");
    Serial.println(rightCharacter);

    server.client().flush();
    delay(500);

    ESP.restart();
}

void AccessPointMode::handleNotFound()
{
    server.send(
        404,
        "text/plain; charset=utf-8",
        "Page introuvable."
    );
}

String AccessPointMode::makeConfigurationPage(
    const String& message
) const
{
    const KeyerSettings& settings = settingsStore.get();

    String page;

    page.reserve(2200);

    page += F(
        "<!doctype html>"
        "<html lang='fr'>"
        "<head>"
        "<meta charset='utf-8'>"
        "<meta name='viewport' "
        "content='width=device-width,initial-scale=1'>"
        "<title>Morse Keyer</title>"
        "<style>"
        "body{font-family:sans-serif;max-width:500px;"
        "margin:40px auto;padding:0 20px}"
        "label{display:block;margin-top:20px}"
        "input{font-size:1.4em;width:80px;padding:8px}"
        "button{margin-top:25px;padding:12px 25px;"
        "font-size:1em}"
        ".message{padding:12px;background:#e4f4e4;"
        "margin-bottom:20px}"
        "</style>"
        "</head>"
        "<body>"
        "<h1>Configuration du keyer</h1>"
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
        "<input id='left' name='left' maxlength='1' "
        "required value='"
    );

    page += htmlEscape(
        String(settings.leftPaddleCharacter)
    );

    page += F(
        "'>"

        "<label for='right'>"
        "Right Paddle character"
        "</label>"
        "<input id='right' name='right' maxlength='1' "
        "required value='"
    );

    page += htmlEscape(
        String(settings.rightPaddleCharacter)
    );

    page += F(
        "'>"
        "<br>"
        "<button type='submit'>Save and Reboot</button>"
        "</form>"
        "</body>"
        "</html>"
    );

    return page;
}

String AccessPointMode::htmlEscape(const String& value) const
{
    String escaped = value;

    escaped.replace("&", "&");
    escaped.replace("<", "<");
    escaped.replace(">", ">");
    escaped.replace(String((char)34), "\"");
    escaped.replace(String((char)39), "'");

    return escaped;
}