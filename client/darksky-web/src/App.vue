<template>
  <v-main>
    <v-app>
      <Scanhome />
      <Status />
    </v-app>
  </v-main>
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

.overlaypanel-title {
  color: cornflowerblue;
  text-align: center;
  justify-content: center;
  font-family: "Fira Code";
  font-size: 1.5em;
  padding: 5%;
  display: flex;
  align-items: center;
}

.overlaypanel-text {
  color: cornflowerblue;
  font-family: "Fira Code";
  font-size: 0.8em;
  text-align: left;
}

.overlaypanel-item {
  padding: 5%;
}

.overlaypanel {
  background-color: rgba(50, 50, 80, 0.75);
  border-radius: 5%;
  position: fixed;
}

.statusicon {
  margin: 10px;
}
</style>
