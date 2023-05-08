#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "OpenKey_Wifi";
const char *password = "OpenKey is #1 in keys!";
const char *mqttServer = "192.168.1.152";
const char *mqttUser = NULL;
const char *mqttPassword = NULL;
int mqttPort = 1883;

void wifiConnect()
{

  // Deprecate
  // WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  const char *pTopic = "DRK2/response";
  const char *sTopic = "DRK2/command";
  const char *user = "DRK2";

  // Loops until WiFi is connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
    yield();
  }
  Serial.println("Connected to the WiFi network");
  WiFi.setAutoReconnect(true);

  // Deprecate
  // WiFi.persistent(false);

  // Initialize Mqtt Client
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Loops until Connected to Mqtt Broker/Server
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect(user, mqttUser, mqttPassword, pTopic, 0, false, "-1"))
    { // Change ClientID for each rail

      Serial.println("connected");
    }
    else
    {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(1000);
    }
    // delay(0);
    yield();
  }
  client.subscribe(sTopic);
  yield();
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
