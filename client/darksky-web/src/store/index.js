import Vue from 'vue';
import Vuex from 'vuex';
const lodash = require('lodash');

Vue.use(Vuex);

export default new Vuex.Store({
    state: {
        motorServerConnected: false,
        gps: {
            mode: 0,
            sats: 0,
            lat: 0,
            lon: 0,
            time: new Date(
                Date.parse('2000-01-01T11:58:55.816Z')
            ).toISOString(),
            error: '',
            alt: 0,
            climb: 0,
        },
        jde: 0,
        lnb: {
            voltage: 0,
            carrier: false,
            strength: 0,
        },
        dish: {
            az: 0,
            alt: 0,
            historyPath: [],
            historyStrength: [],
        },
        motors: {
            stopAll: false,
            ra: {
                state: 'unknown',
                position: 0,
                stopAt: 0,
            },
            dec: {
                state: 'unknown',
                position: 0,
                stopAt: 0,
            },
        },
        serial: {
            port: '',
            connected: false,
        },
        sdr: {},
        sky: {
            ra: 0,
            dec: 0,
        },
    },
    mutations: {
        setMotorConnection(state, connected) {
            if (connected) {
                console.log('Motor server connected');
            } else {
                console.log('Motor server disconnected');
            }
            state.motorServerConnected = connected;
        },
        updateState(state, newState) {
            state = lodash.merge(state, newState);
            state.dish.historyPath = newState.dish.historyPath;
            state.dish.historyStrength = newState.dish.historyStrength;
        },
        updateJDE(state, jde) {
            state.jde = jde;
        },
        updateTime(state, time) {
            state.location.time = time;
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
