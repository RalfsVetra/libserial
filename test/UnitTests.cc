// SPDX-FileCopyrightText: 2026 Ralfs K. Vētra
//
// SPDX-License-Identifier: MIT

#include <array>
#include <cstdint>
#include <gtest/gtest.h>

import serial;

namespace {
class SerialTest : public testing::Test {
protected:
  serial::Serial Port{
      {.Port{"/dev/ttyACM0"}, .Parity{serial::ParityMode::Odd}}};
};

TEST_F(SerialTest, GetTimeout) { EXPECT_EQ(-1, Port.getTimeout()); }

TEST_F(SerialTest, SetTimeout) {
  Port.setTimeout(500);
  EXPECT_EQ(500, Port.getTimeout());
}

// TEST_F(SerialTest, WritePort) {
//   std::array<std::uint8_t, 11> Buf{};
//   EXPECT_EQ(11, Port.write(Buf));
// }

// TEST_F(SerialTest, ReadPort) {
//   std::array<std::uint8_t, 11> Buf{};
//   EXPECT_EQ(11, Port.read(Buf));
// }
} // namespace
