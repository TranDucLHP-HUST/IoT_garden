#include <Wire.h>
#include <PubSubClient.h>
#include "SSD1306.h"
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Scheduler.h>

#define WIFI_TIMEOUT 1000
#define LOOP_TIMEOUT 1000

// WiFi
const char *ssid = "Samsung galaxy s9+";    // Enter your WiFi name
const char *password = "0123456789"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "www.maqiatto.com";
const char *topic_subcribe = "honghoang1232@gmail.com/topicTime";
const char *mqtt_username = "honghoang1232@gmail.com";
const char *mqtt_password = "chandattoong99";
const int mqtt_port = 1883;
long unsigned int timenow = 0;

const char *humidity;
const char *temperature;

int timecontrol = 0;
int statusled = 0;
int timeshow=0;

WiFiClient espClient;
PubSubClient client(espClient);

SSD1306 display(0x3c, 4, 5);

void show_data()
{
  int mid = millis();
  Serial.println(timeshow);
  while (mid - timenow < timeshow) //1 lần subcribe dữ liệu
  {
    for (int i = 128; i > -180; i--)
    {
      display.drawString(i, 0, "Nhiet do: ");
      display.drawString(i + 100, 0, temperature);
      display.display();
      delay(10);
      if (i == -170)
      {
        for (int j = 128; j > -170; j--)
        {
          display.drawString(j, 0, "Do am: ");
          display.drawString(j + 80, 0, humidity);
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
    mid = millis();
  }
  timenow = millis();
}

class TurnLed : public Task
{
protected:
  void loop()
  {
    if (statusled == 1 && timecontrol > 0) 
    {
      digitalWrite(16, 1);
      delay(timecontrol);
      Serial.println("off");
      Serial.println(timecontrol);
      digitalWrite(16, 0);

      //tắt đi sau khi hết thời gian
      statusled = 0;
    }
    
  }

} turn_led;

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
  humidity = doc["humidity"];
  temperature = doc["temperature"];

  // Xử lý dữ liệu để hiển thị led
  int number = atoi(humidity);
  Serial.println(number);
  // cập nhật lại các tham số điều khiển, giả sử độ ẩm tốt là 80mm, độ lệch cho nhân 5s tượng trưng
  if (number > 80) 
  {
    timeshow = (number-80) *4000;
    timecontrol = 0;//(number-80)*100;
  }
  else 
  {
    timeshow = (80-number)*4000;
    timecontrol = (80 - number)*100;
  }

  statusled = 1; //bật đèn led
  delay(1000);
  show_data();
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

class MQTTLoop : public Task
{
protected:
  void loop()
  {
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();
  }
} mqtt_loop;

void setup()
{
  Serial.begin(9600);

  display.init();
  display.setFont(ArialMT_Plain_24);

  Serial.setDebugOutput(true);

  setup_wifi();

  // setup led
  pinMode(16, OUTPUT);

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  display.drawString(0, 32, "Team 9-IoT");
  display.display();

  // cho led sáng tượng chưng cho việc tưới tiêu
  Scheduler.start(&turn_led);

  // Hiển thị dữ liệu len màn LCD
  Scheduler.start(&mqtt_loop);
}


void loop()
{
  Scheduler.begin();
}
