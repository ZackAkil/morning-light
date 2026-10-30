#include "http_router.h"

#include <Arduino.h>
#include <BridgeClient.h>
#include <Process.h>
#include <string.h>
#include "../core/codec.h"
#include "web_ui.h"

static size_t readRequestLine(BridgeClient &client, char *buf, size_t size) {
  size_t i = 0;
  unsigned long start = millis();
  while (millis() - start < 500) {
    while (client.available() > 0) {
      char c = client.read();
      if (c == '\n') {
        while (i > 0 && (buf[i - 1] == '\r' || buf[i - 1] == ' ')) {
          i--;
        }
        buf[i] = '\0';
        return i;
      }
      if (i < size - 1) {
        buf[i++] = c;
      }
    }
    delay(1);
  }
  buf[i] = '\0';
  return i;
}

HttpRouter::HttpRouter(YunClock &clock, AlarmSchedule &schedule, LedStrip &strip)
    : clock_(clock), schedule_(schedule), strip_(strip) {}

void HttpRouter::begin() {
  Process p;
  p.runShellCommand("uci set arduino.@arduino[0].secure_rest_api=false && uci commit arduino && rm -f /tmp/luci-indexcache");

  server_.listenOnLocalhost();
  server_.begin();
}

void HttpRouter::handleClient() {
  BridgeClient client = server_.accept();
  if (!client) {
    return;
  }

  char cmd[80];
  readRequestLine(client, cmd, sizeof(cmd));

  Serial.print(F("[http] /arduino/"));
  Serial.println(cmd);

  if (strcmp(cmd, "toggle") == 0) {
    strip_.setInstant(!strip_.isActive());
    Serial.print(F("[led] "));
    Serial.println(strip_.isActive() ? F("ON") : F("OFF"));
    WebUi::sendPlainText(client, 200, strip_.isActive() ? "ON" : "OFF");
  } else if (strcmp(cmd, "home") == 0) {
    char nowStr[24];
    clock_.formatTime(nowStr, sizeof(nowStr));
    WebUi::sendHomePage(client, strip_.isActive(), nowStr, schedule_.config());
  } else if (strncmp(cmd, "alarm/", 6) == 0) {
    handleAlarm(client, cmd);
  } else if (strncmp(cmd, "test/", 5) == 0) {
    handleTest(client, cmd);
  } else {
    WebUi::sendPlainText(client, 404, "Not found");
  }

  delay(20);
  client.stop();
}

// Route: alarm/<on|none>/<fadeIn>/<off|none>/<fadeOut>/<startHex>/<startBri>/<endHex>/<endBri>
void HttpRouter::handleAlarm(BridgeClient &client, char *cmd) {
  char *savePtr = nullptr;
  char *tok = strtok_r(cmd, "/", &savePtr);
  char *onTok = strtok_r(nullptr, "/", &savePtr);
  char *finTok = strtok_r(nullptr, "/", &savePtr);
  char *offTok = strtok_r(nullptr, "/", &savePtr);
  char *foutTok = strtok_r(nullptr, "/", &savePtr);
  char *scTok = strtok_r(nullptr, "/", &savePtr);
  char *sbTok = strtok_r(nullptr, "/", &savePtr);
  char *ecTok = strtok_r(nullptr, "/", &savePtr);
  char *ebTok = strtok_r(nullptr, "/", &savePtr);

  if (!tok || !onTok || !finTok || !offTok || !foutTok ||
      !scTok || !sbTok || !ecTok || !ebTok) {
    WebUi::sendPlainText(client, 400, "Bad request");
    return;
  }

  ScheduleConfig next;
  if (!Codec::parseTimeToken(onTok, next.onMinute) ||
      !Codec::parseUint8Max(finTok, kMaxFadeMinutes, next.fadeInMin) ||
      !Codec::parseTimeToken(offTok, next.offMinute) ||
      !Codec::parseUint8Max(foutTok, kMaxFadeMinutes, next.fadeOutMin) ||
      !Codec::parseHexColor(scTok, next.startColor) ||
      !Codec::parseUint8Max(sbTok, 100, next.startBri) ||
      !Codec::parseHexColor(ecTok, next.endColor) ||
      !Codec::parseUint8Max(ebTok, 100, next.endBri)) {
    WebUi::sendPlainText(client, 400, "Invalid parameters");
    return;
  }

  schedule_.save(next);
  strip_.applyConfig(next);

  Serial.print(F("[alarm] saved on="));
  Serial.print(onTok);
  Serial.print(F(" off="));
  Serial.println(offTok);

  WebUi::sendPlainText(client, 200, "OK");
}

// Route: test/<hex6>/<briPct>
void HttpRouter::handleTest(BridgeClient &client, char *cmd) {
  char *savePtr = nullptr;
  char *tok = strtok_r(cmd, "/", &savePtr);
  char *hexTok = strtok_r(nullptr, "/", &savePtr);
  char *briTok = strtok_r(nullptr, "/", &savePtr);

  RgbColor color;
  uint8_t briPct = 0;
  if (!tok || !hexTok || !briTok ||
      !Codec::parseHexColor(hexTok, color) ||
      !Codec::parseUint8Max(briTok, 100, briPct)) {
    WebUi::sendPlainText(client, 400, "Invalid test parameters");
    return;
  }

  strip_.preview(color, briPct);
  WebUi::sendPlainText(client, 200, strip_.isActive() ? "ON" : "OFF");
}
