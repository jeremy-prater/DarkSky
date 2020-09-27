<template>
  <div class="overlaypanel status-panel">
    <!-- <b-nav class="status fixed-bottom status-light bg-light">
      <b-nav-item>Fixed bottom</b-nav-item>
    </b-nav>-->
    <div class="overlaypanel-title">
      <font-awesome-icon
        :icon="['fas', 'satellite-dish']"
        size="lg"
        color="green"
        class="statusicon"
        v-if="state.actual.motorServerConnected"
      />
      <font-awesome-icon
        :icon="['fas', 'satellite-dish']"
        size="lg"
        color="orange"
        class="statusicon"
        v-else
      />

      <font-awesome-icon
        :icon="['fas', 'compass']"
        size="lg"
        color="green"
        class="statusicon"
        v-if="state.actual.motorServerConnected && state.actual.gps.mode >= 2"
      />
      <font-awesome-icon
        :icon="['fas', 'compass']"
        size="lg"
        color="orange"
        class="statusicon"
        v-else
      />System Status
    </div>
    <div class="overlaypanel-text overlaypanel-item maplist">
      <ul>
        <li>Az : {{ common.deg2dms(state.actual.dish.az) }}</li>
        <li>Alt : {{ common.deg2dms(state.actual.dish.alt) }}</li>
        <li>RA : {{ common.deg2hms(dish.ra) }}</li>
        <li>Dec : {{ common.deg2dms(dish.dec) }}</li>
        <li>Strength : {{ state.actual.lnb.strength.toFixed(5) }}</li>
      </ul>
      <b-button @click="openCalibrate">Calibrate</b-button>
    </div>
    <Calibration />
  </div>
</template>

<script>
import { mapState } from "vuex";
import { library } from "@fortawesome/fontawesome-svg-core";
import { faSatelliteDish, faCompass } from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";
import common from "./common";
import Calibration from "./calibration";

library.add(faSatelliteDish);
library.add(faCompass);

export default {
  name: "Status",
  data() {
    return {
      common: common,
      dish: {
        ra: 0,
        dec: 0
      },
      calibration: {
        step: 100,
        offsetAz: 0,
        offsetAlt: 0,
        marks: {
          "1": "1",
          "10": "10",
          "100": "100",
          "500": "500",
          "1000": "1000",
          "2000": "2000",
          "5000": "5000",
          "10000": "10000"
        }
      }
    };
  },
  components: { FontAwesomeIcon, Calibration },
  computed: {
    ...mapState({
      state: state => state
    })
  },
  mounted() {
    console.log("Created Status panel...");

    this.sockets.listener.subscribe("connect", () => {
      this.$store.commit("setMotorConnection", true);
    });

    this.sockets.listener.subscribe("disconnect", () => {
      this.$store.commit("setMotorConnection", false);
    });

    this.sockets.listener.subscribe("updateState", data => {
      this.$store.commit("updateState", data);
    });

    setInterval(this.tick, 1000);
  },
  methods: {
    tick() {
      const radec = common.convertAzAlt2RADec(this.state, {
        az: this.state.actual.dish.az - 180,
        alt: this.state.actual.dish.alt
      });
      this.dish.ra = radec.ra;
      this.dish.dec = radec.dec;
    },
    openCalibrate() {
      this.$store.commit("requestCalibration", true);
      console.log("Open Calibration...");
    }
  }
};
</script>

<style scoped>
.status-panel {
  left: 3.5vw;
  top: 0.5vh;
  width: 20vw;
}
</style>
