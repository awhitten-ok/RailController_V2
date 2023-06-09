#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "stepperController.h"

const char *ssid = "Whittenwifi";
const char *password = "AmWhitten2016";
const char *mqttServer = "192.168.1.73";
const char *mqttUser = NULL;
const char *mqttPassword = NULL;
int mqttPort = 1883;

WiFiClient espClient;

PubSubClient client(espClient);

stepperController stepperController;
const char *pTopic = "DRK2/response";
const char *sTopic = "DRK2/command";
const char *user = "DRK2";

void wifiConnect()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Loops until WiFi is connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
    yield();
  }
  Serial.println("Connected to the WiFi network");
  WiFi.setAutoReconnect(true);
}

void mqttConnect()
{
  // Initialize Mqtt Client
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Loops until Connected to Mqtt Broker/Server
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect(user, mqttUser, mqttPassword, pTopic, 0, false, "-1"))
    {

      // Change ClientID for each rail
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(1000);
    }
    yield();
  }
  yield();
}

void mqttSubscribe()
{
  while (!client.subscribe(sTopic))
  {
    Serial.println("Subscribing to " + *sTopic);
    yield();
  }
  yield();
}

// Callback fuction for mqtt client
// All Communication Logic goes here
void callback(char *topic, byte *payload, unsigned int length)
{
  String tempMessage = payloadToString(payload, length);
  String message;
  if (tempMessage.startsWith("moveToPosition?"))
  {

    tempMessage.remove(0, tempMessage.indexOf("?") + 1);

    const unsigned int movementParam = tempMessage.toInt();

    Serial.println("Received command to move to " + tempMessage);

    message = stepperController.moveToPosition(movementParam);
  }
  else if (tempMessage.equals("areYouReady"))
  {
    message = "99";
    yield();
  }
  else if (tempMessage.equals("calibrate"))
  {
    message = "99";
    stepperController.calibration();
    yield();
  }
  else if (tempMessage.equals("currentPosition"))
  {
    int position = stepperController.getCurrentPosition();
    message = String(position);
  }
  char messageBuffer[message.length() + 1];
  message = message + '\0';  message.toCharArray(messageBuffer, message.length());
  client.publish(pTopic, messageBuffer);
  yield();
}

// Converts MqttMessage payload to String
String payloadToString(byte *payload, unsigned int length)
{
  String message = "";
  unsigned int i = 0;
  for (i = 0; i < length; i++)
  {
    message = message + (char)payload[i];

    yield();
  }
  message = message + '\0';
  Serial.println(message);
  yield();
  return message;
}

void setup()
{
  Serial.begin(115200);
  stepperController.stepperSetup();
  stepperController.calibration();
  wifiConnect();
  mqttConnect();
  mqttSubscribe();
}

void loop()
{
   if (client.connected() == false){
    wifiConnect();
    yield();
  }
  client.loop();
  yield();
}
