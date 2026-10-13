// SPDX-License-Identifier: GPL-3.0-or-later
#include "Kaleidoscope-PloverHID.h"
#include <Kaleidoscope-FocusSerial.h>
#include "PloverHIDTransport.h"
namespace kaleidoscope { namespace plugin {
EventHandlerResult PloverHID::onKeyswitchEvent(Key &key, KeyAddr address, uint8_t flags) {
  const bool plv = key.getRaw() >= PLOVER_HID_FIRST && key.getRaw() <= PLOVER_HID_LAST;
  const bool owned = address.isValid() && state_.owns(address.toInt());
  if (!plv && !owned) return EventHandlerResult::OK;
  if (!address.isValid() || (flags & INJECTED)) return EventHandlerResult::EVENT_CONSUMED;
  state_.connection(ploverhid::connectionGeneration());
  if (keyToggledOff(flags)) state_.release(address.toInt());
  else if (keyToggledOn(flags) && plv) state_.press(address.toInt(), key.getRaw() - PLOVER_HID_FIRST);
  return EventHandlerResult::EVENT_CONSUMED;
}
EventHandlerResult PloverHID::beforeEachCycle() {
  state_.connection(ploverhid::connectionGeneration());
  flush();
  return EventHandlerResult::OK;
}
EventHandlerResult PloverHID::afterEachCycle() {
  // Also reconcile physical releases masked before plugin hooks.
  for (uint8_t i = 0; i < KeyAddr::upper_limit; ++i)
    if (state_.owns(i) && !Runtime.device().isKeyswitchPressed(KeyAddr(i))) state_.release(i);
  state_.connection(ploverhid::connectionGeneration());
  flush();
  return EventHandlerResult::OK;
}
void PloverHID::flush() {
  const uint32_t generation = ploverhid::connectionGeneration();
  state_.connection(generation);
  bool fault = false;
  state_.flush([&](const uint8_t *report) {
    const auto result = ploverhid::sendReport(report, generation);
    fault = result == ploverhid::SendResult::Fault;
    return result == ploverhid::SendResult::Accepted;
  });
  if (fault) state_.deliveryFault();
}
EventHandlerResult PloverHID::onFocusEvent(const char *command) {
  if (::Focus.handleHelp(command, "ploverhid.version\nploverhid.faults")) return EventHandlerResult::OK;
  if (strcmp(command, "ploverhid.version") == 0) ::Focus.send(uint8_t(1));
  else if (strcmp(command, "ploverhid.faults") == 0) ::Focus.send(state_.faults());
  else return EventHandlerResult::OK;
  return EventHandlerResult::EVENT_CONSUMED;
}
}}
kaleidoscope::plugin::PloverHID PloverHID;
