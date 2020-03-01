var app = require('express')();
var server = require('http').Server(app);
var io = require('socket.io')(server);
const SerialPort = require('serialport');

var CommPacket = require('./comm-packet');

const listenPort = 42182;

server.listen(listenPort);

const index = require('./routes/index');

app.use(index);

// Feature : server driven events from a timer
//let interval;

io.on('connection', socket => {
    console.log('Server : New Client!');

    // Feature : server driven events from a timer
    // Not sure what we need this for yet
    // if (interval) {
    //     clearInterval(interval);
    // }

    // interval = setInterval(() => {
    //     // Setup new Client state here!!
    //     //serverTick(socket), 100;
    // });

    socket.on('disconnect', () => {
        console.log('Server : Client Disconnected...');
        interval = clearInterval(interval);
    });
});

// Feature : server driven events from a timer
// const serverTick = async socket => {
//     try {
//         // Do something dangerous
//         socket.emit('FromAPI', "I'm a test!");
//     } catch (error) {
//         console.error(`Error: ${error.code}`);
//     }
// };

console.log(`Server : Start up --> listening on port ${listenPort}`);

//const port = new SerialPort('/dev/ttyUSB0');

// macOS identifier for test cable /dev/tty.usbserial-AC01HSL7
const port = new SerialPort('/dev/tty.usbserial-AC01HSL7');

port.on('update', function(data) {
    console.log('Data:', data);
});

var warnRebootCommand = new CommPacket(
    CommPacket.COMM_PACKET_COMMAND.SIGNAL_BOOT,
    0,
    0,
    0
);

port.write(warnRebootCommand.buffer, function(err) {
    if (err) {
        return console.log('Error on write: ', err.message);
    }
    console.log('message written');
});
