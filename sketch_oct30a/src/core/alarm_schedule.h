#pragma once

#include <stdint.h>
#include "types.h"

class AlarmSchedule {
public:
  AlarmSchedule();

  void begin();
  void save(const ScheduleConfig &next);
  const ScheduleConfig &config() const { return cfg_; }

  bool poll(int16_t currentMinute, bool &outTurnOn);

private:
  static const uint8_t kEepromMagic = 0xA4;

  ScheduleConfig cfg_;
  int16_t lastMinute_ = -1;

  void setDefaults();
};
