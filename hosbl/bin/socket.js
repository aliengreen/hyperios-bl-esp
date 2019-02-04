/* jslint node: true, sub: true */
/* jshint esversion: 6 */
'use strict';

var net = require("net");



var mySingleton = (function () {

    // Instance stores a reference to the Singleton
    var instance;
    var client = new net.Socket();
    var incomingDataLock = false;
    var incomingData = null;

  function init() {

        // Singleton

        /* ------------------------------------------------------------ */

    return {

            create: (host, port, openCB, closeCB, recvCB) => {

              client.connect(port, host, function() {
              	console.log('Connected');
                openCB();
                  client.on('data', function(data) {
                    if(!incomingDataLock) {
                      incomingDataLock = true;
                      incomingData = new Buffer(data)
                      setTimeout(recvCB, 10);
                    } else {
                      incomingData = Buffer.concat([incomingData, new Buffer(data)]);
                    }
                  	// console.log('Received: ' + data);
                  	// client.destroy(); // kill client after server's response
                  });
              });

              client.on('close', function() {
                closeCB();
              	console.log('Connection closed');
              });
              //   serialPort = new SerialPort(serialPortName, {
              //     autoOpen: false,
              //     baudRate: serialPortBaud
              //   });
              //
              //   serialPort.on("open", function () {
              //   openCB();
              //   serialPort.on('data', function(data) {
              //
              //     if(!incomingDataLock) {
              //       incomingDataLock = true;
              //       incomingData = new Buffer(data)
              //       setTimeout(recvCB, 80);
              //     } else {
              //       incomingData = Buffer.concat([incomingData, new Buffer(data)]);
              //     }
              //   });
              // });

              // serialPort.on('close', function() {
              //   closeCB();
              // });

            },

            start: () => {
                 // return serialPort.open();
            },

            stop: () => {
                // serialPort.close();
                client.destroy();
            },

            unlock: () => {
                incomingDataLock = false;
            },

            incomingData: () => {
                return incomingData;
            },

            send: (data) => {
              if(data != null) {
                client.write(data);
                //
                // serialPort.write(data, function (err, result) {
                //   if (err) {
                //     console.log('Error while sending packet : ' + err);
                //   }
                // });
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
