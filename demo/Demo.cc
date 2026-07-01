// SPDX-FileCopyrightText: 2026 Ralfs K. Vētra
//
// SPDX-License-Identifier: MIT

// #include <vector>
#include <array>
#include <cstdint>

import serial;

using namespace serial;

int main() {
  Serial Port({.Port{"/dev/ttyACM0"},
               .Rate{BitRate::BR115200},
               .Parity{ParityMode::Odd}});

  // const std::vector<std::uint8_t> Buf(11, 0);
  // const std::uint8_t Buf[11]{};
  const std::array<std::uint8_t, 11> Buf{};
  auto BytesWritten = Port.write(Buf);

  // std::vector<std::uint8_t> Buf(11, 0);
  // std::uint8_t Buf[11]{};
  // std::array<std::uint8_t, 11> Buf{};
  // auto BytesRead = Port.read(Buf);
}
