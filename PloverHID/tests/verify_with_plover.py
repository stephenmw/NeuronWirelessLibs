"""Decode real firmware-state reports using an installed, unmodified Plover.

Run with Plover's Python and pass the compiled report_dump executable.
This verifies the wire mapping, not USB/Bluetooth hardware interoperability.
"""
import json
from pathlib import Path
import subprocess
import sys
from plover.machine.plover_hid import PloverHid, STENO_KEY_CHART

contract = json.loads((Path(__file__).parent.parent / 'contract.json').read_text())
assert [entry['index'] for entry in contract] == list(range(64))
assert [entry['code'] for entry in contract] == list(range(0xE000, 0xE040))
assert [entry['name'] for entry in contract] == list(STENO_KEY_CHART)

class IdentityKeymap:
    def keys_to_actions(self, keys):
        return keys

host = object.__new__(PloverHid)
host.keymap = IdentityKeymap()
received = []
host._notify = received.append
reports = subprocess.check_output([sys.argv[1]], text=True).splitlines()
assert len(reports) == 64
for entry, report in zip(contract, reports):
    received.clear()
    host._send(host._parse(bytes.fromhex(report)))
    assert received == [[entry['name']]], (entry, report, received)
print('All 64 firmware reports decoded correctly by installed Plover.')
