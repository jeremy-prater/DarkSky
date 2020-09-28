import Vue from 'vue';
import Vuex from 'vuex';
const lodash = require('lodash');
// const common = require('../components/common');

Vue.use(Vuex);

export default new Vuex.Store({
    state: {},
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
            // state.actual.dish.historyPath = newState.dish.historyPath;
            // state.actual.dish.historyStrength = newState.dish.historyStrength;

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
            state; payload;
            // state.requested.decMotor.state = payload;
        },
        requestDecMotorPosition(state, payload) {
            state; payload;
            // state.requested.decMotor.position = payload;
        },
        requestDecMotorStopPos(state, payload) {
            state; payload;
            // state.requested.decMotor.stopPos = payload;
        },
        requestRAMotorState(state, payload) {
            state; payload;
            // state.requested.raMotor.state = payload;
        },
        requestRAMotorPosition(state, payload) {
            state; payload;
            // state.requested.raMotor.position = payload;
        },
        requestRAMotorStopPos(state, payload) {
            state; payload;
            // state.requested.raMotor.stopPos = payload;
        },
        requestLNBVoltage(state, payload) {
            state; payload;
            // state.requested.lnbState.voltage = payload;
        },
        requestLNBCarrier(state, payload) {
            state; payload;
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
            if (key in state) {
                return state[key];
            }
            else
            {
                return undefined;
            }
        }
    },
    actions: {
    },
    modules: {},
});
