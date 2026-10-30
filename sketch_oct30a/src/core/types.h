#pragma once

#include <stdint.h>

static const uint8_t kMaxFadeMinutes = 180;

struct RgbColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct ScheduleConfig {
  int16_t onMinute;
  int16_t offMinute;
  uint8_t fadeInMin;
  uint8_t fadeOutMin;
  RgbColor startColor;
  uint8_t startBri;
  RgbColor endColor;
  uint8_t endBri;
};
