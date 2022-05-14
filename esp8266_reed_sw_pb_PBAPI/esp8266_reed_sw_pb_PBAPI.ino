#include <Pushbullet.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define PORT 443
#define REED_TO_RESET_PIN 14 // (D5 - GPIO14)
#define REED_SWITCH_PIN 2    // (D4 - GPIO2)
#define YELLOW_LED_PIN 0     // (D3 - GPIO0)

WiFiClientSecure client;
int reedStatus;
bool wifiConnected = true;
char *ssid = "<Your WiFi SSID>";
char *password = "<Your WiFi Password>";
char *pbAccessToken = "<Your Pushbullet Access Token>";

void wifi_setup()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int wi = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (wi > 20)
    {
      wifiConnected = false;
      break;
    }
    wi++;
  }

  if (wifiConnected)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
  else
  {
    Serial.println("");
    Serial.println("Connection failed.");
  }
}

void notify_to_pushbullet()
{
  digitalWrite(YELLOW_LED_PIN, HIGH);

  // Connecting to pushbullet ...
  Serial.println("Connecting to Pushbullet ...");
  PushBullet pb = PushBullet("API", &client, PORT);
  if (!pb.checkConnection())
  {
    Serial.println("Connecting to Pushbullet ... FAILED");
    return;
  }
  Serial.println("Connecting to Pushbullet ... SUCCESS");

  // Sending notification ...
  Serial.println("Sending notification to Pushbullet ...");
  pb.sendNotePush("Home Doorbell", "Doorbell pressed, Someone is/was waiting outside"); //"https://i.imgur.com/QteGU4m.jpg")
}

void setup()
{
  Serial.begin(115200);
  pinMode(REED_SWITCH_PIN, INPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  delay(10);

  reedStatus = digitalRead(REED_SWITCH_PIN);
  if (reedStatus == HIGH)
  {
    Serial.println("Doorbell pressed");
    wifi_setup();
    notify_to_pushbullet();
  }

  // Starting deep sleep ...
  digitalWrite(YELLOW_LED_PIN, LOW);
  Serial.println("Deep-sleeping ...");
  ESP.deepSleep(0);
}

void loop() {}
