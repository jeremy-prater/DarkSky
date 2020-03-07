<template>
  <div>
    <b-navbar fixed="bottom" class="darksky-navbar" toggleable="lg" type="dark">
      <font-awesome-icon
        :icon="['fas', 'satellite-dish']"
        size="2x"
        color="orange"
        class="darksky-navbar-logoicon"
      />
      <b-navbar-brand href="#">
        <span class="darksky-navbar-logotext">DarkSky</span>
      </b-navbar-brand>
      <b-navbar-nav>
        <b-nav-item href="#">Manual Tracking</b-nav-item>
        <b-nav-item href="#">Align To Target</b-nav-item>
      </b-navbar-nav>

      <b-dropdown dropup text="Comm Port" class="darksky-menu">
        <b-dropdown-item
          v-for="comport in this.state.comports"
          v-bind:key="comport.device"
          @click="selectComport(comport)"
        >{{ comport.device }}</b-dropdown-item>
      </b-dropdown>

      <b-button variant="outline-danger" class="darksky-menu" v-if="connected==false">
        <font-awesome-icon
          :icon="['fas', 'plug']"
          size="lg"
          color="orange"
          @click="serialConnect()"
        />
      </b-button>
      <b-button variant="outline-success" class="darksky-menu" v-if="connected==true">
        <font-awesome-icon
          :icon="['fas', 'link']"
          size="lg"
          color="grey"
          @click="serialDisconnect()"
        />
      </b-button>

      <b-dropdown dropup text="LNB Power" class="darksky-menu">
        <b-dropdown-item href="#">Off</b-dropdown-item>
        <b-dropdown-item href="#">Auto</b-dropdown-item>
        <b-dropdown-divider />
        <b-dropdown-item href="#">Mode 0 : +13 vdc @ 0 Hz</b-dropdown-item>
        <b-dropdown-item href="#">Mode 1 : +13 vdc @ 22 KHz</b-dropdown-item>
        <b-dropdown-item href="#">Mode 2 : +18 vdc @ 22 KHz</b-dropdown-item>
        <b-dropdown-item href="#">Mode 3 : +18 vdc @ 0 KHz</b-dropdown-item>
      </b-dropdown>
      <b-button variant="outline-danger" class="darksky-menu">
        <font-awesome-icon :icon="['fas', 'power-off']" size="lg" color="gray" />
      </b-button>

      <!-- Right aligned nav items -->
      <b-navbar-nav class="ml-auto">
        <b-nav-form>
          <b-form-input size="sm" class="mr-sm-2" placeholder="Search"></b-form-input>
          <b-button size="sm" class="my-2 my-sm-0" type="submit">Search</b-button>
        </b-nav-form>
      </b-navbar-nav>
    </b-navbar>
  </div>
</template>

<script>
console.log("Creating NavMenu...");

import { mapState } from "vuex";

import { library } from "@fortawesome/fontawesome-svg-core";
import {
  faSatelliteDish,
  faPowerOff,
  faPlug,
  faLink
} from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";

library.add(faSatelliteDish);
library.add(faPowerOff);
library.add(faPlug);
library.add(faLink);

export default {
  name: "NavMenu",
  data() {
    return {
      currentComport: undefined,
      connected: false
    };
  },
  components: { FontAwesomeIcon },
  computed: mapState({
    state: state => state
  }),
  methods: {
    selectComport(comport) {
      this.$store.commit("selectComport", comport);
      this.currentComport = comport;
    },
    serialConnect() {
      if (this.currentComport != undefined) {
        console.log(`Connecting to comport ${this.currentComport.device}`);
      }
      else
      {
        console.warn('No comport selected!');
      }
    },
    serialDisconnect() {
      if (this.currentComport != undefined) {
        console.log(`Disconnecting from comport ${this.currentComport.device}`);
      }
    }
  }
};
</script>


<style scoped>
.darksky-navbar {
  background-color: #404040;
  height: 64px;
}

.darksky-navbar-logoicon {
  font-size: 38px;
}

.darksky-navbar-logotext {
  margin-bottom: 8px;
  padding: 6px;
}

.darksky-menu {
  margin-left: 5px;
  margin-right: 5px;
}
</style>