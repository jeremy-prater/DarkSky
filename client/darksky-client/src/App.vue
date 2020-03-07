<template>
  <div id="app" class="app">
    <NavMenu />
  </div>
</template>

<script>
//import NavBar from "./components/NavBar.vue";
import NavMenu from "./components/NavMenu.vue";
import { mapState } from "vuex";
import socketIOClient from "socket.io-client";

export default {
  name: "app",
  components: {
    //NavBar,
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
    console.log(`Connecting to ${this.state.endpoint}`);
    const socket = socketIOClient(this.state.endpoint);

    socket.on('connect', () => {
      console.log('Client Connected!');
    });

    socket.on('connect_error', (error) => {
      console.error(`Failed to connect to ${this.state.endpoint} ${error}`);
    });

    socket.on('comport.update', comports => {
      this.$store.commit("setComports", comports);
    });

    socket.on('comport.status', comstatus => {
      console.log(`[comport.status]`);
      console.log(comstatus);
    });
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
