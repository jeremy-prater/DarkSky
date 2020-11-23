<template>
  <modal
    v-show="state.image['calibrating']"
    @close="cancelCalibration"
    style="max-height: 70vh"
  >
    <template v-slot:title>Motor Calibration</template>
    <template v-slot:body>
      <div style="padding: 20px">
        <vue-slider
          v-model="calibration.step"
          :min="0"
          :max="10000"
          :interval="10"
          :marks="calibration.marks"
        />
      </div>
      <button
        type="button"
        class="btn btn-primary"
        @click="calibrationMove('az', 'left')"
        style="margin: 10px"
      >
        Az+
      </button>
      <button
        type="button"
        class="btn btn-primary"
        @click="calibrationMove('az', 'right')"
        style="margin: 10px"
      >
        Az-
      </button>
      <button
        type="button"
        :class="AzAutodriveClass()"
        @click="calibrationMove('az', 'autodrive')"
        style="margin: 10px"
      >
        Az Autodrive
      </button>
      <button
        type="button"
        class="btn btn-primary"
        @click="calibrationMove('alt', 'up')"
        style="margin: 10px"
      >
        Alt+
      </button>
      <button
        type="button"
        class="btn btn-primary"
        @click="calibrationMove('alt', 'down')"
        style="margin: 10px"
      >
        Alt-
      </button>

      <div class="container">
        OffsetAz : {{ calibration.offsetAz }} OffsetAlt :
        {{ calibration.offsetAlt }}
      </div>
    </template>
    <template v-slot:footer>
      <button
        type="button"
        class="btn btn-secondary"
        @click="cancelCalibration"
      >
        Cancel
      </button>
      <button type="button" class="btn btn-primary" @click="applyCalibration">
        Apply Calibration
      </button>
    </template>
  </modal>
</template>

<script>
import { mapState } from "vuex";
import Modal from "./Modal";
import VueSlider from "vue-slider-component";
import "vue-slider-component/theme/antd.css";

export default {
  name: "Calibration",
  data() {
    return {
      dish: {
        ra: 0,
        dec: 0,
      },
      calibration: {
        step: 100,
        offsetAz: 0,
        offsetAlt: 0,
        marks: {
          1: "1",
          10: "10",
          100: "100",
          500: "500",
          1000: "1000",
          2000: "2000",
          5000: "5000",
          10000: "10000",
        },
      },
    };
  },
  components: { Modal, VueSlider },
  computed: {
    ...mapState({
      state: (state) => state,
    }),
  },
  mounted() {
    console.log("Created Calibration Modal...");
  },
  methods: {
    AzAutodriveClass() {
      if (this.state.image["motors.az.autodrive"]) {
        return "btn btn-success";
      } else {
        return "btn btn-primary";
      }
    },
    calibrationMove(axis, direction) {
      console.log(
        "Calibration move : " +
          axis +
          " " +
          direction +
          " " +
          this.calibration.step +
          " units"
      );
      if (axis === "az") {
        this.$store.commit("requestControllerState", {
          state: "motors.az.delta",
          value: this.calibration.step,
        });
        if (direction === "left") {
          this.calibration.offsetAz += this.calibration.step;
          this.$store.commit("requestControllerState", {
            state: "motors.az.state",
            value: "forward",
          });
        } else if (direction === "right") {
          this.calibration.offsetAz -= this.calibration.step;
          this.$store.commit("requestControllerState", {
            state: "motors.az.state",
            value: "reverse",
          });
        } else if (direction === "autodrive") {
          this.$store.commit("requestControllerState", {
            state: "motors.az.autodrive",
            value: !this.state.image["motors.az.autodrive"],
          });
        }
      } else if (axis === "alt") {
        this.$store.commit("requestControllerState", {
          state: "motors.alt.delta",
          value: this.calibration.step,
        });
        if (direction === "up") {
          this.calibration.offsetAlt += this.calibration.step;
          this.$store.commit("requestControllerState", {
            state: "motors.alt.state",
            value: "forward",
          });
        } else if (direction === "down") {
          this.calibration.offsetAlt -= this.calibration.step;
          this.$store.commit("requestControllerState", {
            state: "motors.alt.state",
            value: "reverse",
          });
        }
      }
    },
    applyCalibration() {
      this.$store.commit("requestControllerState", {
        state: "calibrating",
        value: false,
      });
      console.log("Applying Calibration!");
    },
    cancelCalibration() {
      this.$store.commit("requestControllerState", {
        state: "calibrating",
        value: false,
      });
      console.log("Calibration Cancelled...");
    },
  },
};
</script>