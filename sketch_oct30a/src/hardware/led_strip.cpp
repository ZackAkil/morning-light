#include "led_strip.h"

#include <Arduino.h>

LedStrip::LedStrip() : pixels_(kCount, kPin, NEO_GRB + NEO_KHZ800) {}

void LedStrip::begin(const ScheduleConfig &cfg) {
  pinMode(LED_BUILTIN, OUTPUT);
  setActiveState(false);

  applyConfig(cfg, false);
  pixels_.begin();
  pixels_.setBrightness(255);
  clearStrip();
}

void LedStrip::blinkReady() {
  for (uint8_t i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
  }
  digitalWrite(LED_BUILTIN, active_ ? HIGH : LOW);
}

void LedStrip::applyConfig(const ScheduleConfig &cfg, bool refreshActive) {
  startColor_ = cfg.startColor;
  startBri_ = cfg.startBri;
  endColor_ = cfg.endColor;
  endBri_ = cfg.endBri;
  if (refreshActive && active_ && !previewing_) {
    renderSequenceStep(currentPermille_);
  }
}

void LedStrip::setInstant(bool on) {
  fading_ = false;
  previewing_ = false;
  setActiveState(on);
  currentPermille_ = on ? 1000 : 0;
  if (on) {
    renderColorAtBrightness(endColor_, static_cast<uint16_t>(endBri_) * 10U);
  } else {
    clearStrip();
  }
}

void LedStrip::preview(const RgbColor &color, uint8_t briPct) {
  fading_ = false;
  previewing_ = true;
  setActiveState(briPct > 0);
  currentPermille_ = active_ ? 1000 : 0;
  renderColorAtBrightness(color, static_cast<uint16_t>(briPct) * 10U);
}

void LedStrip::startSequence(bool toOn, uint8_t durationMin) {
  previewing_ = false;
  if (durationMin == 0) {
    setInstant(toOn);
    return;
  }

  targetOn_ = toOn;
  fromPermille_ = active_ ? currentPermille_ : (toOn ? 0 : 1000);
  targetPermille_ = toOn ? 1000 : 0;

  if (!active_ && !toOn) {
    fading_ = false;
    clearStrip();
    return;
  }

  setActiveState(true);
  currentPermille_ = fromPermille_;
  renderSequenceStep(currentPermille_);

  if (fromPermille_ == targetPermille_) {
    fading_ = false;
    if (!toOn) {
      setActiveState(false);
      clearStrip();
    }
    return;
  }

  fadeStartMs_ = millis();
  fadeDurationDecisec_ = static_cast<uint32_t>(durationMin) * 600UL;
  fading_ = true;
}

bool LedStrip::update() {
  if (!fading_) {
    return false;
  }

  uint32_t elapsedDecisec = (millis() - fadeStartMs_) / 100UL;
  if (elapsedDecisec >= fadeDurationDecisec_) {
    fading_ = false;
    currentPermille_ = targetPermille_;
    if (targetOn_) {
      setActiveState(true);
      renderSequenceStep(1000);
    } else {
      setActiveState(false);
      clearStrip();
    }
    return true;
  }

  int32_t span = static_cast<int32_t>(targetPermille_) - static_cast<int32_t>(fromPermille_);
  uint16_t next = static_cast<uint16_t>(
      static_cast<int32_t>(fromPermille_) +
      (span * static_cast<int32_t>(elapsedDecisec)) / static_cast<int32_t>(fadeDurationDecisec_));

  if (next != currentPermille_) {
    currentPermille_ = next;
    renderSequenceStep(currentPermille_);
  }
  return false;
}

void LedStrip::setActiveState(bool active) {
  active_ = active;
  digitalWrite(LED_BUILTIN, active_ ? HIGH : LOW);
}

void LedStrip::clearStrip() {
  pixels_.clear();
  pixels_.show();
}

void LedStrip::renderSequenceStep(uint16_t u) {
  RgbColor c;
  c.r = static_cast<uint8_t>(startColor_.r + ((int32_t)(endColor_.r - startColor_.r) * u) / 1000L);
  c.g = static_cast<uint8_t>(startColor_.g + ((int32_t)(endColor_.g - startColor_.g) * u) / 1000L);
  c.b = static_cast<uint8_t>(startColor_.b + ((int32_t)(endColor_.b - startColor_.b) * u) / 1000L);

  int32_t sBri = static_cast<int32_t>(startBri_) * 10L;
  int32_t eBri = static_cast<int32_t>(endBri_) * 10L;
  uint16_t briPermille = static_cast<uint16_t>(sBri + ((eBri - sBri) * u) / 1000L);

  renderColorAtBrightness(c, briPermille);
}

void LedStrip::renderColorAtBrightness(const RgbColor &c, uint16_t briPermille) {
  if (briPermille == 0) {
    clearStrip();
    return;
  }

  int32_t env = (static_cast<int32_t>(briPermille) * (250L + (750L * briPermille) / 1000L)) / 1000L;

  uint8_t r = static_cast<uint8_t>((static_cast<int32_t>(c.r) * env + 999L) / 1000L);
  uint8_t g = static_cast<uint8_t>((static_cast<int32_t>(c.g) * env + 999L) / 1000L);
  uint8_t b = static_cast<uint8_t>((static_cast<int32_t>(c.b) * env + 999L) / 1000L);

  pixels_.fill(pixels_.Color(r, g, b), 0, kCount);
  pixels_.show();
}
