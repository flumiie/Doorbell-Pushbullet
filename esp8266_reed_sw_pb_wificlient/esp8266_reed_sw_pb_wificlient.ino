#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define REED_TO_RESET_PIN 14 // (D5 - GPIO14)
#define REED_SWITCH_PIN 2    // (D4 - GPIO2)
#define YELLOW_LED_PIN 0     // (D3 - GPIO0)

char *ssid = "<Your WiFi SSID>";
char *password = "<Your WiFi Password>";
char *host = "api.pushingbox.com";
char deviceID = "<Your PushingBox scenario's device ID>";
int reedStatus;

void wifi_setup()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void notify_to_pushbullet(char IDs[])
{
  Serial.println("Sending notification to Pushbullet ...");

  for (int count = 0; count < sizeof(IDs); count++)
  {
    digitalWrite(YELLOW_LED_PIN, HIGH);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort))
    {
      Serial.println("!! Connection failed !!");
      return;
    }

    // We now create a URI for the request
    String url = "/pushingbox";
    url += "?devid=";
    url += IDs[count];

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    // Read all the lines of the reply from server and print them to Serial
    while (client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }

  Serial.println();
  Serial.println("closing connection");

  digitalWrite(YELLOW_LED_PIN, LOW);
}

void setup()
{
  Serial.begin(115200);
  pinMode(REED_SWITCH_PIN, INPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  //  digitalWrite(REED_SWITCH_PIN, HIGH);
  delay(10);

  reedStatus = digitalRead(REED_SWITCH_PIN);
  if (reedStatus == HIGH)
  {
    Serial.println("Doorbell pressed");
    wifi_setup();
    notify_to_pushbullet(deviceID);
  }
  Serial.println("Deep-sleeping ...");
  ESP.deepSleep(0);
}

void loop() {}
