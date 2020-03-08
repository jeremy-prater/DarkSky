<template>
  <div id="app" class="app">
    <DarkSkySocketIO ref="darkskysocketio" />
    <NavMenu />
  </div>
</template>

<script>
//import NavBar from "./components/NavBar.vue";
import NavMenu from "./components/NavMenu";
import { mapState } from "vuex";
import DarkSkySocketIO from "./socketio";

export default {
  name: "app",
  components: {
    DarkSkySocketIO,
    NavMenu
  },
  computed: mapState({
    state: state => state
  }),
  mounted: function() {
    console.log("Created App");

    //
    // Application entry point for state setup
    //
    //this.$store.commit("init-stuff");
  }
};
</script>

<style scoped>
.app {
  text-align: center;
  background-color: #101010;
  font-family: "Courier New", Courier, monospace;
}
</style>
