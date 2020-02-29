import React, { Component } from "react";
import socketIOClient from "socket.io-client";

class App extends Component {
  constructor() {
    super();
    this.state = {
      response: false,
      endpoint: "http://127.0.0.1:42182"
    };
  }

  componentDidMount() {
    const { endpoint } = this.state;
    console.log(`Connecting to ${endpoint}`);
    const socket = socketIOClient(endpoint);

    if (!socket.connected)
    {
      console.error(`Failed to connect to ${endpoint}`);
    }

    console.log(socket);
    socket.on("FromAPI", data => this.setState({ response: data }));
  }

  render() {
    const { response } = this.state;
    return (
        <div style={{ textAlign: "center" }}>
          {response
              ? <p>
                The response is : {response}
              </p>
              : <p>Loading...</p>}
        </div>
    );
  }
}

export default App;