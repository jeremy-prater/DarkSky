<template>
  <div id="app">
    <Scanhome />
    <Status />
  </div>
</template>

<script>
import Scanhome from "./components/Scanhome.vue";
import Status from "./components/Status.vue";

export default {
  name: "App",
  components: {
    Scanhome,
    Status
  }
};
</script>

<style>
@import url(https://cdn.jsdelivr.net/gh/tonsky/FiraCode@4/distr/fira_code.css);

code {
  font-family: "Fira Code", monospace;
}

@supports (font-variation-settings: normal) {
  code {
    font-family: "Fira Code VF", monospace;
  }
}

body {
  padding: 0px;
  margin: 0px;
  border: 0px;
  height: 100vh;
  overflow: hidden;
}
</style>
