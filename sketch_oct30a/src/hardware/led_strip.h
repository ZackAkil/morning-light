#pragma once

#include <Adafruit_NeoPixel.h>
#include <stdint.h>
#include "../core/types.h"

class LedStrip {
public:
  static const uint8_t kPin = 6;
  static const uint16_t kCount = 21;

  LedStrip();

  void begin(const ScheduleConfig &cfg);
  void blinkReady();

  void applyConfig(const ScheduleConfig &cfg, bool refreshActive = true);
  void setInstant(bool on);
  void preview(const RgbColor &color, uint8_t briPct);
  void startSequence(bool toOn, uint8_t durationMin);

  bool update();
  bool isActive() const { return active_; }

private:
  Adafruit_NeoPixel pixels_;
  RgbColor startColor_ = {255, 59, 0};
  uint8_t startBri_ = 5;
  RgbColor endColor_ = {255, 214, 153};
  uint8_t endBri_ = 100;

  bool active_ = false;
  bool fading_ = false;
  bool previewing_ = false;
  bool targetOn_ = false;
  uint16_t currentPermille_ = 0;
  uint16_t fromPermille_ = 0;
  uint16_t targetPermille_ = 0;
  unsigned long fadeStartMs_ = 0;
  uint32_t fadeDurationDecisec_ = 0;

  void setActiveState(bool active);
  void clearStrip();
  void renderSequenceStep(uint16_t u);
  void renderColorAtBrightness(const RgbColor &c, uint16_t briPermille);
};
