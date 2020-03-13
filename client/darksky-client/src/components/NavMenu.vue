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
        <b-dropdown dropup text="DarkSky" variant="outline-primary" class="darksky-menu">
          <b-dropdown-item href="#">Manual Tracking</b-dropdown-item>
          <b-dropdown-item href="#">Align To Target</b-dropdown-item>
        </b-dropdown>
      </b-navbar-brand>
      <b-navbar-nav></b-navbar-nav>

      <b-dropdown dropup text="Comport" variant="outline-secondary" class="darksky-menu">
        <b-dropdown-item
          v-for="comport in this.state.comports"
          v-bind:key="comport.device"
          @click="selectComport(comport)"
        >{{ comport.device }}</b-dropdown-item>

        <span class="menuStatusLeft">{{ this.comportName() }}</span>
      </b-dropdown>

      <b-dropdown dropup text="LNB Power" variant="outline-secondary" class="darksky-menu">
        <b-dropdown-item href="#">Off</b-dropdown-item>
        <b-dropdown-item href="#">Auto</b-dropdown-item>
        <b-dropdown-divider />
        <b-dropdown-item href="#">Mode 0 : +13 vdc @ 0 Hz</b-dropdown-item>
        <b-dropdown-item href="#">Mode 1 : +13 vdc @ 22 KHz</b-dropdown-item>
        <b-dropdown-item href="#">Mode 2 : +18 vdc @ 22 KHz</b-dropdown-item>
        <b-dropdown-item href="#">Mode 3 : +18 vdc @ 0 KHz</b-dropdown-item>
        <b-dropdown-divider />
      </b-dropdown>

      <b-button variant="outline-danger" class="darksky-menu" v-if="this.state.comportState==false">
        <font-awesome-icon
          :icon="['fas', 'plug']"
          size="lg"
          color="orange"
          @click="serialConnect()"
        />
      </b-button>
      <b-button variant="outline-success" class="darksky-menu" v-if="this.state.comportState==true">
        <font-awesome-icon
          :icon="['fas', 'link']"
          size="lg"
          color="grey"
          @click="serialDisconnect()"
        />
      </b-button>

      <b-button variant="outline-danger" class="darksky-menu">
        <font-awesome-icon :icon="['fas', 'power-off']" size="lg" color="gray" />
      </b-button>

      <b-button variant="outline-danger" class="darksky-menu">
        <font-awesome-icon :icon="['fas', 'stop-circle']" size="lg" color="gray" />
      </b-button>

      <b-button variant="outline-primary" class="darksky-menu">
        <font-awesome-icon :icon="['fas', 'chevron-circle-up']" size="lg" color="gray" />
      </b-button>
      <b-button variant="outline-primary" class="darksky-menu">
        <font-awesome-icon :icon="['fas', 'chevron-circle-down']" size="lg" color="gray" />
      </b-button>

      <b-button variant="outline-primary" class="darksky-menu">
        <font-awesome-icon :icon="['fas', 'chevron-circle-left']" size="lg" color="gray" />
      </b-button>
      <b-button variant="outline-primary" class="darksky-menu">
        <font-awesome-icon :icon="['fas', 'chevron-circle-right']" size="lg" color="gray" />
      </b-button>



      <!-- Right aligned nav items -->
      <b-navbar-nav class="ml-auto">
        <b-nav-form>
          <b-form-input size="sm" class="mr-sm-2" placeholder="Search"></b-form-input>
          <b-button size="sm" class="my-2 my-sm-0" type="submit">Search</b-button>
        </b-nav-form>
      </b-navbar-nav>
    </b-navbar>
    <router-view></router-view>
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
  faLink,
  faStopCircle,
  faChevronCircleUp,
  faChevronCircleDown,
  faChevronCircleLeft,
  faChevronCircleRight
} from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";
import { EventBus } from "../eventbus.js";

library.add(faSatelliteDish);
library.add(faPowerOff);
library.add(faPlug);
library.add(faLink);
library.add(faStopCircle);
library.add(faLink);
library.add(faStopCircle);
library.add(faChevronCircleUp);
library.add(faChevronCircleDown);
library.add(faChevronCircleLeft);
library.add(faChevronCircleRight);

export default {
  name: "NavMenu",
  data() {
    return {};
  },
  components: { FontAwesomeIcon },
  computed: mapState({
    state: state => state
  }),
  methods: {
    comportName() {
      if (this.state.currentComport === undefined) {
        return "Com Port";
      }
      let comNameArray = this.state.currentComport.device.split("/");
      let comName = comNameArray[comNameArray.length - 1];
      return comName.substr(0, 15);
    },
    selectComport(comport) {
      this.$store.commit("selectComport", comport);
      this.currentComport = comport;
    },
    serialConnect() {
      if (this.currentComport != undefined) {
        EventBus.$emit("comport.connect", this.currentComport);
      } else {
        console.warn("No comport selected!");
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
