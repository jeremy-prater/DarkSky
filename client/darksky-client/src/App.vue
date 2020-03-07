<template>
  <div id="app">
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
    this.$store.commit("loadProject");
  }
};
</script>

<style>
#app {
  font-family: "Avenir", Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
}
</style>
