import React, { Component } from 'react';
import socketIOClient from 'socket.io-client';

class App extends Component {
    constructor() {
        super();
        this.state = {
            response: false,
            endpoint: 'http://127.0.0.1:8100'
        };
    }

    componentDidMount() {
        const { endpoint } = this.state;
        console.log(`Connecting to ${endpoint}`);
        const socket = socketIOClient(endpoint);

        if (!socket.connected) {
            console.error(`Failed to connect to ${endpoint}`);
        }

        socket.on('COMMAND_REBOOT', packet => {
            console.log(packet);
            this.setState({ response: packet.command });
        });
    }

    render() {
        const { response } = this.state;
        return (
            <div style={{ textAlign: 'center' }}>
                {response ? (
                    <p>The response is : {response}</p>
                ) : (
                    <p>Loading...</p>
                )}
            </div>
        );
    }
}

export default App;
