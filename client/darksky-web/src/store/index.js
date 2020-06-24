import Vue from 'vue';
import Vuex from 'vuex';

Vue.use(Vuex);

export default new Vuex.Store({
    state: {
        motorServerConnected: false,
        decMotor: {
            state: null,
            position: 0,
            stopPos: 0,
        },
        raMotor: {
            state: null,
            position: 0,
            stopPos: 0,
        },
        allMotorStop: false,
        lnbState: {
            voltage: 0,
            carrier: false,
        },
        location: {
            lat: 45.663177,
            lng: -122.710401,
            time: "no-lock"
        },
    },
    mutations: {
        updateTime(state, time) {
            // console.log(`updateTime ${time}`);
            state.location.time = time;
        },
        setMotorConnection(state, connected) {
            if (connected) {
                console.log('Motor server connected');
            } else {
                console.log('Motor server disconnected');
            }
            state.motorServerConnected = connected;
        },
        updateDecMotorState(state, payload) {
            state.decMotor.state = payload;
        },
        updateDecMotorPosition(state, payload) {
            state.decMotor.position = payload;
        },
        updateDecMotorStopPos(state, payload) {
            state.decMotor.stopPos = payload;
        },
        updateRAMotorState(state, payload) {
            state.raMotor.state = payload;
        },
        updateRAMotorPosition(state, payload) {
            state.raMotor.position = payload;
        },
        updateRAMotorStopPos(state, payload) {
            state.raMotor.stopPos = payload;
        },
        updateLNBVoltage(state, payload) {
            state.lnbState.voltage = payload;
        },
        updateLNBCarrier(state, payload) {
            state.lnbState.carrier = payload;
        },
        updateAllMotorStop(state, payload) {
            state.allMotorStop = payload;
        },
    },
    actions: {},
    modules: {},
});
