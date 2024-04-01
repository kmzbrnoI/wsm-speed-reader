# WSM Speed Reader

![Speed Reader Screenshot](doc/speed_reader_screenshot.png)

WSM Speed Reader is a simple desktop application. It connects to the serial
port of the *Wireless SpeedoMeter* and displays its speed.

This application is developed in [QT](https://www.qt.io/).

## Resources

 * [WSM PCB](https://github.com/kmzbrnoI/wsm-pcb)
 * [WSM FW](https://github.com/kmzbrnoI/wsm-fw)

## Building & toolkit

This SW was developed in `vim` using `make`. Downloads are available in
*Releases* section. It is suggested to use `clang` as a compiler, because
then you may use `clang-tools` during development process (see below).

### Prerequisities

 * Qt 5/6
 * Qt's `serialport`
 * Optional: clang build tools (see below)
 * Optional for clang: [Bear](https://github.com/rizsotto/Bear)

#### Example: toolchain setup on Debian 12 Bookworm

```bash
$ apt install qt6-base-dev qt6-charts-dev qt6-serialport-dev
$ apt install clang clang-tools clang-tidy clang-format
$ apt install bear
```

### Build

Clone this repository (including submodules!):

```
git clone --recurse-submodules https://github.com/kmzbrnoI/wsm-speed-reader
```

And then build:

```
$ mkdir build
$ cd build
$ qmake6 -spec linux-clang ..
$ bear make
```

## Connecting to WSM

 * Windows: pair it with HC-05 module, serial port should be added
   automatically
 * Linux: pair it with HC-05 module and map it to new serial device:

    ```
    rfcomm connect /dev/rfcomm0 hc-05-hw-address 2
    ```

## Style checking

```
$ clang-tidy -p build src/*.cpp
$ clang-format *.cpp *.h
```

## Authors

 * Jan Horacek ([jan.horacek@kmz-brno.cz](mailto:jan.horacek@kmz-brno.cz))

## License

This application is released under the [Apache License v2.0
](https://www.apache.org/licenses/LICENSE-2.0).
