#include "codec.h"

#include <string.h>

namespace Codec {

static int hexVal(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
  if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
  return -1;
}

static void putHexByte(uint8_t b, char *dst) {
  static const char kHexDigits[] = "0123456789abcdef";
  dst[0] = kHexDigits[(b >> 4) & 0x0F];
  dst[1] = kHexDigits[b & 0x0F];
}

bool parseTimeToken(const char *s, int16_t &outMinute) {
  if (strcmp(s, "none") == 0) {
    outMinute = -1;
    return true;
  }
  if (strlen(s) != 4) {
    return false;
  }
  for (uint8_t i = 0; i < 4; i++) {
    if (s[i] < '0' || s[i] > '9') {
      return false;
    }
  }
  int hh = (s[0] - '0') * 10 + (s[1] - '0');
  int mm = (s[2] - '0') * 10 + (s[3] - '0');
  if (hh > 23 || mm > 59) {
    return false;
  }
  outMinute = static_cast<int16_t>(hh * 60 + mm);
  return true;
}

bool parseUint8Max(const char *s, uint8_t maxVal, uint8_t &outVal) {
  if (s[0] == '\0') {
    return false;
  }
  uint16_t val = 0;
  for (size_t i = 0; s[i] != '\0'; i++) {
    if (s[i] < '0' || s[i] > '9') {
      return false;
    }
    val = val * 10 + (s[i] - '0');
    if (val > maxVal) {
      return false;
    }
  }
  outVal = static_cast<uint8_t>(val);
  return true;
}

bool parseHexColor(const char *s, RgbColor &outColor) {
  if (strlen(s) != 6) {
    return false;
  }
  uint8_t bytes[3];
  for (uint8_t i = 0; i < 3; i++) {
    int hi = hexVal(s[i * 2]);
    int lo = hexVal(s[i * 2 + 1]);
    if (hi < 0 || lo < 0) {
      return false;
    }
    bytes[i] = static_cast<uint8_t>((hi << 4) | lo);
  }
  outColor.r = bytes[0];
  outColor.g = bytes[1];
  outColor.b = bytes[2];
  return true;
}

void formatTimeInput(int16_t minute, char *out) {
  if (minute < 0 || minute >= 1440) {
    out[0] = '\0';
    return;
  }
  uint8_t h = minute / 60;
  uint8_t m = minute % 60;
  out[0] = '0' + (h / 10);
  out[1] = '0' + (h % 10);
  out[2] = ':';
  out[3] = '0' + (m / 10);
  out[4] = '0' + (m % 10);
  out[5] = '\0';
}

void formatHexColor(const RgbColor &c, char *out) {
  out[0] = '#';
  putHexByte(c.r, out + 1);
  putHexByte(c.g, out + 3);
  putHexByte(c.b, out + 5);
  out[7] = '\0';
}

void formatClockTime(uint32_t secondOfDay, const char *tz, char *out, size_t outSize) {
  if (outSize < 9) {
    if (outSize > 0) out[0] = '\0';
    return;
  }

  uint8_t h = (secondOfDay / 3600UL) % 24UL;
  uint8_t m = (secondOfDay % 3600UL) / 60UL;
  uint8_t s = secondOfDay % 60UL;

  char tmp[24];
  tmp[0] = '0' + (h / 10);
  tmp[1] = '0' + (h % 10);
  tmp[2] = ':';
  tmp[3] = '0' + (m / 10);
  tmp[4] = '0' + (m % 10);
  tmp[5] = ':';
  tmp[6] = '0' + (s / 10);
  tmp[7] = '0' + (s % 10);

  size_t pos = 8;
  if (tz && tz[0] != '\0' && pos < sizeof(tmp) - 2) {
    tmp[pos++] = ' ';
    for (size_t i = 0; tz[i] != '\0' && pos < sizeof(tmp) - 1; i++) {
      tmp[pos++] = tz[i];
    }
  }
  tmp[pos] = '\0';

  strncpy(out, tmp, outSize - 1);
  out[outSize - 1] = '\0';
}

}  // namespace Codec
