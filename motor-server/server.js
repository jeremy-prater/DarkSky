const { jspack } = require('jspack');

let buffer = [];

// Decode one packet of data
function addToQueue(data) {
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
            console.log('header detected');
            console.log(stateUpdate);
        } else {
            buffer.shift();
        }
    }
}

const SerialPort = require('serialport');
const port = new SerialPort('/dev/ttyUSB0', {
    baudRate: 115200,
});

// Phase 1 : Dump serial to console

// const ByteLength = require('@serialport/parser-byte-length')
// const parser = port.pipe(new ByteLength({length: 1}))
// parser.on('data', (data) => {
//     console.log(data);
// });

// Step 2: Identify patterns in serial stream

// const Ready = require('@serialport/parser-ready');
// const parser = port.pipe(new Ready({ delimiter: [0xdadaf00d] }));
// parser.on('ready', () => console.log('Packet in'));

// Step 3 : Delimiter

// const Delimiter = require('@serialport/parser-delimiter');
// const parser = port.pipe(new Delimiter({ delimiter: [0xdadaf00d] }));
// parser.on('data', (data) => {
//     processData(data);
// });

// Custom parser
const ByteLength = require('@serialport/parser-byte-length');
const parser = port.pipe(new ByteLength({ length: 1 }));
parser.on('data', (data) => {
    addToQueue(data);
});
