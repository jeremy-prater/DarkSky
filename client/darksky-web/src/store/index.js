import Vue from 'vue';
import Vuex from 'vuex';
const lodash = require('lodash');
// const common = require('../components/common');

Vue.use(Vuex);

export default new Vuex.Store({
    state: {
        motorServerConnected: false,
        image: {
            'dish.az': 0,
            'dish.alt': 0,
            calibrating: false,
            'gps.mode': 0,
            'lnb.voltage': 0,
            'lnb.carrier': false,
            'lnb.strength': 0,
            'dish.historyPath': [],
            'dish.historyStrength': [],
            'motors.stopAll': false,
            'motors.ra.state': 'unknown',
            'motors.ra.position': 0,
            'motors.ra.delta': 0,
            'motors.dec.state': 'unknown',
            'motors.dec.position': 0,
            'motors.dec.delta': 0,
            'serial.port': '',
            'serial.connected': false,
            'time.jde': 0,
            'time.sidereal.local': 0,
            'time.sidereal.gmt': 0,
            'gps.sats': 0,
            'gps.lat': 0,
            'gps.lon': 0,
            'gps.time': '',
            'gps.error': '',
            'gps.alt': 0,
            'gps.climb': 0,
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
            state.image = lodash.merge(state.image, newState);
            state.image['dish.historyPath'] = newState['dish.historyPath'];
            state.image['dish.historyStrength'] =
                newState['dish.historyStrength'];
        },
        requestStopAll(state, payload) {
            state;
            this._vm.$socket.emit('requestState', {
                'motors.stopAll': payload,
            });
        },
        requestDecMotorState(state, payload) {
            state;
            payload;
        },
        requestDecMotorPosition(state, payload) {
            state;
            payload;
        },
        requestDecMotorStopPos(state, payload) {
            state;
            payload;
        },
        requestRAMotorState(state, payload) {
            state;
            payload;
        },
        requestRAMotorPosition(state, payload) {
            state;
            payload;
        },
        requestRAMotorStopPos(state, payload) {
            state;
            payload;
        },
        requestLNBVoltage(state, payload) {
            state;
            payload;
        },
        requestLNBCarrier(state, payload) {
            state;
            payload;
        },
        requestCalibration(state, payload) {
            state;
            this._vm.$socket.emit('requestState', { calibrating: payload });
        },
    },
    getters: {
        getByKey: (state) => (key) => {
            if (key in state.image) {
                return state.image[key];
            } else {
                return undefined;
            }
        },
    },
    actions: {},
    modules: {},
});
