<template>
  <div>
    <b-navbar toggleable="lg" type="dark" variant="info">
      <b-navbar-brand href="#">
        <FontAwesomeIcon icon="{faSatelliteDish}" size="2x" color="orange" class="LogoIcon" />DarkSky
      </b-navbar-brand>

      <b-navbar-toggle target="nav-collapse"></b-navbar-toggle>

      <b-collapse id="nav-collapse" is-nav>
        <b-navbar-nav>
          <b-nav-item href="#">Manual Tracking</b-nav-item>
          <b-nav-item href="#">Align To Target</b-nav-item>
        </b-navbar-nav>

        <b-nav-item-dropdown text="Comm Port">
          <b-dropdown-item href="#">/dev/tty1</b-dropdown-item>
          <b-dropdown-item href="#">/something/else/v2</b-dropdown-item>
        </b-nav-item-dropdown>

        <b-button variant="outline-danger">
          <FontAwesomeIcon icon="{faPlug}" size="lg" color="orange" />
        </b-button>

        <b-nav-item-dropdown text="LNB Power">
          <b-dropdown-item href="#">Off</b-dropdown-item>
          <b-dropdown-item href="#">Auto</b-dropdown-item>
          <b-dropdown-item href="#">Mode 0 : +13 vdc @ 0 Hz</b-dropdown-item>
          <b-dropdown-item href="#">Mode 1 : +13 vdc @ 22 KHz</b-dropdown-item>
          <b-dropdown-item href="#">Mode 2 : +18 vdc @ 22 KHz</b-dropdown-item>
          <b-dropdown-item href="#">Mode 3 : +18 vdc @ 0 KHz</b-dropdown-item>
        </b-nav-item-dropdown>
        <b-button variant="outline-danger">
          <FontAwesomeIcon icon="{faPowerOff}" size="lg" color="gray" />
        </b-button>
      </b-collapse>

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

export default {
  name: "NavMenu",
  data() {
    return {};
  },
  //components: {  },
  computed: mapState({
    project: state => state.project,
    currentWorkspace: state => state.currentWorkspace
  }),
  methods: {}
};
</script>


<style scoped>
</style>