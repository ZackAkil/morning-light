#include "alarm_schedule.h"

#include <EEPROM.h>

AlarmSchedule::AlarmSchedule() {
  setDefaults();
}

void AlarmSchedule::begin() {
  if (EEPROM.read(0) == kEepromMagic) {
    ScheduleConfig loaded;
    EEPROM.get(1, loaded);
    if (loaded.onMinute >= -1 && loaded.onMinute < 1440 &&
        loaded.offMinute >= -1 && loaded.offMinute < 1440 &&
        loaded.fadeInMin <= kMaxFadeMinutes &&
        loaded.fadeOutMin <= kMaxFadeMinutes &&
        loaded.startBri <= 100 &&
        loaded.endBri <= 100) {
      cfg_ = loaded;
    }
  }
}

void AlarmSchedule::save(const ScheduleConfig &next) {
  cfg_ = next;
  EEPROM.update(0, kEepromMagic);
  EEPROM.put(1, cfg_);
}

bool AlarmSchedule::poll(int16_t currentMinute, bool &outTurnOn) {
  if (currentMinute < 0) {
    return false;
  }
  if (lastMinute_ < 0) {
    lastMinute_ = currentMinute;
    return false;
  }
  if (currentMinute == lastMinute_) {
    return false;
  }

  int16_t diff = (currentMinute - lastMinute_ + 1440) % 1440;
  bool triggered = false;

  if (diff <= 5) {
    while (lastMinute_ != currentMinute) {
      lastMinute_ = (lastMinute_ + 1) % 1440;
      if (cfg_.onMinute >= 0 && lastMinute_ == cfg_.onMinute) {
        outTurnOn = true;
        triggered = true;
      }
      if (cfg_.offMinute >= 0 && lastMinute_ == cfg_.offMinute) {
        outTurnOn = false;
        triggered = true;
      }
    }
  } else {
    lastMinute_ = currentMinute;
  }

  return triggered;
}

void AlarmSchedule::setDefaults() {
  cfg_.onMinute = -1;
  cfg_.offMinute = -1;
  cfg_.fadeInMin = 15;
  cfg_.fadeOutMin = 15;
  cfg_.startColor = {255, 59, 0};    // #ff3b00 warm sunrise
  cfg_.startBri = 5;
  cfg_.endColor = {255, 214, 153};   // #ffd699 bright warm sun
  cfg_.endBri = 100;
}
