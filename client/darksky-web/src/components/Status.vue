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
