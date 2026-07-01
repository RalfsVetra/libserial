# libserial

A Linux serial port library inspired by the simplicity of [pySerial](https://pyserial.readthedocs.io/en/latest/), built
entirely as a module.

## Example

```c++
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
  std::array<std::uint8_t, 11> Buf{};
  auto BytesRead = Port.read(Buf);
}
```

See the `demo/` directory for an example of how to use this library with CMake as a standalone project.

## References

[Understanding UNIX termios VMIN and VTIME](http://www.unixwiz.net/techtips/termios-vmin-vtime.html) – by [Steve Friedl](http://www.unixwiz.net/).

[CppLinuxSerial](https://github.com/gbmhunter/CppLinuxSerial) - Inspiration for configuring serial timeouts.

[serial](https://github.com/wjwwood/serial) - An honorable mention for serial port implementation.

## License
This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for details.