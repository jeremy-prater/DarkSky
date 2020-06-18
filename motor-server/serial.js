const { jspack } = require('jspack');
const SerialPort = require('serialport');
const ByteLength = require('@serialport/parser-byte-length');

let buffer = [];

// Decode one packet of data
function addToQueue(data, callback) {
    // console.log('addToQueue');
    // console.log(data);
    buffer.push(data);

    if (buffer.length == 16) {
        const packet = buffer.map((node) => node[0]);
        // console.log(`packet len : ${packet.length}`);
        // console.log(packet);

        //
        // The wire protocol
        //
        // typedef struct {
        //     uint32_t header;
        //     uint16_t command;
        //     uint16_t arg1;
        //     uint16_t arg2;
        //     uint16_t arg3;
        //     uint32_t checksum;
        //   } CommPacket; // Communication packet is 16 bytes
        //
        // TODO : I wish I cared about checksum....
        const stateUpdate = jspack.Unpack('<IHHHHI', packet, 0);
        // console.log(stateUpdate);
        if (stateUpdate[0] == 0xdadaf00d) {
            buffer = [];
            // console.log('header detected');
            // console.log(stateUpdate);
            const payload = {
                magic: stateUpdate[0],
                command: stateUpdate[1],
                arg1: stateUpdate[2],
                arg2: stateUpdate[3],
                arg3: stateUpdate[4],
                checksum: stateUpdate[5]
            };
            // console.log(payload);
            callback(payload);
        } else {
            buffer.shift();
        }
    }
}

module.exports = {
    AddSerialHandler: function(callback) {
        const port = new SerialPort('/dev/ttyUSB0', {
            baudRate: 115200,
        });
        
        const parser = port.pipe(new ByteLength({ length: 1 }));
        parser.on('data', (data) => {
            addToQueue(data, callback);
        });      

        return port;
    }
};