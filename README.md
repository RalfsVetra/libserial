# libserial

A Linux serial port library inspired by the simplicity of [pySerial](https://pyserial.readthedocs.io/en/latest/), built
entirely as a module.

## Example

```c++
import serial;

using namespace serial;

int main()
{
    Serial port({
        .port{"/dev/ttyACM0"},
        .rate{bit_rate::b115200},
        .parity{parity_mode::odd}
    });

    // const std::vector<std::uint8_t> buf(11, 0);
    // const std::uint8_t buf[] = {1, 2, 3};
    const std::array<std::uint8_t, 3> buf{1, 2, 3};
    auto bytes_written = port.write(buf);

    // std::vector<std::uint8_t> buf(11, 0);
    // std::uint8_t buf[64] = {};
    std::array<std::uint8_t, 11> buf{};
    auto bytes_read = port.read(buf);
}
```

See the `demo/` directory for an example of how to use this library with CMake as a standalone project.

## References

[Understanding UNIX termios VMIN and VTIME](http://www.unixwiz.net/techtips/termios-vmin-vtime.html) – by [Steve Friedl](http://www.unixwiz.net/).

[CppLinuxSerial](https://github.com/gbmhunter/CppLinuxSerial) - Inspiration for configuring serial timeouts.

[serial](https://github.com/wjwwood/serial) - An honorable mention for serial port implementation.

## License
This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for details.