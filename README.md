![](./resources/fiubaLogo.png)

# Proyect "Rastreador GNSS para Tareas Investigativas" (GNSS Tracker for investigation Tasks)

# Brief

The Firmware contain in this reposity form part of the proyect GNSS Tracker for investigation Tasks which
is the final work of the degree Electronic Engenieering from the
[Faculty of Engenieering of the University of Buenos Aires](https://www.fi.uba.ar/).
This code consist in the code upload into the tracker it self, while the others main parts of the proyect are
a [LoRa Gateway](https://github.com/FacundoBaez1994/GatewayLoRa) and the [Remote Control Server](https://github.com/joaquinelordi/Torcaza)
named Torcaza after a Argentinean dove.

The GNNS tracker system for investigative tasks, consists of an autonomous device built with the objective of getting the localization 
of himself in real time, and sending it to a remote base, in order to keep the track of a vehicle terrestrial or naval.
The device was initialy conceived for his use in the Argentinian Coast Guard, but is extrapolable to almost any LEA, 
(Law Enforcement Agent) and it was devised after a real problem. 

# Firmware

The device was developed in the C++ programming languages and using the NUCLEO develop board from ARM, and it makes use of various IOT devices.
  This work is carried out within the framework of the Embedded Systems subject of the Faculty of Engineering of the University of Buenos Aires and in it the topics learned in the subject and in general in the Electronic Engineering career are applied; Such as OOP, general electronics, data communication, among others. 
  In these memories the work is presented, its planning and motivation, and the technical information about the hardware and firmware, as well as the explanation of its design and organization

# Hardware

# Members

## authors

1. Baez Facundo
2. Elordi Joaquin

## tutor

1. Ing. Graña Jorge

# Credits (Libraries used)



You can build the project with all supported [Mbed OS build tools](https://os.mbed.com/docs/mbed-os/latest/tools/index.html). However, this example project specifically refers to the command-line interface tool [Arm Mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).
(Note: To see a rendered example you can import into the Arm Online Compiler, please see our [import quick start](https://os.mbed.com/docs/mbed-os/latest/quick-start/online-with-the-online-compiler.html#importing-the-code).)


### Mbed CLI 2
Starting with version 6.5, Mbed OS uses Mbed CLI 2. It uses Ninja as a build system, and CMake to generate the build environment and manage the build process in a compiler-independent manner. If you are working with Mbed OS version prior to 6.5 then check the section [Mbed CLI 1](#mbed-cli-1).
1. [Install Mbed CLI 2](https://os.mbed.com/docs/mbed-os/latest/build-tools/install-or-upgrade.html).
1. From the command-line, import the example: `mbed-tools import mbed-os-example-blinky`
1. Change the current directory to where the project was imported.

### Mbed CLI 1
1. [Install Mbed CLI 1](https://os.mbed.com/docs/mbed-os/latest/quick-start/offline-with-mbed-cli.html).
1. From the command-line, import the example: `mbed import mbed-os-example-blinky`
1. Change the current directory to where the project was imported.

## Application functionality

The `main()` function is the single thread in the application. It toggles the state of a digital output connected to an LED on the board.

**Note**: This example requires a target with RTOS support, i.e. one with `rtos` declared in `supported_application_profiles` in `targets/targets.json` in [mbed-os](https://github.com/ARMmbed/mbed-os). For non-RTOS targets (usually with small memory sizes), please use [mbed-os-example-blinky-baremetal](https://github.com/ARMmbed/mbed-os-example-blinky-baremetal) instead.

## Building and running

1. Connect a USB cable between the USB port on the board and the host computer.
1. Run the following command to build the example project and program the microcontroller flash memory:

    * Mbed CLI 2

    ```bash
    $ mbed-tools compile -m <TARGET> -t <TOOLCHAIN> --flash
    ```

    * Mbed CLI 1

    ```bash
    $ mbed compile -m <TARGET> -t <TOOLCHAIN> --flash
    ```

Your PC may take a few minutes to compile your code.

The binary is located at:
* **Mbed CLI 2** - `./cmake_build/mbed-os-example-blinky.bin`</br>
* **Mbed CLI 1** - `./BUILD/<TARGET>/<TOOLCHAIN>/mbed-os-example-blinky.bin`

Alternatively, you can manually copy the binary to the board, which you mount on the host computer over USB.

## Expected output
The LED on your target turns on and off every 500 milliseconds.


## Troubleshooting
If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it.

## Related Links

* [Mbed OS Stats API](https://os.mbed.com/docs/latest/apis/mbed-statistics.html).
* [Mbed OS Configuration](https://os.mbed.com/docs/latest/reference/configuration.html).
* [Mbed OS Serial Communication](https://os.mbed.com/docs/latest/tutorials/serial-communication.html).
* [Mbed OS bare metal](https://os.mbed.com/docs/mbed-os/latest/reference/mbed-os-bare-metal.html).
* [Mbed boards](https://os.mbed.com/platforms/).

### License and contributions

The software is provided under Apache-2.0 license. Contributions to this project are accepted under the same license. Please see [CONTRIBUTING.md](./CONTRIBUTING.md) for more info.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.
