#!/usr/bin/env node

// var UTIL       = require("./saswell");
var YAML = require('yamljs');
var yargs = require('yargs')
var readline = require('readline');
var serial = require('./serial.js');
var bl = require('./bl.js');

var argv = yargs.usage('Hyperios serial bootloader for AVR')
  .describe('c', 'Configuration file (default hosbl.yam)')
  // .alias('i', 'increment')
  // .describe('i', 'Increment minor version number by one in hos_uds_config.h file')
  .version("1.0.0")
  .epilogue('for more information, find our manual at http://www.aliengreen.ge\nplease note, only valid baud for 8Mhz crystal are 2400, 4800, 9600, 14400, 19200, 28800*, 38400, 76800*, 250000, 500000\nthe bauds marked \'*\' in this are the ones that must use U2X mod')
  // .demandCommand(1)
  .help("h")
  .alias("h", "help")
  .alias("v", "version")
  .argv

let verbose = false;

try {
  if (argv.c) {
    cfgFile = YAML.load(argv.c);
  } else {
    cfgFile = YAML.load("hosbl.yam");
  }
} catch (e) {
  console.log(e.message);
  process.exit(1);
}

var rl = readline.createInterface(process.stdin, process.stdout);
yargs.showHelp();
console.log(`dev: ${cfgFile.device.serial_port}`);
console.log(`bps: ${cfgFile.device.baudrate}`);
console.log(`firmware: ${cfgFile.device.firmware}`);
serial.create(cfgFile.device.serial_port, cfgFile.device.baudrate, initSerial, deInitSerial, processPacket);


try {
  serial.start();
  bl.firmware(cfgFile.device.firmware);
} catch (e) {
  console.log(e.message);
  process.exit(1);
}

function initSerial() {
  console.log(`\r\nWaiting to bootloader...`);
}

function deInitSerial() {
  console.log(`\r\nStop`);
}

function processPacket() {
  serial.unlock();
  var ret = bl.process(serial.incomingData(), serial);
  if(ret == 2) {
    console.log('Firmware updated successfully');
    serial.stop();
    process.exit(0);
  } else if(ret == 3) {
    console.log('Error while updating firmware');
    serial.stop();
    process.exit(1);
  }
}
