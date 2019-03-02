Warning: messy code!

This code is for an ESP8266 based IoT smartswitch, which connects to an MQTT server (via WiFi) and listens for instructions. 
The code is based on the example Arduino code for MQTT communication from the Adafruit MQTT library.

For this particular implementation, I used an ESP-01 module, and since it only gives 2 outputs (GPIO 0 and GPIO 2), I also used the serial TX/RX pins to give 2 more outputs. The serial code is still there, but commented. 
