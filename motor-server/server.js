const serial = require('./serial');

const io = require('socket.io');
const server = io.listen(22502);

let activeConnections = {};

const serialPort = serial.AddSerialHandler(sendState);

server.on('connection', function (socket) {
    const userId = socket.id;
    console.log(`User Connected [${userId}]`);
    activeConnections[userId] = socket;
    
    socket.on('disconnect', (reason) => {
        console.log(`User Disconnected [${userId}] ${reason}`);
        delete activeConnections[userId];
    });

    socket.on('update', (payload) => {
        console.log(payload);
        serialPort.write(payload);
    });
});

function sendState(data) {
    for (const userId in activeConnections) {
        activeConnections[userId].emit('updateState', data);
    }
}
