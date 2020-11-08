import Vue from 'vue';
import VueRouter from 'vue-router';
import Vuex from 'vuex';

import App from './App.vue';

Vue.config.productionTip = false;

import BootstrapVue from 'bootstrap-vue';
import 'bootstrap/dist/css/bootstrap.css';
import 'bootstrap-vue/dist/bootstrap-vue.css';

import VueObserveVisibility from 'vue-observe-visibility';

//const remote = require('electron').remote;
//const fs = remote.require('fs');
//const path = require('path');
//const uuidv1 = require('uuid/v1');

Vue.use(BootstrapVue);
Vue.use(VueRouter);
Vue.use(Vuex);
Vue.use(VueObserveVisibility);

import Scanhome from './components/Scanhome.vue';

const router = new VueRouter({
    mode: 'history',
    base: __dirname,
    routes: [{ path: '/', component: Scanhome }]
});

const defaultState = function() {
    return {
        endpoint: 'http://127.0.0.1:8100',
        comports: [],
        currentComport: undefined,
        comportState: false,
        mcpState: {
            dec: {
                position: 0,
                state: 'unknown'
            },
            ra: {
                position: 0,
                state: 'unknown'
            },
            lnb: 'unknown'
        }
    };
};

const store = new Vuex.Store({
    state: defaultState(),
    mutations: {
        // Comport Mutations
        setComports(state, comports) {
            state.comports = comports;
            console.log(`setComports(${JSON.stringify(comports)})`);
        },
        selectComport(state, comport) {
            console.log(`selectComport(${JSON.stringify(comport)})`);
            state.currentComport = comport;
        },
        comportStatus(state, comportStatus) {
            console.log(`comportStatus(${JSON.stringify(comportStatus)})`);
            state.comportState = comportStatus;
        },
        // MCP Mutations
        signalBoot(state, value) {
            if (value == true) {
                console.log('MCP booted');
            }
        },
        signalMororAltPosition(state, value) {
            if (value != state.mcpState.alt.position) {
                state.mcpState.alt.position = value;
                console.log(`mcpState.alt.position = ${value}`);
            }
        },
        signalMororAltState(state, value) {
            if (value != state.mcpState.alt.state) {
                state.mcpState.alt.state = value;
                console.log(`mcpState.alt.state = ${value}`);
            }
        },
        signalMororAzPosition(state, value) {
            if (value != state.mcpState.az.position) {
                state.mcpState.az.position = value;
                console.log(`mcpState.az.position = ${value}`);
            }
        },
        signalMororAzState(state, value) {
            if (value != state.mcpState.az.state) {
                state.mcpState.az.state = value;
                console.log(`mcpState.az.state = ${value}`);
            }
        },
        signalLNBPower(state, value) {
            if (value != state.mcpState.lnb) {
                state.mcpState.lnb = value;
                console.log(`mcpState.lnb = ${value}`);
            }
        }
    }
});

new Vue({
    router,
    store,
    render: h => h(App)
}).$mount('#app');
