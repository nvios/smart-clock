#include "MQTTController.h"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe eventFeed(&mqtt, AIO_USERNAME AIO_FEED_NAME);

String eventTitle = "";
String eventStart = "";
String eventEnd = "";
bool showEvent = false, eventDisplayed = false;
unsigned long eventDisplayStart = 0;

bool mqttConnect(int maxAttempts)
{
  int attempts = 0;
  while (attempts < maxAttempts)
  {
    Serial.print("Attempting MQTT connection (try #");
    Serial.print(attempts + 1);
    Serial.println(")...");

    int8_t ret = mqtt.connect();
    if (ret == 0)
    {
      Serial.println("MQTT Connected!");
      return true;
    }
    else
    {
      Serial.print("MQTT connect failed: ");
      Serial.println(mqtt.connectErrorString(ret));
      mqtt.disconnect();
      delay(5000);
    }
    attempts++;
  }

  Serial.println("Failed to connect to MQTT after max attempts. Continuing without MQTT.");
  return false;
}


void parseEventJSON(const char *payload)
{
  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err)
  {
    Serial.print("JSON parse failed: ");
    Serial.println(err.f_str());
    return;
  }

  eventTitle = doc["subject"] | "";
  eventStart = doc["startTime"] | "";
  eventEnd = doc["endTime"] | "";

  Serial.println("New Event:");
  Serial.print("Title: ");
  Serial.println(eventTitle);
  Serial.print("Start: ");
  Serial.println(eventStart);
  Serial.print("End:   ");
  Serial.println(eventEnd);
}