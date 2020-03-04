import React from 'react';
import './statusbar.css';
import NavDropdown from 'react-bootstrap/NavDropdown';
import './statusbar.css';

class ComportMenu extends React.Component {
    constructor(props) {
        super(props);
        console.log('Comport Menu !');
        console.log(this.props);
    }

    generateComportList() {
        let newComports = [];

        this.comPortsList = this.props.comports.map(comport => {
            newComports.append(comport);
        });
        console.log(this.comPortsList);
        this.setState({
            comports: newComports
        });
    }

    componentDidMount()
    {
        console.log("Comport Menu Mounted!");
        console.log(this);
                
    }

    render() {
        return (
            <NavDropdown title="Serial Port" id="comport">
                <NavDropdown.Item href="#com/1">test</NavDropdown.Item>

                <NavDropdown.Divider />
                <NavDropdown.Item href="#com/usb">First USB</NavDropdown.Item>
            </NavDropdown>
        );
    }
}

export default ComportMenu;