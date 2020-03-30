#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

ESP8266WebServer server(80);

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

    server.on("/", handle_OnConnect);
    server.on("/color", handle_color);
    server.on("/led_off", handle_led_off);
    server.onNotFound(handle_NotFound);

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

void handle_OnConnect()
{
    server.send(200, "text/html", SendHTML());
}

void handle_color()
{
    String r = server.arg("r");
    String g = server.arg("g");
    String b = server.arg("b");

    R = r.toInt();
    G = g.toInt();
    B = b.toInt();

    server.send(200, "text/html", SendHTML());

    Serial.println("Updated RGB values");
}

void handle_led_off()
{
    R, G, B = 0;
    Serial.println("LEDs turned off");
    server.send(200, "text/html", SendHTML());
}

void handle_NotFound()
{
    server.send(404, "text/plain", "Not found");
    Serial.println("Error: 404");
}

String SendHTML()
{
    return F("<!DOCTYPE html><html lang=\"en\"><head> <style>.c{display: block; margin-top: 25px; margin-left: auto; margin-right: auto; width: 300px; position: relative;}.b{margin-top: 25px;}</style> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\"> <title>Color picker</title> <script src=\"https://cdn.jsdelivr.net/npm/@jaames/iro\"></script> <link href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css\" rel=\"stylesheet\"></head><body> <div class=\"container-sm\"> <div class=\"c\" id=\"cW\"> </div></div><div class=\"container-sm\"> <form action=\"/led_off\" method=\"POST\"> <div class=\"b\"> <input type=\"submit\" class=\"btn btn-primary btn-lg btn-block\" value=\"Turn LED\'s off\"></input> </div></form> </div><script>var colorWheel=new iro.ColorPicker(\"#cW\",{layout: [{component: iro.ui.Wheel,},{component: iro.ui.Slider,}]}); colorWheel.on(\'input:change\', function (color){console.log(colorWheel.color.rgb);}); colorWheel.on(\'mount\', function (color){}); </script></body></html>");
}