var app = require('express')();
var server = require('http').Server(app);
var io = require('socket.io')(server);

const listenPort = 42182;

server.listen(listenPort);

const index = require('./routes/index');

app.use(index);
let interval;

io.on('connection', socket => {
    console.log('Server : New Client!');

    if (interval) {
        clearInterval(interval);
    }

    interval = setInterval(() => {
        // Setup new Client state here!!
        serverTick(socket), 100;
    });

    socket.on('disconnect', () => {
        console.log('Server : Client Disconnected...');
        interval = clearInterval(interval);
    });
});

const serverTick = async socket => {
    console.log('tick');
    try {
        // Do something dangerous
        socket.emit('FromAPI', "I'm a test!");
    } catch (error) {
        console.error(`Error: ${error.code}`);
    }
};

console.log(`Server : Start up --> listening on port ${listenPort}`);

const SerialPort = require('serialport');

//const port = new SerialPort('/dev/ttyUSB0');

// macOS identifier for test cable /dev/tty.usbserial-AC01HSL7
const port = new SerialPort('/dev/tty.usbserial-AC01HSL7');

port.write('main screen turn on', function(err) {
    if (err) {
        return console.log('Error on write: ', err.message);
    }
    console.log('message written');
});

// Open errors will be emitted as an error event
port.on('error', function(err) {
    console.log('Error: ', err.message);
});

port.on('data', function(data) {
    console.log('Data:', data);
});
