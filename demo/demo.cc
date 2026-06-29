// SPDX-FileCopyrightText: 2026 Ralfs K. Vētra
//
// SPDX-License-Identifier: MIT

// #include <vector>
#include <array>
#include <cstdint>

import serial;

using namespace serial;

int main() {
  Serial port({.port{"/dev/ttyACM0"},
               .rate{bit_rate::b115200},
               .parity{parity_mode::odd}});

  // const std::vector<std::uint8_t> buf(11, 0);
  // const std::uint8_t buf[]{1, 2, 3};
  const std::array<std::uint8_t, 3> buf{1, 2, 3};
  auto bytes_written = port.write(buf);

  // std::vector<std::uint8_t> buf(11, 0);
  // std::uint8_t buf[64]{};
  // std::array<std::uint8_t, 11> buf{};
  // auto bytes_read = port.read(buf);
}
