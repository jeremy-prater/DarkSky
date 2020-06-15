<template>
  <div class="darksky-status">
    <!-- <b-nav class="status fixed-bottom status-light bg-light">
      <b-nav-item>Fixed bottom</b-nav-item>
    </b-nav>-->
    <div class="darksky-status-text">
      <font-awesome-icon :icon="['fas', 'satellite-dish']" size="lg" color="orange" />
      System Status
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
  },
  methods: {}
};
</script>

<style scoped>
.darksky-status-text {
  color: cornflowerblue;
  text-align: center;
  font-family: "Fira Code";
  font-size: 1.5em;
  padding: 5%;
  width: 100%;
}

.darksky-status {
  background-color: rgba(20, 20, 20, 0.8);
  bottom: 79.0vh;
  left: 0.5vw;
  position: fixed;
  height: 20vh;
  width: 20vw;
  border-radius: 5%;
}
</style>
