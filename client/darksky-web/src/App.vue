<template>
  <div id="app">
    <ScanHome />
  </div>
</template>

<script>
import ScanHome from "./components/Scanhome.vue";

export default {
  name: "App",
  components: {
    ScanHome
  }
};
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}
</style>
