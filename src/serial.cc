// SPDX-FileCopyrightText: 2026 Ralfs K. Vētra
//
// SPDX-License-Identifier: MIT

module;

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <cerrno>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <system_error>

module serial;

serial::serial::serial(port_opt opt) :
    opt{std::move(opt)},
    fd{-1}
{
    if (opt.device.empty()) [[unlikely]]
	throw std::invalid_argument("Device path cannot be empty.");
}

serial::serial::~serial() {
    close();
}

void serial::serial::close()
{
    if (fd >= 0) {
	::close(fd);
	fd = -1;
    }
}

void serial::serial::open()
{
    fd = ::open(opt.device.c_str(), O_RDWR | O_NOCTTY);

    if (fd < 0) [[unlikely]] {
	throw std::system_error(errno, std::generic_category(),
				"Failed to open " + opt.device);
    }

    conf_port();
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

void set_parity(termios& tty, const serial::parity parity)
{
    switch (parity) {
    case serial::parity::none:
	break;
    case serial::parity::odd:
	tty.c_cflag |= (PARENB | PARODD);
	break;
    case serial::parity::even:
	tty.c_cflag |= PARENB;
	tty.c_cflag &= ~PARODD;
	break;
    case serial::parity::mark:
#ifdef CMSPAR
	tty.c_cflag |= (PARENB | CMSPAR | PARODD);
	break;
#else
	throw std::invalid_argument("Mark parity is not supported");
#endif
    case serial::parity::space:
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
    } else if (timeout_ms < 0) {
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;
    } else {
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 0;
    }
}

void serial::serial::conf_port()
{
    struct termios tty;

    if (::tcgetattr(fd, &tty) < 0) {
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
    set_parity(tty, opt.parity);
    set_stop_bit(tty, opt.stop);
    set_flow_control(tty, opt.flow);
    set_timeout(tty, opt.timeout);

    if (::tcsetattr(fd, TCSANOW, &tty) < 0) {
	throw std::system_error(errno, std::generic_category(),
				"Failed to set attributes");
    }
}
