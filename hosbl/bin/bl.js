/* jslint node: true, sub: true */
/* jshint esversion: 6 */
'use strict';

const fs = require('fs');
const { crc16ccitt } = require('crc');
var ProgressBar = require('progress');

var mySingleton = (function() {

  // Instance stores a reference to the Singleton
  var instance;
  var pageSize = 128;
  var pageCount = 224;

  function init() {

    // Singleton

    // 224 pages (page size 128 bytes)
    var frmb = Buffer.alloc(pageCount * pageSize, 0xFF);
    let pages = frmb.length / pageSize;
    var bar = new ProgressBar('  uploading [:bar] :rate/pps :percent :etas', {
    complete: '=',
    incomplete: ' ',
    width: 20,
    total: pages
  });
    /* ------------------------------------------------------------ */

    return {

      firmware: (filename) => {
          let frm = fs.readFileSync(filename);
          frm.copy(frmb, 0, 0, frm.length);
      },

      process: (packet, serial) => {

        var data = packet.toString();
        var lines = data.split('\r\n');

        let buffer = Buffer.alloc(3);
        for(var i = 0; i < lines.length; i++) {
          var param = lines[i].split('=');

          if(param[0] == 'FCHK') {
              var num = parseInt(param[1]);
              if(num == 2) {         
                return num;
              } else if(num == 3) {
                return num;
              } else {
                buffer.writeUInt8(0xFD, 0);
                buffer.writeUInt16LE(crc16ccitt(frmb), 1);
                serial.send(buffer);
              }
          } if(param[0] == 'FRPG') {
              var p = parseInt(param[1]);
              if(p < pages ) {
                var pg = frmb.slice(p * pageSize, (p + 1) * pageSize);
                serial.send(pg);
                bar.tick();
            } 

          } 
        }

        return 1;
      }


    };

  };

  return {

    // Get the Singleton instance if one exists
    // or create one if it doesn't
    getInstance: function() {

      if (!instance) {
        instance = init();
      }

      return instance;
    }

  };

})();

module.exports = mySingleton.getInstance();
