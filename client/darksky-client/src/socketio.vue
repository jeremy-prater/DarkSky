<template>
  <div />
</template>

<script>
import { mapState } from "vuex";
import socketIOClient from "socket.io-client";
import { EventBus } from "./eventbus.js";

export default {
  name: "DarkSkySocketIO",
  data: function() {
    return {};
  },
  components: {},
  computed: mapState({
    state: state => state
  }),
  mounted: function() {
    console.log(`Connecting to ${this.state.endpoint}`);
    this.socket = socketIOClient(this.state.endpoint);

    this.socket.on("connect", () => {
      console.log("Client Connected!");
    });

    this.socket.on("connect_error", error => {
      console.error(`Failed to connect to ${this.state.endpoint} ${error}`);
    });

    this.socket.on("comport.update", comports => {
      this.$store.commit("setComports", comports);
    });

    this.socket.on("comport.status", comstatus => {
      console.log(`comport.status(${JSON.stringify(comstatus)})`);
    });

    EventBus.$on("serialPortConnect", comport =>
      this.serialPortConnect(comport)
    );
  },
  methods: {
    serialPortConnect(comport) {
      console.log(`serialPortConnect(${JSON.stringify(comport)})`);
    }
  }
};
</script>

<style scoped>
</style>