<template>
  <div id="app">
    <Scanhome />
    <Navhome />
  </div>
</template>

<script>
import Scanhome from "./components/Scanhome.vue";
import Navhome from "./components/Navhome.vue";

export default {
  name: "App",
  components: {
    Scanhome,
    Navhome
  }
};
</script>

<style>
body {
  padding: 0px;
  margin: 0px;
  border: 0px;
  height: 100vh;
  overflow: hidden;
}
</style>
