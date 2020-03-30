#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <FS.h>          // Include the SPIFFS library

ESP8266WebServer server(80);

bool handle_file_read(String path);

uint8_t R, G, B;

#define R_pin 1
#define G_pin 1
#define B_pin 1

void setup()
{
    Serial.begin(115200);

    //WiFiManager intialisation. Once completed there is no need to repeat the process on the current board
    WiFiManager wifiManager;
    // Restart the ESP8266 and connect your PC to the wireless access point called 'LED Controller' then connect to http://192.168.4.1/ and follow instructions to make the WiFi connection
    wifiManager.setTimeout(180);
    // Fetches ssid and password and tries to connect, if connections succeeds it starts an access point with the name called "LED Controller" and waits in a blocking loop for configuration
    if (!wifiManager.autoConnect("LED Controller"))
    {
        Serial.println("Failed to connect and timeout occurred");
        delay(3000);
        ESP.reset(); //reset and try again
        delay(5000);
    }

    SPIFFS.begin(); // Start the SPI Flash Files System

    server.onNotFound([]() {                                  // If the client requests any URI
        if (!handle_file_read(server.uri()))                  // send it if it exists
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    server.begin();
    Serial.println("HTTP server started");
}
void loop()
{
    server.handleClient();

    analogWrite(R_pin, R * 4);
    analogWrite(G_pin, G * 4);
    analogWrite(B_pin, B * 4);
}

// void handle_color()
// {
//     String r = server.arg("r");
//     String g = server.arg("g");
//     String b = server.arg("b");

//     R = r.toInt();
//     G = g.toInt();
//     B = b.toInt();

//     server.send(200, "text/html", SendHTML());
//     Serial.println("Updated RGB values");
// }

// void handle_led_off()
// {
//     R, G, B = 0;
//     Serial.println("LEDs turned off");
//     server.send(200, "text/html", SendHTML());
// }

String getContentType(String filename)
{ // convert the file extension to the MIME type
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
{ // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);

    if (path.endsWith("/"))
    {
        path += "index.html"; // If a folder is requested, send the index file
    }

    String contentType = getContentType(path); // Get the MIME type

    if (SPIFFS.exists(path))
    {                                                       // If the file exists
        File file = SPIFFS.open(path, "r");                 // Open it
        size_t sent = server.streamFile(file, contentType); // And send it to the client
        file.close();                                       // Then close the file again
        return true;
    }

    Serial.println("\tFile Not Found");

    return false; // If the file doesn't exist, return false
}
