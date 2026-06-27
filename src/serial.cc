// SPDX-FileCopyrightText: 2026 Ralfs K. Vētra
//
// SPDX-License-Identifier: MIT

module;

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <cerrno>
#include <cstdint>
#include <span>
#include <utility>
#include <stdexcept>
#include <system_error>

module serial;

serial::Serial::Serial(port_opt popt) :
    opt{std::move(popt)},
    fd{-1}
{
    if (opt.port.empty()) [[unlikely]]
        throw std::invalid_argument("Port path cannot be empty");
}

serial::Serial::~Serial() {
    close();
}

void serial::Serial::open()
{
    fd = ::open(opt.port.c_str(), O_RDWR | O_NOCTTY);

    if (fd < 0) [[unlikely]] {
        throw std::system_error(errno, std::generic_category(),
                                "Failed to open " + opt.port);
    }

    conf_port();
}

void serial::Serial::close()
{
    if (fd >= 0) [[likely]] {
        ::close(fd);
        fd = -1;
    }
}

void set_byte_size(termios& tty, const serial::byte_size size)
{
    switch (size) {
    case serial::byte_size::five:
        tty.c_cflag |= CS5;
        break;
    case serial::byte_size::six:
        tty.c_cflag |= CS6;
        break;
    case serial::byte_size::seven:
        tty.c_cflag |= CS7;
        break;
    case serial::byte_size::eight:
        tty.c_cflag |= CS8;
        break;
    default:
        throw std::range_error("Unrecognized byte size");
    }
}

void set_parity_mode(termios& tty, const serial::parity_mode parity)
{
    switch (parity) {
    case serial::parity_mode::none:
        break;
    case serial::parity_mode::odd:
        tty.c_cflag |= (PARENB | PARODD);
        break;
    case serial::parity_mode::even:
        tty.c_cflag |= PARENB;
        tty.c_cflag &= ~PARODD;
        break;
    case serial::parity_mode::mark:
#ifdef CMSPAR
        tty.c_cflag |= (PARENB | CMSPAR | PARODD);
        break;
#else
        throw std::invalid_argument("Mark parity is not supported");
#endif
    case serial::parity_mode::space:
#ifdef CMSPAR
        tty.c_cflag |= (PARENB | CMSPAR);
        tty.c_cflag &= ~PARODD;
        break;
#else
        throw std::invalid_argument("Space parity is not supported");
#endif
    default:
        throw std::range_error("Unrecognized parity");
    }
}

void set_stop_bit(termios& tty, const serial::stop_bit stop)
{
    switch (stop) {
    case serial::stop_bit::one:
        tty.c_cflag &= ~CSTOPB;
        break;
    case serial::stop_bit::two:
        tty.c_cflag |= CSTOPB;
        break;
    default:
        throw std::range_error("Unrecognized stop bit");
    }
}

void set_flow_control(termios& tty, const serial::flow_control flow)
{
    switch (flow) {
    case serial::flow_control::none:
        tty.c_cflag &= ~CRTSCTS;
        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        break;
    case serial::flow_control::hardware:
        tty.c_cflag |= CRTSCTS;
        break;
    case serial::flow_control::software:
        tty.c_iflag |= (IXON | IXOFF | IXANY);
        break;
    default:
        throw std::range_error("Unrecognized flow control");
    }
}

void set_timeout(termios& tty, const std::int16_t timeout_ms)
{
    if (timeout_ms > 25'500)
        throw std::out_of_range("Timeout cannot exceed 25500 milliseconds");

    if (timeout_ms > 0) {
        tty.c_cc[VMIN] = 0;
        tty.c_cc[VTIME] = static_cast<cc_t>((timeout_ms + 99) / 100);
    } else if (timeout_ms < 0) [[likely]] {
        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 0;
    } else {
        tty.c_cc[VMIN] = 0;
        tty.c_cc[VTIME] = 0;
    }
}

void serial::Serial::conf_port()
{
    struct termios tty;

    if (::tcgetattr(fd, &tty) < 0) [[unlikely]] {
        throw std::system_error(errno, std::generic_category(),
                                "Failed to get port stats");
    }

    ::cfmakeraw(&tty);

    const speed_t mapped_bit_rate = static_cast<speed_t>(opt.rate);
    if (::cfsetspeed(&tty, mapped_bit_rate) < 0) [[unlikely]] {
        throw std::system_error(errno, std::generic_category(),
                                "Failed to set speed");
    }

    set_byte_size(tty, opt.size);
    set_parity_mode(tty, opt.parity);
    set_stop_bit(tty, opt.stop);
    set_flow_control(tty, opt.flow);
    set_timeout(tty, opt.timeout);

    if (::tcsetattr(fd, TCSANOW, &tty) < 0) [[unlikely]] {
        throw std::system_error(errno, std::generic_category(),
                                "Failed to set attributes");
    }
}

std::uint32_t serial::Serial::write(std::span<const std::uint8_t> data)
{
    if (data.empty())
        return 0;

    ssize_t bytes_written = ::write(fd, data.data(), data.size());

    if (bytes_written < 0) [[unlikely]]
        throw std::system_error(errno, std::generic_category(),
                                "Failed to write to serial port");

    return static_cast<std::uint32_t>(bytes_written);
}

std::uint32_t serial::Serial::read(std::span<std::uint8_t> data)
{
    if (data.empty())
        return 0;

    ssize_t bytes_read = ::read(fd, data.data(), data.size());

    if (bytes_read < 0) [[unlikely]]
        throw std::system_error(errno, std::generic_category(),
                                "Failed to read from serial port");

    return static_cast<std::uint32_t>(bytes_read);
}
