// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <Kaleidoscope.h>
#include <Kaleidoscope-Ranges.h>
#include "PloverHIDProtocol.h"
#include "PloverHIDState.h"
static_assert(kaleidoscope::ranges::KALEIDOSCOPE_SAFE_START < PLOVER_HID_FIRST,
              "PloverHID codes must not overlap persisted Kaleidoscope ranges");
#define Key_PloverHID(index) Key(uint16_t(PLOVER_HID_FIRST + (index)))
namespace kaleidoscope { namespace plugin {
class PloverHID : public Plugin {
 public:
  EventHandlerResult onKeyswitchEvent(Key &key, KeyAddr address, uint8_t state);
  EventHandlerResult beforeEachCycle();
  EventHandlerResult afterEachCycle();
  EventHandlerResult onFocusEvent(const char *command);
 private:
  void flush();
  ploverhid::State<KeyAddr::upper_limit> state_;
};
}}
extern kaleidoscope::plugin::PloverHID PloverHID;
