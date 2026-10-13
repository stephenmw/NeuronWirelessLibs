// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <stdint.h>
namespace ploverhid {
uint32_t connectionGeneration();
enum class SendResult { Accepted, Retry, Fault };
SendResult sendReport(const uint8_t *report, uint32_t generation);
}
