import Vue from 'vue';
import App from './App.vue';
import store from './store';
import { BootstrapVue, IconsPlugin } from 'bootstrap-vue';
import VueObserveVisibility from 'vue-observe-visibility';
import VueSocketIO from 'vue-socket.io';
import vuetify from './plugins/vuetify';

import 'bootstrap/dist/css/bootstrap.css'
import 'bootstrap-vue/dist/bootstrap-vue.css'

// Install BootstrapVue
Vue.use(BootstrapVue);
// Optionally install the BootstrapVue icon components plugin
Vue.use(IconsPlugin);
Vue.use(VueObserveVisibility);
Vue.use(
    new VueSocketIO({
        connection: `${window.location.hostname}:22502`,
        vuex: {
            store,
            actionPrefix: 'SOCKET_',
            mutationPrefix: 'SOCKET_',
        },
        debug: false,
    })
);

Vue.config.productionTip = false;

new Vue({
    store,
    vuetify,
    render: (h) => h(App),
}).$mount('#app');
