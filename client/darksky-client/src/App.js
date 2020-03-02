import React, { Component } from 'react';
import socketIOClient from 'socket.io-client';
import 'bootstrap/dist/css/bootstrap.min.css';
import StatusBar from './components/statusbar';
import './App.css';

class App extends Component {
    constructor() {
        super();
        this.state = {
            comports: [],
            comport: {
                comport: {},
                connected: false
            },
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

        socket.on('comport.update', comports => {
            console.log(comports);
            this.setState({ response: comports[0].device });
        });
        socket.on('comport.status', comstatus => {
            console.log(comstatus);
        });
    }

    render() {
        return (
            <div class="App">
                <StatusBar class="StatusBar"></StatusBar>
                <div class="View"></div>
                <div class="BottomBar"></div>
            </div>
        );
    }
}

export default App;
