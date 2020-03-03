import React, { Component } from 'react';
import './statusbar.css';
import Navbar from 'react-bootstrap/Navbar';
import Nav from 'react-bootstrap/Nav';
import NavDropdown from 'react-bootstrap/NavDropdown';
import Button from 'react-bootstrap/Button';
import Form from 'react-bootstrap/Form';
import FormControl from 'react-bootstrap/FormControl';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import {
    faSatelliteDish,
    faPlug,
    faPowerOff
    //faCheckCircle,
} from '@fortawesome/free-solid-svg-icons';
import './statusbar.css';

class StatusBar extends Component {
    render() {
        return (
            <Navbar bg="dark" expand="lg" className="navbar-dark">
                <FontAwesomeIcon
                    icon={faSatelliteDish}
                    size="2x"
                    color="orange"
                    className="LogoIcon"
                />
                <Navbar.Brand href="#home">DarkSky</Navbar.Brand>
                <Navbar.Toggle aria-controls="basic-navbar-nav" />
                <Navbar.Collapse id="basic-navbar-nav">
                    <Nav className="mr-auto">
                        <Nav.Link href="#manuak">Manual Tracking</Nav.Link>
                        <Nav.Link href="#target">Align to target</Nav.Link>

                        <NavDropdown title="Serial Port" id="comport">
                            <NavDropdown.Item href="#com/1">
                                com1
                            </NavDropdown.Item>
                            <NavDropdown.Item href="#com/2">
                                com2
                            </NavDropdown.Item>
                            <NavDropdown.Item href="#com/3">
                                Something
                            </NavDropdown.Item>
                            <NavDropdown.Divider />
                            <NavDropdown.Item href="#com/usb">
                                First USB
                            </NavDropdown.Item>
                        </NavDropdown>
                        <Button variant="outline-danger">
                            <FontAwesomeIcon
                                icon={faPlug}
                                size="lg"
                                color="orange"
                            />
                        </Button>

                        <NavDropdown title="LNB Power" id="lnb_power">
                            <NavDropdown.Item href="#lnb/off">
                                Off
                            </NavDropdown.Item>
                            <NavDropdown.Item href="#lnb/auto">
                                Auto
                            </NavDropdown.Item>
                            <NavDropdown.Divider />
                            <NavDropdown.Item href="#lnb/0">
                                Mode 0 : +13 vdc @ 0 Hz
                            </NavDropdown.Item>
                            <NavDropdown.Item href="#lnb/1">
                                Mode 1 : +13 vdc @ 22 KHz
                            </NavDropdown.Item>
                            <NavDropdown.Item href="#lnb/2">
                                Mode 2 : +18 vdc @ 22 KHz
                            </NavDropdown.Item>
                            <NavDropdown.Item href="#lnb/3">
                                Mode 3 : +18 vdc @ 0 KHz
                            </NavDropdown.Item>
                        </NavDropdown>
                        <Button variant="outline-danger">
                            <FontAwesomeIcon
                                icon={faPowerOff}
                                size="lg"
                                color="gray"
                            />
                        </Button>
                    </Nav>
                    <Form inline>
                        <FormControl
                            type="text"
                            placeholder="Search"
                            className="mr-sm-2"
                        />
                        <Button variant="outline-success">Search</Button>
                    </Form>
                </Navbar.Collapse>
            </Navbar>
        );
    }
}

export default StatusBar;
