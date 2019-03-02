/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "HOME1234"
#define WLAN_PASS       "supersecurewifipassword"

//#define led D4 //LoLin
//#define led 2  //LoLin
#define out2 2
#define out1 0

#define out3 3   //RX = 3
#define out4 1   //TX = 1
/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "mqtt.server.net"
#define AIO_SERVERPORT  8883                   // use 8883 for SSL
#define AIO_USERNAME    "username"
#define AIO_KEY         "password"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
//WiFiClient client;
// or... use WiFiClientSecure for SSL
WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup feeds for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish statusfeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/status");
Adafruit_MQTT_Publish out1status = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/out1");
Adafruit_MQTT_Publish out2status = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/out2");
Adafruit_MQTT_Publish out3status = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/out3");
Adafruit_MQTT_Publish out4status = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/out4");

Adafruit_MQTT_Publish mainstatusfeed = Adafruit_MQTT_Publish(&mqtt, "status"); //shared feed shows devices connecting

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoff1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff1");
Adafruit_MQTT_Subscribe onoff2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff2");
Adafruit_MQTT_Subscribe onoff3 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff3");
Adafruit_MQTT_Subscribe onoff4 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff4");


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

char statusMessage[32] = "ESP-01 Switcher Status:      ";
char out1stat, out2stat, out3stat, out4stat;
uint8_t out1val, out2val, out3val, out4val;

void setup() {

  //********** CHANGE PIN FUNCTION  TO GPIO **********
  //GPIO 1 (TX) swap the pin to a GPIO.
 // pinMode(1, FUNCTION_3);
  //GPIO 3 (RX) swap the pin to a GPIO.
  //pinMode(3, FUNCTION_3);
  //**************************************************
pinMode(1, OUTPUT);
pinMode(3, OUTPUT);

  //pinMode(led, OUTPUT);
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  digitalWrite(out1, LOW); //switch to HIGH as needed, depending on what sort of device you have connected
  digitalWrite(out2, LOW);
  digitalWrite(out3, LOW);
  digitalWrite(out4, LOW);

  /*
    digitalWrite(led, LOW);
    delay(500);
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
    digitalWrite(led, HIGH);
  */
  //Serial.begin(115200);
  delay(10);

  //Serial.println(F("RuhNet MQTTS switcher"));

  // Connect to WiFi access point.
  //Serial.println(); Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println();

  //Serial.println("WiFi connected");
  //Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoff1);
  mqtt.subscribe(&onoff2);
  mqtt.subscribe(&onoff3);
  mqtt.subscribe(&onoff4);

}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoff1) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)onoff1.lastread);
      if        (strcmp((char *)onoff1.lastread, "on1") == 0) {
        digitalWrite(out1, HIGH);
      } else if (strcmp((char *)onoff1.lastread, "off1") == 0) {
        digitalWrite(out1, LOW);
      } else if (strcmp((char *)onoff1.lastread, "1") == 0) {
        digitalWrite(out1, HIGH);
      } else if (strcmp((char *)onoff1.lastread, "0") == 0) {
        digitalWrite(out1, LOW);
      } else if (strcmp((char *)onoff1.lastread, "on2") == 0) {
        digitalWrite(out2, HIGH);
      } else if (strcmp((char *)onoff1.lastread, "off2") == 0) {
        digitalWrite(out2, LOW);
      } else if (strcmp((char *)onoff1.lastread, "onall") == 0) {
        digitalWrite(out1, HIGH);
        digitalWrite(out2, HIGH);
      } else if (strcmp((char *)onoff1.lastread, "offall") == 0) {
        digitalWrite(out1, LOW);
        digitalWrite(out2, LOW);
     digitalWrite(out3, LOW);
        digitalWrite(out4, LOW);
      } else if (strcmp((char *)onoff1.lastread, "toggle1") == 0) {
        digitalWrite(out1, !digitalRead(out1));
      } else if (strcmp((char *)onoff1.lastread, "toggle2") == 0) {
        digitalWrite(out2, !digitalRead(out2));
      }
      //Serial.println((char *)onoff1.lastread);
    } else if (subscription == &onoff2) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)onoff2.lastread);
      if        (strcmp((char *)onoff2.lastread, "on1") == 0) {
        digitalWrite(out1, HIGH);
      } else if (strcmp((char *)onoff2.lastread, "off1") == 0) {
        digitalWrite(out1, LOW);
      } else if (strcmp((char *)onoff2.lastread, "on2") == 0) {
        digitalWrite(out2, HIGH);
      } else if (strcmp((char *)onoff2.lastread, "off2") == 0) {
        digitalWrite(out2, LOW);
      } else if (strcmp((char *)onoff2.lastread, "1") == 0) {
        digitalWrite(out2, HIGH);
      } else if (strcmp((char *)onoff2.lastread, "0") == 0) {
        digitalWrite(out2, LOW);
      } else if (strcmp((char *)onoff2.lastread, "onall") == 0) {
        digitalWrite(out1, HIGH);
        digitalWrite(out2, HIGH);
      } else if (strcmp((char *)onoff2.lastread, "offall") == 0) {
        digitalWrite(out1, LOW);
        digitalWrite(out2, LOW);          
        digitalWrite(out3, LOW);
      digitalWrite(out4, LOW);

      } else if (strcmp((char *)onoff2.lastread, "toggle1") == 0) {
        digitalWrite(out1, !digitalRead(out1));
      } else if (strcmp((char *)onoff2.lastread, "toggle2") == 0) {
        digitalWrite(out2, !digitalRead(out2));
      }
      //Serial.println((char *)onoff2.lastread);


    } else if (subscription == &onoff3) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)onoff2.lastread);
      if        (strcmp((char *)onoff3.lastread, "on") == 0) {
        digitalWrite(out3, HIGH);
      } else if (strcmp((char *)onoff3.lastread, "off") == 0) {
        digitalWrite(out3, LOW);
      } else if (strcmp((char *)onoff3.lastread, "on3") == 0) {
        digitalWrite(out3, HIGH);
      } else if (strcmp((char *)onoff3.lastread, "off3") == 0) {
        digitalWrite(out3, LOW);
      } else if (strcmp((char *)onoff3.lastread, "1") == 0) {
        digitalWrite(out3, HIGH);
      } else if (strcmp((char *)onoff3.lastread, "0") == 0) {
        digitalWrite(out3, LOW);
      } else if (strcmp((char *)onoff3.lastread, "toggle") == 0) {
        digitalWrite(out3, !digitalRead(out3));
      }
      //Serial.println((char *)onoff3.lastread);


    } else if (subscription == &onoff4) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)onoff4.lastread);
      if        (strcmp((char *)onoff4.lastread, "on") == 0) {
        digitalWrite(out4, HIGH);
      } else if (strcmp((char *)onoff4.lastread, "off") == 0) {
        digitalWrite(out4, LOW);
      } else if (strcmp((char *)onoff4.lastread, "on4") == 0) {
        digitalWrite(out4, HIGH);
      } else if (strcmp((char *)onoff4.lastread, "off4") == 0) {
        digitalWrite(out4, LOW);
      } else if (strcmp((char *)onoff4.lastread, "1") == 0) {
        digitalWrite(out4, HIGH);
      } else if (strcmp((char *)onoff4.lastread, "0") == 0) {
        digitalWrite(out4, LOW);
      } else if (strcmp((char *)onoff4.lastread, "toggle") == 0) {
        digitalWrite(out4, !digitalRead(out4));
      }
      //Serial.println((char *)onoff4.lastread);

    }
  }

  // Now we can publish stuff!
  //Serial.print(F("\nPublishing status to statusfeed, out1status, out2status: "));
  //Serial.print(statusMessage);
  //Serial.print("\n");
  if (digitalRead(out1) == 0) {
    out1stat = '0';
    out1val = 0;
  } else {
    out1stat = '1';
    out1val = 1;
  }
  if (digitalRead(out2) == 0) {
    out2stat = '0';
    out2val = 0;
  } else {
    out2stat = '1';
    out2val = 1;
  }
  if (digitalRead(out3) == 0) {
    out3stat = '0';
    out3val = 0;
  } else {
    out3stat = '1';
    out3val = 1;
  }
  if (digitalRead(out4) == 0) {
    out4stat = '0';
    out4val = 0;
  } else {
    out4stat = '1';
    out4val = 1;
  }

  out1status.publish(out1val);
  out2status.publish(out2val);
  out3status.publish(out3val);
  out4status.publish(out4val);

  statusMessage[26] = out1stat;
  statusMessage[27] = ' ';
  statusMessage[28] = out2stat;
statusMessage[29] = out3stat;
  statusMessage[30] = ' ';
  statusMessage[31] = out4stat;

  if (! statusfeed.publish(statusMessage)) {
    //Serial.println(F("Failed"));
  } else {
    //Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
    if(! mqtt.ping()) {
    mqtt.disconnect();
    }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  //Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    //Serial.println(mqtt.connectErrorString(ret));
    //Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  //Serial.println("MQTT Connected!");
  mainstatusfeed.publish("Connected!");
}
