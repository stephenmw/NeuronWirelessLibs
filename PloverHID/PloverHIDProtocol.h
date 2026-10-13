// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <stdint.h>
#define PLOVER_HID_REPORT_ID 0x50
#define PLOVER_HID_REPORT_SIZE 8
// Explicit allocation: does not move Kaleidoscope's existing persisted ranges.
#define PLOVER_HID_FIRST 0xE000
#define PLOVER_HID_LAST (PLOVER_HID_FIRST + 63)
// macOS publishes this usage to HIDAPI only as an Application collection.
// Keep the Plover usage and report format; a Logical collection is not discoverable.
#define PLOVER_HID_DESCRIPTOR \
  0x06, 0x50, 0xff, 0x0a, 0x56, 0x4c, 0xa1, 0x01, \
  0x85, PLOVER_HID_REPORT_ID, 0x15, 0x00, 0x25, 0x01, \
  0x75, 0x01, 0x95, 0x40, 0x05, 0x0a, 0x19, 0x00, \
  0x29, 0x3f, 0x81, 0x02, 0xc0
