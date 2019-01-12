/* jslint node: true, sub: true */
/* jshint esversion: 6 */
'use strict';

var SerialPort = require("serialport");



var mySingleton = (function () {

    // Instance stores a reference to the Singleton
    var instance;
    var serialPort;
    var incomingDataLock = false;
    var incomingData = null;

  function init() {

        // Singleton

        /* ------------------------------------------------------------ */

    return {

            create: (serialPortName, serialPortBaud, openCB, closeCB, recvCB) => {

                serialPort = new SerialPort(serialPortName, {
                  autoOpen: false,
                  baudRate: serialPortBaud
                });

                serialPort.on("open", function () {
                openCB();
                serialPort.on('data', function(data) {

                  if(!incomingDataLock) {
                    incomingDataLock = true;
                    incomingData = new Buffer(data)
                    setTimeout(recvCB, 80);
                  } else {
                    incomingData = Buffer.concat([incomingData, new Buffer(data)]);
                  }
                });
              });

              serialPort.on('close', function() {
                closeCB();
              });

            },

            start: () => {
                 return serialPort.open();
            },

            stop: () => {
                serialPort.close();
            },

            unlock: () => {
                incomingDataLock = false;
            },

            incomingData: () => {
                return incomingData;
            },

            send: (data) => {
              if(data != null) {
                serialPort.write(data, function (err, result) {
                  if (err) {
                    console.log('Error while sending packet : ' + err);
                  }
                });
              }
            },
    };

  };

  return {

    // Get the Singleton instance if one exists
    // or create one if it doesn't
    getInstance: function () {

      if ( !instance ) {
        instance = init();
      }

      return instance;
    }

  };

})();

module.exports = mySingleton.getInstance();
