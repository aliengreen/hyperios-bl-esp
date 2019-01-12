# Hyperios serial bootloader for AVR

Hyperios serial bootloader for AVR MCU. The [hyperios-bl-esp](http://github.com/aliengreen/hyperios-bl-esp) firmware is needed in Atmega328P MCU in order to use this CLI.

### Easy Installation

You will need npm installed on your system.

    $ npm install && npm link

To uninstall simply type:

    $ npm unlink


## Usage

Use `hosbl --help` to see a summary of all available commands and command line options. To see all options for a particular command, append --help to the command name. 

## Configuration file hosbl.yam

First you need to create config file for your emulator. General config file structure looks like this:

    device:
        serial_port: /dev/cu.usbserial-A6007Whc
        baudrate: 500000
        firmware: MostatPowerAddon-8Mhz.bin

### Keys

- `serial_port` - USB-868Mhz device serial port name.
- `baudrate` - baud rate of hyperios-bl-esp firmware (should be 500000bps)
- `firmware` - firmware binary file name.

## Commands

Just type `hosbl` in the terminal window. You get rid of command line text like this:

