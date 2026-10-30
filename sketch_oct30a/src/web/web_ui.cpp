#include "web_ui.h"

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "../core/codec.h"
#include "assets/ui_markup.h"
#include "assets/ui_script.h"
#include "assets/ui_styles.h"

namespace WebUi {

static const char kDocOpen[] PROGMEM =
    "Status: 200\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Cache-Control: no-store\r\n"
    "\r\n"
    "<!DOCTYPE html><html lang=\"en\"><head>"
    "<meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
    "<title>Morning Light</title><style>";

static const char kBodyOpen[] PROGMEM = "</style></head><body>";
static const char kScriptOpen[] PROGMEM = "<script>";
static const char kDocClose[] PROGMEM = "</script></body></html>";

static void writeProgmem(BridgeClient &client, const char *data) {
  char buf[64];
  size_t len = strlen_P(data);
  for (size_t offset = 0; offset < len; offset += sizeof(buf)) {
    size_t chunk = len - offset;
    if (chunk > sizeof(buf)) {
      chunk = sizeof(buf);
    }
    memcpy_P(buf, data + offset, chunk);
    client.write(reinterpret_cast<const uint8_t *>(buf), chunk);
  }
}

static void printQuoted(BridgeClient &client, const char *s) {
  client.print('"');
  client.print(s);
  client.print('"');
}

void sendHomePage(BridgeClient &client, bool ledOn, const char *timeStr,
                  const ScheduleConfig &cfg) {
  char onStr[6];
  char offStr[6];
  char startHex[8];
  char endHex[8];
  Codec::formatTimeInput(cfg.onMinute, onStr);
  Codec::formatTimeInput(cfg.offMinute, offStr);
  Codec::formatHexColor(cfg.startColor, startHex);
  Codec::formatHexColor(cfg.endColor, endHex);

  writeProgmem(client, kDocOpen);
  writeProgmem(client, kUiStyles);
  writeProgmem(client, kBodyOpen);
  writeProgmem(client, kUiMarkup);
  writeProgmem(client, kScriptOpen);
  writeProgmem(client, kUiScript);

  client.print("init(");
  printQuoted(client, timeStr);
  client.print(',');
  client.print(ledOn ? 1 : 0);
  client.print(',');
  printQuoted(client, onStr);
  client.print(',');
  client.print(cfg.fadeInMin);
  client.print(',');
  printQuoted(client, offStr);
  client.print(',');
  client.print(cfg.fadeOutMin);
  client.print(',');
  printQuoted(client, startHex);
  client.print(',');
  client.print(cfg.startBri);
  client.print(',');
  printQuoted(client, endHex);
  client.print(',');
  client.print(cfg.endBri);
  client.print(");");

  writeProgmem(client, kDocClose);
}

void sendPlainText(BridgeClient &client, int status, const char *body) {
  client.print("Status: ");
  client.println(status);
  client.println("Content-Type: text/plain");
  client.println("Cache-Control: no-store");
  client.println();
  client.print(body);
}

}  // namespace WebUi
