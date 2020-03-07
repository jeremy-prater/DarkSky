<template>
  <div id="app" class="app">
    <NavMenu />
  </div>
</template>

<script>
//import NavBar from "./components/NavBar.vue";
import NavMenu from "./components/NavMenu.vue";

export default {
  name: "app",
  components: {
    //NavBar,
    NavMenu
  },
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
