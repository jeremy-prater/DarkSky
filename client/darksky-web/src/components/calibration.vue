<template>
    <modal
      v-show="state.actual.calibrating"
      @close="cancelCalibration"
      style="max-height: 70vh;"
    >
      <template v-slot:title>Motor Calibration</template>
      <template v-slot:body>
        <div style="padding: 20px;">
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
          style="margin:10px;"
        >
          Az+
        </button>
        <button
          type="button"
          class="btn btn-primary"
          @click="calibrationMove('az', 'right')"
          style="margin:10px;"
        >
          Az-
        </button>
        <button
          type="button"
          class="btn btn-primary"
          @click="calibrationMove('alt', 'up')"
          style="margin:10px;"
        >
          Alt+
        </button>
        <button
          type="button"
          class="btn btn-primary"
          @click="calibrationMove('alt', 'down')"
          style="margin:10px;"
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
  components: { Modal, VueSlider },
  computed: {
    ...mapState({
      state: state => state
    })
  },
  mounted() {
    console.log("Created Calibration Modal...");
  },
  methods: {
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
        if (direction === "left") {
          this.calibration.offsetAz += this.calibration.step;
        } else if (direction === "right") {
          this.calibration.offsetAz -= this.calibration.step;
        }
      } else if (axis === "alt") {
        if (direction === "up") {
          this.calibration.offsetAlt += this.calibration.step;
        } else if (direction === "down") {
          this.calibration.offsetAlt -= this.calibration.step;
        }
      }
    },
    applyCalibration() {
      this.$store.commit("requestCalibration", false);
      console.log("Applying Calibration!");
    },
    cancelCalibration() {
      this.$store.commit("requestCalibration", false);
      console.log("Calibration Cancelled...");
    }
  }
};
</script>