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
            state.image['dish.historyStrength'] = newState['dish.historyStrength'];

            // if (state.firstUpdate) {
            //     state.firstUpdate = false;
            //     state.requested.calibrating = state.actual.calibrating;
            //     state.requested.lnb = state.actual.lnb;
            //     state.requested.motors = state.actual.motors;
            // }

            // // Check for state differences!
            // const stateDiff = common.default.diffObjects(state.requested, state.actual);
            // console.log(JSON.stringify(stateDiff));

            // if (state.requested.motors.stopAll && state.actual.motors.stopAll) {
            //     console.log("Motor Stop All acknowledged!");
            //     this.commit("requestStopAll", false);
            // }

            // if (state.requested.motors.stopAll && state.actual.motors.stopAll) {
            //     console.log("Motor Stop All acknowledged!");
            //     this.commit("requestStopAll", false);
            // }
        },
        requestStopAll(state, payload) {
            state.requested.motors.stopAll = payload;
            this._vm.$socket.emit('stopAll', payload);
        },
        requestDecMotorState(state, payload) {
            state;
            payload;
            // state.requested.decMotor.state = payload;
        },
        requestDecMotorPosition(state, payload) {
            state;
            payload;
            // state.requested.decMotor.position = payload;
        },
        requestDecMotorStopPos(state, payload) {
            state;
            payload;
            // state.requested.decMotor.stopPos = payload;
        },
        requestRAMotorState(state, payload) {
            state;
            payload;
            // state.requested.raMotor.state = payload;
        },
        requestRAMotorPosition(state, payload) {
            state;
            payload;
            // state.requested.raMotor.position = payload;
        },
        requestRAMotorStopPos(state, payload) {
            state;
            payload;
            // state.requested.raMotor.stopPos = payload;
        },
        requestLNBVoltage(state, payload) {
            state;
            payload;
            // state.requested.lnbState.voltage = payload;
        },
        requestLNBCarrier(state, payload) {
            state;
            payload;
            // state.requested.lnbState.carrier = payload;
        },
        requestCalibration(state, payload) {
            state;
            // state.requested.calibrating = payload;
            this._vm.$socket.emit('calibration', payload);
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
