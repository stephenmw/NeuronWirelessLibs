#include "PloverHIDState.h"
#include <cassert>
#include <array>
#include <vector>
using Report = std::array<uint8_t, 8>;
int main() {
  ploverhid::State<60> state;
  std::vector<Report> reports;
  auto send = [&](const uint8_t *data) { Report r{}; std::copy(data, data + 8, r.begin()); reports.push_back(r); return true; };
  state.connection(1);
  state.flush(send);
  state.press(60, 0); // address outside Sonsei matrix is ignored
  state.press(59, 64); // index outside protocol is ignored
  assert(!state.owns(59) && !state.owns(60));
  state.press(0, 0);
  state.press(1, 0);
  state.release(0);
  state.flush(send);
  assert(reports.size() == 2 && reports.back()[0] == 0x80);
  state.release(1);
  state.flush(send);
  assert(reports.size() == 3 && reports.back() == Report{});
  state.press(59, 63);
  state.flush([](const uint8_t *) { return false; });
  state.release(59);
  state.flush(send);
  assert(reports.back()[7] == 1);
  state.flush(send);
  assert(reports.back() == Report{});
  state.press(3, 7);
  state.connection(2); // queued input must never cross connections
  state.flush(send);
  assert(reports.back() == Report{});
  state.press(4, 8); // new presses during the barrier are suppressed too
  state.release(3);
  state.flush(send);
  state.release(4);
  state.flush(send);
  const auto count = reports.size();
  state.press(4, 8);
  state.press(4, 8); // repeated held event is not another physical press
  state.flush(send);
  assert(reports.size() == count + 1 && reports.back()[1] == 0x80);
  state.release(4);
  state.release(4);
  state.flush(send);
  assert(reports.back() == Report{});
  ploverhid::State<60, 2> bounded;
  bounded.connection(1);
  bounded.flush(send);
  bounded.press(0, 0);
  bounded.release(0);
  bounded.press(0, 0); // overflow must recover, not emit a truncated stroke
  assert(bounded.faults() == 1);
  bounded.flush(send);
  assert(reports.back() == Report{});
  bounded.release(0);
  bounded.flush(send);
  bounded.press(0, 22);
  bounded.flush(send);
  assert(reports.back()[2] == 2);
}
