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
  },
  methods: {}
};
</script>

<style scoped>
.status-panel {
  left: 0.5vw;
  top: 0.5vh;
  width: 20vw;
}
</style>
