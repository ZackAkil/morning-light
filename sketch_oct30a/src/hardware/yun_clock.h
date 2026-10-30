#pragma once

#include <stddef.h>
#include <stdint.h>

class YunClock {
public:
  void begin();
  void update();

  int16_t currentMinuteOfDay() const;
  void formatTime(char *out, size_t outSize) const;

private:
  static const unsigned long kSyncIntervalMs = 60000UL;

  bool synced_ = false;
  uint32_t baseSeconds_ = 0;
  unsigned long syncMillis_ = 0;
  char tz_[10] = {0};

  uint32_t currentSecondOfDay() const;
  void sync();
};
