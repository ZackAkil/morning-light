#pragma once

#include <stddef.h>
#include "types.h"

namespace Codec {

bool parseTimeToken(const char *s, int16_t &outMinute);
bool parseUint8Max(const char *s, uint8_t maxVal, uint8_t &outVal);
bool parseHexColor(const char *s, RgbColor &outColor);

void formatTimeInput(int16_t minute, char *out);
void formatHexColor(const RgbColor &c, char *out);
void formatClockTime(uint32_t secondOfDay, const char *tz, char *out, size_t outSize);

}  // namespace Codec
