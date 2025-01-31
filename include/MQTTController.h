#ifndef MQTT_CONTROLLER_H
#define MQTT_CONTROLLER_H

#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ArduinoJson.h>

#include "Config.h"

extern WiFiClient client;
extern Adafruit_MQTT_Client mqtt;
extern Adafruit_MQTT_Subscribe eventFeed;

extern String eventTitle;
extern String eventStart;
extern String eventEnd;
extern bool   showEvent;
extern unsigned long eventDisplayStart;
extern bool eventDisplayed;

bool mqttConnect(int maxAttempts);
void parseEventJSON(const char* payload);

#endif