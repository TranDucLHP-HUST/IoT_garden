#include <Wire.h>
#include "SSD1306.h"
#include <SPI.h>
#include <ESP8266WiFi.h>
#include "MQTTConnector.h"
#include "Credentials.h"

#define WIFI_TIMEOUT 1000
#define LOOP_TIMEOUT 5000

SSD1306 display(0x3c, 4, 5);

void WiFiBegin(const char *ssid, const char *pass)
{
  WiFi.begin(ssid, pass);
  Serial.printf("Waiting for AP connection ...\n");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(WIFI_TIMEOUT);
    Serial.printf(".");
  }
  IPAddress ip = WiFi.localIP();
  Serial.printf("\nConnected to AP. IP : %d.%d.%d.%d\n",
                ip[0], ip[1], ip[2], ip[3]);
}

void setup()
{
  Serial.begin(9600);

  display.init();
  display.setFont(ArialMT_Plain_24);

  Serial.setDebugOutput(true);

  WiFiBegin(STA_SSID, STA_PASS);

  MQTTBegin();

  display.drawString(0, 32, "Team 9-IoT");
  display.display();
}

void loop()
{
  // get data by subcribe MQTT MaQ
  MQTTLoop();
  // String myTopic = TOPIC;
  // char charBuf[myTopic.length() + 1];
  // myTopic.toCharArray(charBuf, myTopic.length() + 1);
  // if (MQTTSubscribe(charBuf) )
  // {
  //   Serial.printf("MQTTSubcribe was succeeded.\n");
  // }

  // publish ok
  // if(MQTTPublish(TOPIC, "test maqiatto thui!")==true)
  // {
  //   Serial.printf("MQTTPublish was succeeded.\n");
  // }
  delay(LOOP_TIMEOUT);
  // show data run in Oled screen

  for (int i = 128; i > -180; i--)
  {
    display.drawString(i, 0, "Nhiet do: 100*C");
    display.display();
    delay(10);
    if (i == -170)
    {
      for (int j = 128; j > -170; j--)
      {
        display.drawString(j, 0, "Do am: 100mm");
        display.display();
        delay(10);
        display.clear();
        display.drawString(0, 32, "Team 9-IoT");
        display.display();
      }
    }
    display.clear();
    display.drawString(0, 32, "Team 9-IoT");
    display.display();
  }

  //Tạo 2 luông
  // Setting on off led light
}
