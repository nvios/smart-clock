#ifndef CONFIG_H
#define CONFIG_H

/******************** Wi-Fi Credentials ************************************/
#define WLAN_SSID  "Wokwi-GUEST"
#define WLAN_PASS  ""

/******************** Adafruit IO Credentials ******************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "luca_bontempi"
#define AIO_KEY         "aio_XXX"
#define AIO_FEED_NAME   "/feeds/calendar"

/******************** CET (UTC+1) Offset ************************************/
constexpr const char* ntpServer    = "pool.ntp.org";
constexpr long        gmtOffsetSec = 3600;  // CET=UTC+1
constexpr int         daylightOffsetSec = 0;

/******************** OLED and Pin Definitions *****************************/
#define SDA_PIN        8
#define SCL_PIN        9
#define BUTTON_PIN     6
#define LED_PIN        4
#define LED_COUNT     16
#define OLED_RESET    -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/******************** Event Timing *****************************************/
constexpr unsigned long EVENT_DISPLAY_DURATION_MS = 60000; // 1 minute
constexpr unsigned long MQTT_RETRY_FREQUENCY_MS = 3600000; // 1 hour

#endif