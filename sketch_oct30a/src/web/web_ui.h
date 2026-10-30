#pragma once

#include <BridgeClient.h>
#include "../core/types.h"

namespace WebUi {

void sendHomePage(BridgeClient &client, bool ledOn, const char *timeStr,
                  const ScheduleConfig &cfg);
void sendPlainText(BridgeClient &client, int status, const char *body);

}  // namespace WebUi
