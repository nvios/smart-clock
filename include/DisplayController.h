#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

extern Adafruit_SSD1306 display;

void setupDisplay();

int writeText(const char *text,
              int startX = 0,
              int startY = 8,
              int textSize = 2,
              int maxLines = 3);

void drawUi(int mode, int hour, int min, const char *wDay, int mDay, int voc);
void drawStackedUi(int hour, int min, const char *wDay, int mDay, int voc);
void drawLinearUi(int hour, int min, const char *wDay, int mDay, int voc);

void drawEventOnDisplay(const String &title,
                        const String &startTime,
                        const String &endTime,
                        int hoursToAdd);

#endif