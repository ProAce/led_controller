#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager.
#include <FS.h>          // Include the SPIFFS library.
#include <ArduinoOTA.h>  // Include the OTA firmware updater library.
#include <ArduinoJson.h> // Include the Arduino JSON parser library.

ESP8266WebServer server(80);

bool handle_file_read(String path);
String get_content_type(String filename);

uint8_t R = 255;
uint8_t G = 255;
uint8_t B = 255;

#define R_pin 1
#define G_pin 1
#define B_pin 1

void setup()
{
    Serial.begin(115200);
    delay(10);

    Serial.println("Connecting to wifi");

    //WiFiManager intialisation. Once completed there is no need to repeat the process on the current board.
    WiFiManager wifiManager;
    // Restart the ESP8266 and connect your PC to the wireless access point called 'LED Controller' then connect to http://192.168.4.1/ and follow instructions to make the WiFi connection.
    wifiManager.setTimeout(180);
    // Fetches ssid and password and tries to connect, if connections succeeds it starts an access point with the name called "LED Controller" and waits in a blocking loop for configuration.
    if (!wifiManager.autoConnect("LED Controller"))
    {
        Serial.println("Failed to connect and timeout occurred");
        delay(3000);
        ESP.reset(); //reset and try again
        delay(5000);
    }

    SPIFFS.begin(); // Start the SPI Flash Files System.

    server.onNotFound([]() {                                  // If the client requests any URI.
        if (!handle_file_read(server.uri()))                  // send it if it exists.
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error.
    });

    server.on("/led", HTTP_POST, handle_led_post);
    server.on("/led", HTTP_GET, handle_led_get);

    server.begin();

    ArduinoOTA.setHostname("LED_controller");
    ArduinoOTA.setPassword("password");

    pinMode(R_pin, OUTPUT);
    pinMode(G_pin, OUTPUT);
    pinMode(B_pin, OUTPUT);

    ArduinoOTA.begin();
}

void loop()
{
    ArduinoOTA.handle();
    server.handleClient();

    analogWrite(R_pin, R * 4);
    analogWrite(G_pin, G * 4);
    analogWrite(B_pin, B * 4);

    yield();
}

void handle_led_post()
{
    String json = server.arg("plain");
    DynamicJsonDocument doc(60);

    deserializeJson(doc, json);

    R = doc["r"];
    G = doc["g"];
    B = doc["b"];

    server.send(202);
}

void handle_led_get()
{
    String JSON;
    DynamicJsonDocument doc(60);

    doc["r"] = R;
    doc["g"] = G;
    doc["b"] = B;

    serializeJson(doc, JSON);

    server.send(200, "application/json", JSON);
}

String get_content_type(String filename)
{ // Convert the file extension to the correct MIME type.
    if (filename.endsWith(".html"))
    {
        return "text/html";
    }
    else if (filename.endsWith(".css"))
    {
        return "text/css";
    }
    else if (filename.endsWith(".js"))
    {
        return "application/javascript";
    }
    else if (filename.endsWith(".ico"))
    {
        return "image/x-icon";
    }
    return "text/plain";
}

bool handle_file_read(String path)
{ // Send the correct file to the client (if it exists).
    if (path.endsWith("/"))
    {
        path += "index.html"; // If a folder is requested, send the index file.
    }

    String contentType = get_content_type(path); // Get the MIME type.

    if (SPIFFS.exists(path))
    {                                                       // If the file exists.
        File file = SPIFFS.open(path, "r");                 // Open it.
        size_t sent = server.streamFile(file, contentType); // And send it to the client.
        file.close();                                       // Then close the file again.
        return true;
    }

    return false; // If the file doesn't exist, return false.
}
