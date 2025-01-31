#include "LEDController.h"
Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

LEDState currentLEDState = LEDState::IDLE;
unsigned long stateStartTime = 0;

static const unsigned long EVENT_ANIMATION_MS = 60000;
static const unsigned long VOC_ANIMATION_MS = 60000;
static const unsigned long VOC_RED_PULSE_MS = 300000;

static uint8_t activeLed = 0;
static unsigned long lastTailUpdate = 0;

static uint8_t pulseVal = 0;
static bool pulseRising = true;

void initLED()
{
  ring.begin();
  ring.clear();
  ring.show();
  currentLEDState = LEDState::IDLE;
}

void setLEDState(LEDState newState)
{
  currentLEDState = newState;
  stateStartTime = millis();
  // Reset animation variables
  activeLed = 0;
  lastTailUpdate = 0;
  pulseVal = 0;
  pulseRising = true;
}

void animateTailFade(std::function<uint32_t(int)> colorFunc, uint8_t tailLen = 9, unsigned long stepTime = 120)
{
  unsigned long now = millis();
  if (now - lastTailUpdate >= stepTime)
  {
    lastTailUpdate = now;
    activeLed = (activeLed + 1) % LED_COUNT;

    ring.clear();
    for (int i = 0; i < LED_COUNT; i++)
    {
      int dist = (activeLed + LED_COUNT - i) % LED_COUNT;
      if (dist < tailLen)
      {
        uint8_t b = ((tailLen - 1 - dist) * 255) / (tailLen - 1);
        uint32_t color = colorFunc(i);

        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b_ = (color) & 0xFF;

        r = (r * b) / 255;
        g = (g * b) / 255;
        b_ = (b_ * b) / 255;
        ring.setPixelColor(i, r, g, b_);
      }
    }
    ring.show();
  }
}

static void animateRainbowTail()
{
  animateTailFade(
      [&](int i)
      {
        uint16_t pixelHue = i * (65535L / LED_COUNT);
        return ring.ColorHSV(pixelHue, 255, 255);
      });
}

static void animateSolidTail(uint8_t R, uint8_t G, uint8_t B)
{
  animateTailFade([&](int i)
                  { return ring.Color(R, G, B); });
}

static void animateRedPulse()
{
  if (pulseRising)
  {
    pulseVal += 5;
    if (pulseVal >= 255)
    {
      pulseVal = 255;
      pulseRising = false;
    }
  }
  else
  {
    pulseVal -= 5;
    if (pulseVal <= 0)
    {
      pulseVal = 0;
      pulseRising = true;
    }
  }

  ring.fill(ring.Color(pulseVal, 0, 0));
  ring.show();
}

void updateLED()
{
  unsigned long now = millis();

  switch (currentLEDState)
  {
  case LEDState::EVENT_RAINBOW:
    if (now - stateStartTime >= EVENT_ANIMATION_MS)
    {
      currentLEDState = LEDState::IDLE;
    }
    else
    {
      animateRainbowTail();
    }
    break;

  case LEDState::VOC_GREEN:
    if (now - stateStartTime >= VOC_ANIMATION_MS)
    {
      currentLEDState = LEDState::IDLE;
    }
    else
    {
      animateSolidTail(0, 255, 0);
    }
    break;

  case LEDState::VOC_YELLOW:
    if (now - stateStartTime >= VOC_ANIMATION_MS)
    {
      currentLEDState = LEDState::IDLE;
    }
    else
    {
      animateSolidTail(255, 255, 0);
    }
    break;

  case LEDState::VOC_RED:
    if (now - stateStartTime >= VOC_ANIMATION_MS)
    {
      currentLEDState = LEDState::VOC_RED_PULSE;
      stateStartTime = now;
    }
    else
    {
      animateSolidTail(255, 0, 0);
    }
    break;

  case LEDState::VOC_RED_PULSE:
    if (now - stateStartTime >= VOC_RED_PULSE_MS)
    {
      currentLEDState = LEDState::IDLE;
    }
    else
    {
      animateRedPulse();
    }
    break;

  case LEDState::IDLE:
  default:
    ring.clear();
    ring.show();
    break;
  }
}