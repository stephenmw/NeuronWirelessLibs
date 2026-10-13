# PloverHID on Sonsei

This Kaleidoscope plugin maps 64 direct key assignments (`0xE000`–`0xE03F`) to
Plover's eight-byte HID bitmap. It owns physical presses through release, counts
duplicate assignments, and sends ordered snapshots. USB and BLE expose report
ID `0x50`; a connection change clears pending reports and requires held PLV
keys to release before another stroke. `ploverhid.version` returns `1` through
Focus so compatible Bazecor builds can enable their PloverHID picker.

The Sonsei application integrates the plugin through `kaleidoscope_adapter`,
provides `PloverHIDTransport.h`'s functions, and assembles USB/BLE descriptors
with `PLOVER_HID_DESCRIPTOR`. The Application collection permits discovery by
macOS HIDAPI. Tests cover the Sonsei 60-address matrix and the 64-key protocol.
The native state test uses a fake transport; `verify_with_plover.py` uses an
installed unmodified Plover decoder. These checks do not prove hardware
interoperability. See the application README for build instructions and the
remaining USB/Bluetooth hardware checks.
