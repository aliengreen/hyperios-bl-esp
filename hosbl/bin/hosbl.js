#!/usr/bin/env node

// var UTIL       = require("./saswell");
var YAML = require('yamljs');
var yargs = require('yargs')
var readline = require('readline');
var serial = require('./serial.js');
var socket = require('./socket.js');
var bridge = undefined;
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
console.log(`firmware: ${cfgFile.device.firmware}`);

if(cfgFile.device.serial_port !== undefined) {
  console.log(`dev: ${cfgFile.device.serial_port}`);
  console.log(`bps: ${cfgFile.device.baud_rate}`);
  serial.create(cfgFile.device.serial_port, cfgFile.device.baud_rate, initSerial, deInitSerial, processPacket);
  bridge = serial;
} else if(cfgFile.device.host !== undefined) {
  console.log(`host: ${cfgFile.device.host}`);
  console.log(`port: ${cfgFile.device.port}`);
  socket.create(cfgFile.device.host, cfgFile.device.port, initSerial, deInitSerial, processPacket);
  bridge = socket;
} else {
  console.log('Invalid configuration file, please specify serial port for remote host');
  process.exit(1);
}

//



try {
  bridge.start();
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
  bridge.unlock();
  var ret = bl.process(bridge.incomingData(), bridge);
  if(ret == 2) {
    console.log('Firmware updated successfully');
    bridge.stop();
    process.exit(0);
  } else if(ret == 3) {
    console.log('Error while updating firmware');
    bridge.stop();
    process.exit(1);
  }
}
