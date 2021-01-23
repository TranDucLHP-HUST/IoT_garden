#include <Wire.h>
#include <PubSubClient.h>
#include "SSD1306.h"
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define WIFI_TIMEOUT 1000
#define LOOP_TIMEOUT 1000

// WiFi
const char *ssid = "22 Tan Lac";    // Enter your WiFi name
const char *password = "244466666"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "www.maqiatto.com";
const char *topic_subcribe = "honghoang1232@gmail.com/topicTime";
const char *mqtt_username = "honghoang1232@gmail.com";
const char *mqtt_password = "chandattoong99";
const int mqtt_port = 1883;
long unsigned int timenow = 0;

WiFiClient espClient;
PubSubClient client(espClient);


SSD1306 display(0x3c, 4, 5);


void showdata(const char* h, const char* t)
{
  Serial.println("Time show data: ");
  Serial.println(millis());
  for (int i = 128; i > -180; i--)
  {
    display.drawString(i, 0, "Nhiet do: ");
    display.drawString(i+100, 0, t);
    display.display();
    delay(10);
    if (i == -170)
    {
      for (int j = 128; j > -170; j--)
      {
        display.drawString(j, 0, "Do am: ");
        display.drawString(j+80, 0, h);
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
}


void turnled()
{
  Serial.println("Time show led: ");
  Serial.println(millis());
  digitalWrite(16,1);
  delay(3000);
  digitalWrite(16,0);
}

// Hàm kết nối wifi
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Hàm call back để nhận dữ liệu
void callback(char *topic_subcribe, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic_subcribe);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // 2 luồng ở đây, 1 luồng hiển thị LCD, 1 luồng nháy led

  // Hiển thị LCD
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
  const char* humidity = doc["humidity"];
  const char* temperature = doc["temperature"];
  
  // Xử lý dữ liệu để hiển thị led

  // thay vì xử lý đa luồng, sẽ cho led sáng trước tượng chưng cho việc tưới tiêu, sau đó mới hiển thị dữ liệu
  // turnled();
  // show data
  int mid = millis();
  while (mid - timenow < 20000) //300000: 5p 1 lần subcribe dữ liệu
  {
    showdata(humidity, temperature);
    mid=millis();
  }
  timenow = millis();
  
  

}

void reconnect()
{
  // Chờ tới khi kết nối
  while (!client.connected())
  {
    Serial.print("\nConnecting to ");
    Serial.print(mqtt_broker);
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password))
    {
      Serial.println("\nConnected!");
      client.subscribe(topic_subcribe);
    }
    else
    {
      Serial.println(" fail, try again in 5 seconds");
      // Đợi 5s
      delay(10000);
    }
  }
}

void setup()
{
  Serial.begin(9600);

  display.init();
  display.setFont(ArialMT_Plain_24);

  Serial.setDebugOutput(true);

  setup_wifi();

  // setup led
  pinMode(16,OUTPUT);

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  display.drawString(0, 32, "Team 9-IoT");
  display.display();

}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
