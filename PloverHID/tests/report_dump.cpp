// SPDX-License-Identifier: GPL-3.0-or-later
#include "PloverHIDState.h"
#include "PloverHIDProtocol.h"
#include <cstdio>
int main() {
  ploverhid::State<80> state;
  state.connection(1);
  state.flush([](const uint8_t *) { return true; });
  for (uint8_t key = 0; key < 64; ++key) {
    state.press(0, key);
    state.flush([](const uint8_t *report) {
      std::printf("%02x", PLOVER_HID_REPORT_ID);
      for (int i = 0; i < 8; ++i) std::printf("%02x", report[i]);
      std::puts("");
      return true;
    });
    state.release(0);
    state.flush([](const uint8_t *) { return true; });
  }
}
