<template>
  <div class="overlaypanel status-panel">
    <!-- <b-nav class="status fixed-bottom status-light bg-light">
      <b-nav-item>Fixed bottom</b-nav-item>
    </b-nav>-->
    <div class="overlaypanel-title">
      <font-awesome-icon
        :icon="['fas', 'satellite-dish']"
        size="lg"
        color="orange"
        class="statusicon"
      />System Status
    </div>
  </div>
</template>

<script>
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
  name: "Status",
  data() {
    return {};
  },
  components: {
    FontAwesomeIcon
  },
  computed: mapState({
    state: state => state
  }),
  mounted() {
    console.log("Created Status panel...");

    this.sockets.listener.subscribe("connect", () => {
      this.$store.commit("setMotorConnection", true);
    });

    this.sockets.listener.subscribe("disconnect", () => {
      this.$store.commit("setMotorConnection", false);
    });

    this.sockets.listener.subscribe("updateState", data => {
      try {
        if (data.magic !== 0xdadaf00d) {
          throw `Incorrect state header ${data.magic}`;
        }

        if (data.checksum !== 0) {
          throw `Incorrect checksum ${data.checksum}`;
        }

        switch (data.command) {
          // MOTOR_DEC_STATE = 0x0101,
          // MOTOR_DEC_POSITION = 0x0102,
          // MOTOR_DEC_STOP_POS = 0x0103,
          // MOTOR_RA_STATE = 0x0201,
          // MOTOR_RA_POSITION = 0x0202,
          // MOTOR_RA_STOP_POS = 0x0203,
          // LNB_STATE = 0x0301,
          // STOP_ALL_MOTORS = 0x1001,
          // ERROR = 0x2000,
          // REBOOT = 0xFFFF
          case 0x0101: // MOTOR_DEC_STATE
            if (this.state.decMotor.state != data.arg1) {
              this.$store.commit("updateDecMotorState", data.arg1);
            }
            break;
          case 0x0102: // MOTOR_DEC_POSITION
            if (this.state.decMotor.position != data.arg1) {
              this.$store.commit("updateDecMotorPosition", data.arg1);
            }
            break;
          case 0x0103: // MOTOR_DEC_STOP_POS
            if (this.state.decMotor.stopPos != data.arg1) {
              this.$store.commit("updateDecMotorStopPos", data.arg1);
            }
            break;

          case 0x0201: // MOTOR_RA_STATE
            if (this.state.raMotor.state != data.arg1) {
              this.$store.commit("updateRAMotorState", data.arg1);
            }
            break;
          case 0x0202: // MOTOR_RA_POSITION
            if (this.state.raMotor.position != data.arg1) {
              this.$store.commit("updateRAMotorPosition", data.arg1);
            }
            break;
          case 0x0203: // MOTOR_RA_STOP_POS
            if (this.state.raMotor.stopPos != data.arg1) {
              this.$store.commit("updateRAMotorStopPos", data.arg1);
            }
            break;

          case 0x0301: // LNB_STATE
            if (this.state.lnbState.voltage != data.arg1) {
              this.$store.commit("updateLNBVoltage", data.arg1);
            }
            if (this.state.lnbState.carrier != data.arg2) {
              this.$store.commit("updateLNBCarrier", data.arg2);
            }
            break;

          case 0x1001: // Stop All Motors
            if (this.state.allMotorStop != data.arg1) {
              this.$store.commit("updateAllMotorStop", data.arg1);
            }
            break;

          case 0x2000: // Error
            break;

          case 0xffff: // Reboot
            break;
        }

        // this.$store.commit("updateState", data);
      } catch (ex) {
        console.error(ex);
      }
    });
  },
  methods: {}
};
</script>

<style scoped>
.status-panel {
  bottom: 79vh;
  left: 0.5vw;
  height: 20vh;
  width: 20vw;
}
</style>
