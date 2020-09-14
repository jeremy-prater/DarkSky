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
        v-if="state.motorServerConnected"
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
        v-if="state.motorServerConnected && (state.gps.mode >= 2)"
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
        <li>Az : {{ common.deg2dms(state.dish.az) }}</li>
        <li>Alt : {{ common.deg2dms(state.dish.alt) }}</li>
        <li>RA : {{ common.deg2hms(dish.ra) }}</li>
        <li>Dec : {{ common.deg2dms(dish.dec) }}</li>
        <li>Strength : {{ state.lnb.strength.toFixed(5) }}</li>
      </ul>
      <b-button @click="openCalibrate">Calibrate</b-button>
    </div>
    <modal v-show="calibrating" @close="cancelCalibration" style="max-height: 70vh;">
      <template v-slot:title>Motor Calibration</template>
      <template v-slot:body>
        <button type="button" class="btn btn-primary" style="margin:10px;">Az+</button>
        <button type="button" class="btn btn-primary" style="margin:10px;">Az-</button>
        <button type="button" class="btn btn-primary" style="margin:10px;">Alt+</button>
        <button type="button" class="btn btn-primary" style="margin:10px;">Alt-</button>

        <div class="container">
           OffsetAz = 0
           OffsetAlt = 0
        </div>
      </template>
      <template v-slot:footer>
        <button type="button" class="btn btn-secondary" @click="cancelCalibration">Cancel</button>
        <button type="button" class="btn btn-primary" @click="applyCalibration">Apply Calibration</button>
      </template>
    </modal>
  </div>
</template>

<script>
import { mapState } from "vuex";
import { library } from "@fortawesome/fontawesome-svg-core";
import { faSatelliteDish, faCompass } from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";
import common from "./common";
import Modal from "./Modal";

library.add(faSatelliteDish);
library.add(faCompass);

export default {
  name: "Status",
  data() {
    return {
      calibrating: false,
      common: common,
      dish: {
        ra: 0,
        dec: 0
      }
    };
  },
  components: { Modal, FontAwesomeIcon },
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
        az: this.state.dish.az - 180,
        alt: this.state.dish.alt
      });
      this.dish.ra = radec.ra;
      this.dish.dec = radec.dec;
    },
    openCalibrate() {
      this.calibrating = true;
      console.log("Open Calibration : " + this.calibrating);
    },
    applyCalibration() {
      console.log("Applying Calibration!");
    },
    cancelCalibration() {
      this.calibrating = false;
      console.log("Calibration Cancelled...");
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
