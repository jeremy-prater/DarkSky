import Vue from 'vue';
import Vuex from 'vuex';
const lodash = require('lodash');
// const common = require('../components/common');

Vue.use(Vuex);

export default new Vuex.Store({
    state: {
        firstUpdate: true,
        requested: {
            calibrating: false,
            lnb: {
                voltage: 0,
                carrier: false,
            },
            motors: {
                stopAll: false,
                ra: {
                    state: 'stopped',
                    position: 0,
                    delta: 0,
                },
                dec: {
                    state: 'stopped',
                    position: 0,
                    delta: 0,
                },
            },
        },
        actual: {
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
            calibrating: false,
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
                    delta: 0,
                },
                dec: {
                    state: 'unknown',
                    position: 0,
                    delta: 0,
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
    },
    mutations: {
        setMotorConnection(state, connected) {
            if (connected) {
                console.log('Motor server connected');
            } else {
                console.log('Motor server disconnected');
            }
            state.actual.motorServerConnected = connected;
            state.firstUpdate = true;
        },
        updateState(state, newState) {
            state.actual = lodash.merge(state.actual, newState);
            state.actual.dish.historyPath = newState.dish.historyPath;
            state.actual.dish.historyStrength = newState.dish.historyStrength;

            if (state.firstUpdate) {
                state.firstUpdate = false;
                state.requested.calibrating = state.actual.calibrating;
                state.requested.lnb = state.actual.lnb;
                state.requested.motors = state.actual.motors;
            }

            // Check for state differences!
            // const stateDiff = common.default.diffObjects(state.requested, state.actual);
            // console.log(JSON.stringify(stateDiff));
        },
        updateJDE(state, jde) {
            state.actual.jde = jde;
        },
        requestDecMotorState(state, payload) {
            state.requested.decMotor.state = payload;
        },
        requestDecMotorPosition(state, payload) {
            state.requested.decMotor.position = payload;
        },
        requestDecMotorStopPos(state, payload) {
            state.requested.decMotor.stopPos = payload;
        },
        requestRAMotorState(state, payload) {
            state.requested.raMotor.state = payload;
        },
        requestRAMotorPosition(state, payload) {
            state.requested.raMotor.position = payload;
        },
        requestRAMotorStopPos(state, payload) {
            state.requested.raMotor.stopPos = payload;
        },
        requestLNBVoltage(state, payload) {
            state.requested.lnbState.voltage = payload;
        },
        requestLNBCarrier(state, payload) {
            state.requested.lnbState.carrier = payload;
        },
        requestCalibration(state, payload) {
            state.requested.calibrating = payload;
            this._vm.$socket.emit('request.calibration', payload);
        },
    },
    actions: {},
    modules: {},
});
