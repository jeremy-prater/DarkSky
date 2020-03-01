var Struct = require('struct');

// Tell the motor and power controller to do a warm reset.
//
// This shuts off the LMB and stops all drive motors
//
// This should also reset the SDR state ... maybe?
//

// Here we define the structure of the data over the wrire
// to the mpc
//
// This needs to be in sync with comm.h from the star-motor-controller project
//
// typedef struct {
// 	uint32_t header;
// 	uint16_t command;
// 	uint16_t arg1;
// 	uint16_t arg2;
// 	uint16_t arg3;
// 	uint32_t checksum;
// } CommPacket; // Communication packet is 16 bytes
//


class CommPacket {
    static COMM_PACKET_HEADER = 0xDADA
    static COMM_PACKET_COMMAND = {
        SIGNAL_BOOT: 0,
        SIGNAL_MOTOR_DEC_STATE: 1,
        SIGNAL_MOTOR_DEC_POSITION: 2,
        SIGNAL_MOTOR_RA_STATE: 3,
        SIGNAL_MOTOR_RA_POSITION: 4,
        SIGNAL_LNB_POWER_STATE: 5,
        COMMAND_MOTOR_DEC_SET_STATE: 6,
        COMMAND_MOTOR_DEC_SET_POSITION: 7,
        COMMAND_MOTOR_RA_SET_STATE: 8,
        COMMAND_MOTOR_RA_SET_POSITION: 9,
        COMMAND_LNB_SET_POWER_STATE: 10,
        COMMAND_STOP_ALL_MOTORS: 11,
        ERROR: 12
    }
    constructor(cmd, a1, a2, a3)
    {
        console.log (`Creating new outgoing command`);
        this.MPCPacket = Struct()
            .word32Ule('header')
            .word16Ule('command')
            .word16Ule('arg1')
            .word16Ule('arg2')
            .word16Ule('arg3')
            .word32Ule('checksum');
        this.MPCPacket.allocate();
        this.buffer = this.MPCPacket.buffer();
        //this.rawBuffer = this.MPCPacket.buffer();
        this.proxy = this.MPCPacket.fields;

        this.proxy.header = CommPacket.COMM_PACKET_HEADER;
        this.proxy.command = cmd;
        this.proxy.arg1 = a1;
        this.proxy.arg2 = a2;
        this.proxy.arg3 = a3;
        this.proxy.checksum = 0x3838;
    }
}

// export default CommPacket;
module.exports = CommPacket;