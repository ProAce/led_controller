# LED controller

This controller is written for the ESP8266 and my first attempt to do something with embedded webservers. This controller is made to control a non-addressable RGB strip via a webinterface. A color can be picked from a colorwheel and the selected color will be displayed on the LEDstrip.

![Website preview](https://github.com/ProAce/led_controller/blob/master/img/preview.png "Website preview")

To upload this to an ESP8266 the following steps should be taken:
 - Download the missing libraries via the Arduino IDE library manager.
 - Flash the files in data to the SPIFFS from within the Arduino IDE via `Tools > ESP8266 Sketch Data upload`
 - Change the WiFi credentials.
 - Flash the sketch.
 - Connect to the IP address of the ESP module.
 
