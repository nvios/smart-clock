#include "Config.h"
#include "MQTTController.h"
#include "DisplayController.h"
#include "LEDController.h"

static unsigned long lastLedUpdate = 0, lastDisplayUpdate = 0, lastMqttCheck = 0;
bool uiToggleState = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 0;

void setup()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  delay(1000);

  setupDisplay();
  writeText("Spinning up the connection...");

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  writeText("Browsing your calendar...");

  mqtt.subscribe(&eventFeed);
  bool connected = mqttConnect(1);
  if (!connected)
  {
    Serial.println("MQTT connection failed; continuing anyway.");
    writeText("No luck with accessing the calendar :(");
  }

  writeText("Checking the time...");
  configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);

  unsigned long tStart = millis();
  while (time(nullptr) < 10000)
  {
    if (millis() - tStart > 10000)
    {
      Serial.println("Time sync failed, continuing anyway.");
      writeText("Time sync failed :(");
      break;
    }
    delay(1000);
  }
}

void loop()
{
  unsigned long now = millis();
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState != lastButtonState)
  {
    lastDebounceTime = now;
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (buttonState == HIGH && lastButtonState == LOW)
    {
      uiToggleState = !uiToggleState;
    }
    lastButtonState = buttonState;
  }

  if (now - lastLedUpdate >= 50)
  {
    lastLedUpdate = now;
    updateLED();
  }

  if (now - lastDisplayUpdate >= 1000)
  {
    lastDisplayUpdate = now;
    if (mqtt.connected())
    {
      Adafruit_MQTT_Subscribe *sub = mqtt.readSubscription(50);
      if (sub == &eventFeed)
      {
        parseEventJSON((char *)eventFeed.lastread);
        showEvent = true;
        eventDisplayStart = now;
      }
    }

    if (showEvent && (now - eventDisplayStart >= EVENT_DISPLAY_DURATION_MS))
    {
      setLEDState(LEDState::IDLE);
      showEvent = false;
      eventDisplayed = false;
      eventTitle = eventStart = eventEnd = "";
    }

    if (showEvent)
    {
      if (!eventDisplayed)
      {
        setLEDState(LEDState::EVENT_RAINBOW);
        display.clearDisplay();
        drawEventOnDisplay(eventTitle, eventStart, eventEnd, 1);
        display.display();
        eventDisplayed = true;
      }
    }
    else
    {
      time_t t = time(nullptr);
      struct tm *ti = localtime(&t);
      const char *daysOfWeek[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
      drawUi(uiToggleState, ti->tm_hour, ti->tm_min, daysOfWeek[ti->tm_wday], ti->tm_mday, readVocSensor());
      display.display();
    }
  }

  if (now - lastMqttCheck >= MQTT_RETRY_FREQUENCY_MS)
    if (!mqtt.connected())
      mqttConnect(3);
}

const int readVocSensor()
{
  static unsigned long lastVocUpdate = 0;
  static int cycleIndex = 0;
  static const int mockVals[3] = {12, 93, 248};
  unsigned long now = millis();
  if (now - lastVocUpdate >= 10000)
  {
    lastVocUpdate = now;
    cycleIndex = (cycleIndex + 1) % 3;
    setLEDState(
        cycleIndex == 0 ? LEDState::VOC_GREEN : cycleIndex == 1 ? LEDState::VOC_YELLOW
                                                                : LEDState::VOC_RED);
  }
  return mockVals[cycleIndex];
}