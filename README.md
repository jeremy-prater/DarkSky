#DarkSky

Darksky is a satellite tracking and low quality radio telescope project

![System Diagram](/diagrams/DarkSky System.png)

By re-using a Ku band LNB and a parabolic satellite dish along with some basic micro-controllers, tracking objects in the sky should be relatively easy.

The LNB is powered by a LNBP9-LIT voltage regulator which outputs 13VDC or 18VDC with an optional 22KHz carrier signal

The drive motors are +/- 12 VDC 9 RPM worm gear drivers with 3.3VDC quadrature encoders with 11000 pulses per revolution. These were chosen because of the high torque rating and stepper motors can slip and require constant power to hold the rotor stationary.

The Arduino controls the drive motors and receives interrupts from the encoders. It also controls the voltage and carrier frequency of the LNB9P-LIT.

The Arduino communicates to a Raspberry PI 4 over a USB serial interface. The Raspberry PI will also have a GPS receiver so computation of right ascension and declination is possible. The Raspberry PI will also run a web server that will serve data from the LNB to the web application client.

The LNB is powered by the LNBP9-LIT though a RF power injector. The output of the rf power injector is fed into a HackRF one which is a software defined radio. This uses the gnuradio software package which will be running on the raspberry pi. The transport between gnuradio and the web client will be ZeroMQ.