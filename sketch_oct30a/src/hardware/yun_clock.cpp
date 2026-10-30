#include "yun_clock.h"

#include <Arduino.h>
#include <Process.h>
#include <string.h>
#include "../core/codec.h"

void YunClock::begin() {
  sync();
}

void YunClock::update() {
  if (!synced_ || (millis() - syncMillis_ >= kSyncIntervalMs)) {
    sync();
  }
}

int16_t YunClock::currentMinuteOfDay() const {
  if (!synced_) {
    return -1;
  }
  return static_cast<int16_t>((currentSecondOfDay() / 60UL) % 1440UL);
}

void YunClock::formatTime(char *out, size_t outSize) const {
  if (outSize == 0) {
    return;
  }
  if (!synced_) {
    strncpy(out, "--:--:--", outSize - 1);
    out[outSize - 1] = '\0';
    return;
  }
  Codec::formatClockTime(currentSecondOfDay(), tz_, out, outSize);
}

uint32_t YunClock::currentSecondOfDay() const {
  uint32_t elapsed = (millis() - syncMillis_) / 1000UL;
  return (baseSeconds_ + elapsed) % 86400UL;
}

void YunClock::sync() {
  Process p;
  p.runShellCommand("date '+%H %M %S %Z'");

  char buf[32];
  size_t i = 0;
  while (p.available() > 0 && i < sizeof(buf) - 1) {
    char c = p.read();
    if (c == '\r' || c == '\n') {
      break;
    }
    buf[i++] = c;
  }
  buf[i] = '\0';

  if (i < 8 || buf[2] != ' ' || buf[5] != ' ') {
    return;
  }

  int hh = (buf[0] - '0') * 10 + (buf[1] - '0');
  int mm = (buf[3] - '0') * 10 + (buf[4] - '0');
  int ss = (buf[6] - '0') * 10 + (buf[7] - '0');
  if (hh < 0 || hh > 23 || mm < 0 || mm > 59 || ss < 0 || ss > 59) {
    return;
  }

  baseSeconds_ = static_cast<uint32_t>(hh) * 3600UL +
                 static_cast<uint32_t>(mm) * 60UL +
                 static_cast<uint32_t>(ss);
  syncMillis_ = millis();
  synced_ = true;

  size_t t = 0;
  if (buf[8] == ' ') {
    const char *src = buf + 9;
    while (*src && t < sizeof(tz_) - 1) {
      tz_[t++] = *src++;
    }
  }
  tz_[t] = '\0';
}
