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
body {
  padding: 0px;
  margin: 0px;
  border: 0px;
  height: 100%;
  overflow: hidden;
}
</style>
