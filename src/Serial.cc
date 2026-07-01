// SPDX-FileCopyrightText: 2026 Ralfs K. Vētra
//
// SPDX-License-Identifier: MIT

module;

#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <span>
#include <stdexcept>
#include <system_error>
#include <termios.h>
#include <unistd.h>
#include <utility>

module serial;

void configureByteSize(termios &Tty, const serial::ByteSize Size) {
  switch (Size) {
  case serial::ByteSize::Five:
    Tty.c_cflag |= CS5;
    break;
  case serial::ByteSize::Six:
    Tty.c_cflag |= CS6;
    break;
  case serial::ByteSize::Seven:
    Tty.c_cflag |= CS7;
    break;
  case serial::ByteSize::Eight:
    Tty.c_cflag |= CS8;
    break;
  default:
    throw std::range_error("Unrecognized byte size");
  }
}

void configureParityMode(termios &Tty, const serial::ParityMode Parity) {
  switch (Parity) {
  case serial::ParityMode::None:
    break;
  case serial::ParityMode::Odd:
    Tty.c_cflag |= (PARENB | PARODD);
    break;
  case serial::ParityMode::Even:
    Tty.c_cflag |= PARENB;
    Tty.c_cflag &= ~PARODD;
    break;
  case serial::ParityMode::Mark:
#ifdef CMSPAR
    Tty.c_cflag |= (PARENB | CMSPAR | PARODD);
    break;
#else
    throw std::invalid_argument("Mark parity is not supported");
#endif
  case serial::ParityMode::Space:
#ifdef CMSPAR
    Tty.c_cflag |= (PARENB | CMSPAR);
    Tty.c_cflag &= ~PARODD;
    break;
#else
    throw std::invalid_argument("Space parity is not supported");
#endif
  default:
    throw std::range_error("Unrecognized parity");
  }
}

void configureStopBit(termios &Tty, const serial::StopBit Stop) {
  switch (Stop) {
  case serial::StopBit::One:
    Tty.c_cflag &= ~CSTOPB;
    break;
  case serial::StopBit::Two:
    Tty.c_cflag |= CSTOPB;
    break;
  default:
    throw std::range_error("Unrecognized stop bit");
  }
}

void configureFlowControl(termios &Tty, const serial::FlowControl Flow) {
  switch (Flow) {
  case serial::FlowControl::None:
    Tty.c_cflag &= ~CRTSCTS;
    Tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    break;
  case serial::FlowControl::Hardware:
    Tty.c_cflag |= CRTSCTS;
    break;
  case serial::FlowControl::Software:
    Tty.c_iflag |= (IXON | IXOFF | IXANY);
    break;
  default:
    throw std::range_error("Unrecognized flow control");
  }
}

void configureTimeout(termios &Tty, const std::int16_t TimeoutMs) {
  if (TimeoutMs > 25'500) {
    throw std::out_of_range("Timeout cannot exceed 25500 milliseconds");
  }

  if (TimeoutMs > 0) {
    Tty.c_cc[VMIN] = 0;
    Tty.c_cc[VTIME] = static_cast<cc_t>((TimeoutMs + 99) / 100);
  } else if (TimeoutMs < 0) [[likely]] {
    Tty.c_cc[VMIN] = 1;
    Tty.c_cc[VTIME] = 0;
  } else {
    Tty.c_cc[VMIN] = 0;
    Tty.c_cc[VTIME] = 0;
  }
}

speed_t mapBitRate(serial::BitRate Rate) {
  switch (Rate) {
  case serial::BitRate::BR0:
    return B0;
  case serial::BitRate::BR50:
    return B50;
  case serial::BitRate::BR75:
    return B75;
  case serial::BitRate::BR110:
    return B110;
  case serial::BitRate::BR134:
    return B134;
  case serial::BitRate::BR150:
    return B150;
  case serial::BitRate::BR200:
    return B200;
  case serial::BitRate::BR300:
    return B300;
  case serial::BitRate::BR600:
    return B600;
  case serial::BitRate::BR1200:
    return B1200;
  case serial::BitRate::BR1800:
    return B1800;
  case serial::BitRate::BR2400:
    return B2400;
  case serial::BitRate::BR4800:
    return B4800;
  case serial::BitRate::BR9600:
    return B9600;
  case serial::BitRate::BR19200:
    return B19200;
  case serial::BitRate::BR38400:
    return B38400;
  case serial::BitRate::BR57600:
    return B57600;
  case serial::BitRate::BR115200:
    return B115200;
  case serial::BitRate::BR230400:
    return B230400;
  case serial::BitRate::BR460800:
    return B460800;
  case serial::BitRate::BR500000:
    return B500000;
  case serial::BitRate::BR576000:
    return B576000;
  case serial::BitRate::BR921600:
    return B921600;
  case serial::BitRate::BR1000000:
    return B1000000;
  case serial::BitRate::BR1152000:
    return B1152000;
  case serial::BitRate::BR1500000:
    return B1500000;
  case serial::BitRate::BR2000000:
    return B2000000;
  default:
    throw std::invalid_argument("Unrecognized bit rate");
  }
}

std::int16_t serial::Serial::getTimeout() const { return Options.Timeout; }

void serial::Serial::setTimeout(std::int16_t Timeout) {
  Options.Timeout = Timeout;
  configurePort();
}

void serial::Serial::configurePort() {
  struct termios Tty;

  if (::tcgetattr(Fd, &Tty) < 0) [[unlikely]] {
    throw std::system_error(errno, std::generic_category(),
                            "Failed to get port stats");
  }

  ::cfmakeraw(&Tty);

  const speed_t BitRate = mapBitRate(Options.Rate);
  if (::cfsetspeed(&Tty, BitRate) < 0) [[unlikely]] {
    throw std::system_error(errno, std::generic_category(),
                            "Failed to set speed");
  }

  configureByteSize(Tty, Options.Size);
  configureParityMode(Tty, Options.Parity);
  configureStopBit(Tty, Options.Stop);
  configureFlowControl(Tty, Options.Flow);
  configureTimeout(Tty, Options.Timeout);

  if (::tcsetattr(Fd, TCSANOW, &Tty) < 0) [[unlikely]] {
    throw std::system_error(errno, std::generic_category(),
                            "Failed to set attributes");
  }
}

std::uint32_t serial::Serial::write(std::span<const std::uint8_t> Data) const {
  if (Data.empty())
    return 0;

  ssize_t BytesWritten = ::write(Fd, Data.data(), Data.size());

  if (BytesWritten < 0) [[unlikely]] {
    throw std::system_error(errno, std::generic_category(),
                            "Failed to write to serial port");
  }

  return static_cast<std::uint32_t>(BytesWritten);
}

std::uint32_t serial::Serial::read(std::span<std::uint8_t> Data) {
  if (Data.empty())
    return 0;

  ssize_t BytesRead = ::read(Fd, Data.data(), Data.size());

  if (BytesRead < 0) [[unlikely]] {
    throw std::system_error(errno, std::generic_category(),
                            "Failed to read from serial port");
  }

  return static_cast<std::uint32_t>(BytesRead);
}

void serial::Serial::openPort() {
  Fd = ::open(Options.Port.c_str(), O_RDWR | O_NOCTTY);

  if (Fd < 0) [[unlikely]] {
    throw std::system_error(errno, std::generic_category(),
                            "Failed to open " + Options.Port);
  }

  try {
    configurePort();
  } catch (...) {
    closePort();
    throw;
  }
}

void serial::Serial::closePort() {
  if (Fd >= 0) [[likely]] {
    ::close(Fd);
    Fd = -1;
  }
}

serial::Serial::~Serial() { closePort(); }

serial::Serial::Serial(PortOptions OptionsInput)
    : Options{std::move(OptionsInput)}, Fd{-1} {
  if (Options.Port.empty()) [[unlikely]] {
    throw std::invalid_argument("Port path cannot be empty");
  }

  openPort();
}

serial::Serial::Serial(Serial &&Other) noexcept
    : Options{std::move(Other.Options)}, Fd{std::exchange(Other.Fd, -1)} {}

serial::Serial &serial::Serial::operator=(Serial &&Other) noexcept {
  if (this != &Other) {
    closePort();
    Options = std::move(Other.Options);
    Fd = std::exchange(Other.Fd, -1);
  }

  return *this;
}
