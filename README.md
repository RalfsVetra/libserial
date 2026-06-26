# libserial

A Linux serial port library inspired by the simplicity of [pySerial](https://pyserial.readthedocs.io/en/latest/), built
entirely as a module.

## Example

```c++
import serial;

using namespace serial;

int main()
{
    serial port({
        .device = "/dev/ttyS0",
        .rate   = bit_rate::b115200,
        .parity = parity::odd
    });
}
```

## References

[Understanding UNIX termios VMIN and VTIME](http://www.unixwiz.net/techtips/termios-vmin-vtime.html) – by [Steve Friedl](http://www.unixwiz.net/).

[CppLinuxSerial](https://github.com/gbmhunter/CppLinuxSerial) - Inspiration for configuring serial timeouts.

[serial](https://github.com/wjwwood/serial) - An honorable mention for serial port implementation.

## License
This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for details.