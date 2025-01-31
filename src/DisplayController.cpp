#include "DisplayController.h"
#include "Config.h"
#include "Fonts.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupDisplay()
{
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 init failed!");
    for (;;)
    {
    }
  }
  display.clearDisplay();
  display.setFont(&Org_01);
  display.display();
}

int writeText(const char *text,
              int startX,
              int startY,
              int textSize,
              int maxLines)
{
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setTextWrap(false);
  display.setCursor(startX, startY);

  String input(text);
  input.trim();

  int lineCount = 0;
  bool ellipsized = false;
  String line;
  int idx = 0;

  while (idx < (int)input.length() && lineCount < maxLines)
  {
    int spacePos = input.indexOf(' ', idx);
    if (spacePos < 0)
      spacePos = input.length();
    String word = input.substring(idx, spacePos);
    idx = spacePos + 1;
    String testLine = line.isEmpty() ? word : (line + " " + word);

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(testLine.c_str(), 0, 0, &x1, &y1, &w, &h);

    if (w > SCREEN_WIDTH)
    {
      if (lineCount == maxLines - 1)
      {
        while (line.length() > 0)
        {
          display.getTextBounds((line + "...").c_str(), 0, 0, &x1, &y1, &w, &h);
          if (w <= SCREEN_WIDTH)
          {
            line += "...";
            ellipsized = true;
            break;
          }
          line.remove(line.length() - 1);
        }
        display.println(line);
        break;
      }
      display.println(line);
      lineCount++;
      line = word;
    }
    else
    {
      line = testLine;
    }
  }

  if (!ellipsized && line.length() > 0 && lineCount < maxLines)
  {
    display.println(line);
    lineCount++;
  }
  int finalY = display.getCursorY();
  display.display();
  return finalY;
}

void drawUi(int mode, int hour, int min, const char *wDay, int mDay, int voc)
{
  if (mode == 0)
  {
    drawStackedUi(hour, min, wDay, mDay, voc);
  }
  else
  {
    drawLinearUi(hour, min, wDay, mDay, voc);
  }
}

void drawStackedUi(int hour, int min, const char *wDay, int mDay, int voc)
{
  display.clearDisplay();

  // Draw Time
  display.setTextSize(5);
  display.setFont(&Num_01);
  display.setCursor(0, 20);
  display.print(hour < 10 ? "0" + String(hour) : String(hour));
  display.setCursor(0, 50);
  display.print(min < 10 ? "0" + String(min) : String(min));

  // Draw Day + VOC text
  display.setTextSize(1);
  display.setFont(&Org_01);
  display.setCursor(64, 24);
  display.print(wDay);
  display.setCursor(64, 54);
  display.print("VOC");

  // Draw Day + VOC values
  display.setTextSize(voc > 200 ? 2 : 3);
  display.setCursor(86, 22);
  display.print(mDay);
  display.setCursor(86, 52);
  display.print(voc);
}

void drawLinearUi(int hour, int min, const char *wDay, int mDay, int voc)
{
  display.clearDisplay();

  // Draw time
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", hour, min);
  display.setTextSize(5);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

  int posX = (SCREEN_WIDTH - w) / 2;
  display.setCursor(posX, 30);
  display.print(buf);

  // Draw VOC + Temperature + Humidity text
  display.setTextSize(1);
  display.setCursor(12, 56);
  display.print("VOC");
  display.setCursor(72, 56);
  display.print(wDay);

  // Draw VOC + Temperature + Humidity values
  display.setTextSize(2);
  display.setCursor(34, 55);
  display.print(voc);
  display.setCursor(94, 55);
  display.print(mDay);

  display.display();
}

String addHours(String timeInput, int hoursToAdd)
{
  int hours = timeInput.substring(0, 2).toInt();
  int minutes = timeInput.substring(3, 5).toInt();
  hours = (hours + hoursToAdd) % 24;

  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", hours, minutes);
  return String(buf);
}

void drawEventOnDisplay(const String &title,
                        const String &startTime,
                        const String &endTime,
                        int hoursToAdd)
{
  int finalY = writeText(title.c_str());
  display.setTextSize(2);
  display.setCursor(0, 62);

  String sTime = addHours(startTime, hoursToAdd);
  String eTime = addHours(endTime, hoursToAdd);
  display.print(sTime);
  display.print(" > ");
  display.println(eTime);

  display.display();
}