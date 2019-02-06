# Hyperios bootloader for AVR

Hyperios bootloader for AVR MCU. The [hyperios-bl-esp](http://github.com/aliengreen/hyperios-bl-esp) firmware is needed in Atmega328P MCU in order to use this CLI.

### Easy Installation

You will need npm installed on your system.

    $ npm install && npm link

To uninstall simply type:

    $ npm unlink


## Usage

Use `hosbl --help` to see a summary of all available commands and command line options. To see all options for a particular command, append --help to the command name. 

## Configuration file hosbl.yam

First you need to create config file for your firmware in order to upload in device. General config file structure looks like this:

    device:
        serial_port: /dev/cu.usbserial-A6007Whc
        baudrate: 500000
        host: 192.168.1.23
        port: 23
        firmware: bin/MostatPowerAddonM-8Mhz.bin

### Keys

- `serial_port` - Serial port device name.
- `baud_rate` - baud rate of hyperios-bl-esp firmware (should be 500000bps)
- `host` - Remote esp-link ip address or host name
- `port` - Remote esp-link TCP port
- `firmware` - firmware binary file name.


There is two options to upload firmware in device. Using serial port for serial communication or using TCP/IP remote transparent bridge between Wi-Fi and serial port like [esp-link](https://github.com/jeelabs/esp-link).

If you prefer using serial port firmware upload you should specify `serial_port` and `baud_rate` in configuration file `hosbl.yam`. See config example below:

    device:
        serial_port: /dev/cu.usbserial-A6007Whc
        baudrate: 500000
        firmware: bin/MostatPowerAddonM-8Mhz.bin

If you prefer using TCP/IP remote transparent bridge you should specify `host` and `port` in configuration file `hosbl.yam`.  See config example below:

    device:
        host: 192.168.1.23
        port: 23
        firmware: bin/MostatPowerAddonM-8Mhz.bin
    
> Please note, you can't use both serial port and TCP/IP at the same time. 
 
## Commands

Just type `hosbl` in the terminal window. You get rid of command line text like this:


## Serial Bridge

In order to connect through the [esp-link](https://github.com/jeelabs/esp-link) to a microcontroller use port 23. Below is REST HTTP calls for reference. Here we mean that esp-link is on `192.168.1.23` IP address.

#### Get esp-link system information

    http://192.168.1.23/system/info

#### Get esp-link Wi-Fi information

    http://192.168.1.23/wifi/info
    
#### Get esp-link Access Point information

    http://192.168.1.23/wifi/apinfo
    
#### Reset connected uC

    http://192.168.1.23/console/reset

#### Get console baud rate

    http://192.168.1.23/console/baud

#### Set console baud rate e.g. 19200

    http://192.168.1.23/console/baud?rate=19200

#### Get console baud rate format

    http://192.168.1.23/console/fmt
    
#### Set console baud rate format

    http://192.168.1.23/console/fmt?fmt=8N1

#### Read console input buffer

    http://192.168.1.23/console/text?start=0
    
#### Write into console output buffer (Hello\r\n)

    http://192.168.1.23/console/send?text=Hello%0D%0A
    
#### Read esp-link log
    
    http://192.168.1.23/log/text?start=0
  
#### Get esp-link UART debug log mode
    
    http://192.168.1.23/log/dbg
    
#### Reset esp-link module

    http://192.168.1.23/log/reset
      
#### Start Scan APs

To start scan Access Points you should send POST request on url below:

    http://192.168.1.23/wifi/scan
    
To get list already scanned APs you should send GET request on the same url:

    http://192.168.1.23/wifi/scan
