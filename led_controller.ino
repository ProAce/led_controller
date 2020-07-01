#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoWebsockets.h>
#include <FS.h>

const char *ssid = "";      // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "!"; // The password of the Wi-Fi network

using namespace websockets;

WebsocketsServer wsServer;
WebsocketsClient client;
ESP8266WebServer server(80);

uint8_t R = 0;
uint8_t G = 0;
uint8_t B = 0;

#define R_pin 13
#define G_pin 12
#define B_pin 14

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password); // Connect to the network

  Serial.print("Connecting to ");
  Serial.println(ssid);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }

  Serial.println();
  Serial.println(WiFi.localIP());

  SPIFFS.begin(); // Start the SPI Flash Files System.

  server.onNotFound([]() {                              // If the client requests any URI.
    if (!handle_file_read(server.uri()))                // send it if it exists.
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error.
  });

  server.begin();

  // start the Websocket server on port 8080.
  wsServer.listen(8080);
}

void loop()
{
  server.handleClient();

  server.handleClient();

  if (wsServer.poll()) // Check if there is a new client trying to connect.
  {
    Serial.println("New client");
    client = wsServer.accept();

    // Setup Callbacks for the new client.
    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);
  }

  if (client.available()) // If there is a client check if it has new messages.
  {
    client.poll();
  }
  // analogWrite(R_pin, R * 4);
  // analogWrite(G_pin, G * 4);
  // analogWrite(B_pin, B * 4);
}

// Functions to handle websocket messages and events

void onEventsCallback(WebsocketsEvent event, String data)
{
  switch (event)
  {
  case WebsocketsEvent::ConnectionOpened:
    Serial.println("Connnection Opened");
    break;

  case WebsocketsEvent::ConnectionClosed:
    Serial.println("Connnection Closed");
    client.close();
    break;
  }
}

void onMessageCallback(WebsocketsMessage message)
{
  Serial.print("Got Message: ");
  Serial.println(message.data());

  String data = message.data();

  switch (data[0]) // Parse the incoming message.
  {
  case 'c':
    unmarshall_colors();
    break;

  case 'i':
    client.send(marshall_colors());
    break;
  }
}

// Functions to handle the webserver file serving

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
  {                                                     // If the file exists.
    File file = SPIFFS.open(path, "r");                 // Open it.
    size_t sent = server.streamFile(file, contentType); // And send it to the client.
    file.close();                                       // Then close the file again.
    return true;
  }

  return false; // If the file doesn't exist, return false.
}

void unmarshall_colors(String input)
{
  uint8_t index = 0;
  uint8_t next_index = input.indexOf(",");

  R = input.substring(index, next_index).toInt();

  index = next_index + 1;
  next_index = input.indexOf(",", index);

  G = input.substring(index, next_index).toInt();

  B = input.substring(next_index + 1).toInt();
}

String marshall_colors()
{
  String data = "c,";
  data += String(R);
  data += ",";
  data += String(G);
  data += ",";
  data += String(B);

  return data;
}