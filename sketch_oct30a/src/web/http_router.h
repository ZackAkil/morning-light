#pragma once

#include <BridgeServer.h>
#include "../core/alarm_schedule.h"
#include "../hardware/led_strip.h"
#include "../hardware/yun_clock.h"

class HttpRouter {
public:
  HttpRouter(YunClock &clock, AlarmSchedule &schedule, LedStrip &strip);

  void begin();
  void handleClient();

private:
  BridgeServer server_;
  YunClock &clock_;
  AlarmSchedule &schedule_;
  LedStrip &strip_;

  void handleAlarm(BridgeClient &client, char *cmd);
  void handleTest(BridgeClient &client, char *cmd);
};
