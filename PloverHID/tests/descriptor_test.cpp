// SPDX-License-Identifier: GPL-3.0-or-later
#include "PloverHIDProtocol.h"
#include <cassert>
#include <cstdio>

int main() {
  const uint8_t descriptor[] = { PLOVER_HID_DESCRIPTOR };
  unsigned page = 0, usage = 0, report_id = 0, size = 0, count = 0;
  unsigned depth = 0, input_bits = 0;
  bool discoverable = false;
  for (unsigned i = 0; i < sizeof(descriptor);) {
    const unsigned prefix = descriptor[i++];
    const unsigned length = (prefix & 3) == 3 ? 4 : prefix & 3;
    assert(i + length <= sizeof(descriptor));
    unsigned value = 0;
    for (unsigned j = 0; j < length; ++j) value |= unsigned(descriptor[i++]) << (8 * j);
    switch (prefix & 0xfc) {
      case 0x04: page = value; break;
      case 0x08: usage = value; break;
      case 0x84: report_id = value; break;
      case 0x74: size = value; break;
      case 0x94: count = value; break;
      case 0xa0:
        if (depth == 0 && page == 0xff50 && usage == 0x4c56) {
          // macOS does not publish this usage for a top-level Logical collection.
          discoverable = value == 1;
        }
        ++depth;
        break;
      case 0x80:
        assert(report_id == 0x50);
        input_bits += size * count;
        break;
      case 0xc0: assert(depth > 0); --depth; break;
    }
  }
  assert(depth == 0);
  assert(discoverable && "PloverHID must expose an Application collection for macOS discovery");
  assert(input_bits == 64);
  std::puts("PloverHID discovery descriptor and 64-bit report passed.");
}
