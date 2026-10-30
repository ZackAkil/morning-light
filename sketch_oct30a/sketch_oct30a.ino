#include <Bridge.h>
#include "src/core/alarm_schedule.h"
#include "src/hardware/led_strip.h"
#include "src/hardware/yun_clock.h"
#include "src/web/http_router.h"

static YunClock yunClock;
static AlarmSchedule schedule;
static LedStrip strip;
static HttpRouter router(yunClock, schedule, strip);

void setup() {
  schedule.begin();
  strip.begin(schedule.config());

  Serial.begin(115200);
  Bridge.begin();

  yunClock.begin();
  router.begin();
  strip.blinkReady();

  Serial.println(F("[boot] Ready at http://<yun-ip>/arduino/home"));
}

void loop() {
  yunClock.update();

  if (strip.update()) {
    Serial.print(F("[fade] Complete -> "));
    Serial.println(strip.isActive() ? F("ON") : F("OFF"));
  }

  bool turnOn = false;
  if (schedule.poll(yunClock.currentMinuteOfDay(), turnOn)) {
    const ScheduleConfig &cfg = schedule.config();
    strip.startSequence(turnOn, turnOn ? cfg.fadeInMin : cfg.fadeOutMin);
    Serial.print(F("[alarm] Starting sequence -> "));
    Serial.println(turnOn ? F("ON") : F("OFF"));
  }

  router.handleClient();
  delay(50);
}
