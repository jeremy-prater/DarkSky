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
    routes: [
        { path: '/', component: Scanhome },
    ]
});

const store = new Vuex.Store({
    state: {
    },
    mutations: {
    }
});

new Vue({
    router,
    store,
    render: h => h(App)
}).$mount('#app');
