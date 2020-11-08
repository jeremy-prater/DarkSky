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

    // Connection State
    this.socket.on("connect", () => {
      console.log("Client Connected!");
      
    });

    this.socket.on("connect_error", error => {
      console.error(`Failed to connect to ${this.state.endpoint} ${error}`);
    });

    // Comport Events
    this.socket.on("comport.update", comports => {
      this.$store.commit("setComports", comports);
    });

    this.socket.on("comport.status", comstatus => {
      this.$store.commit("comportStatus", comstatus);
    });

    EventBus.$on("comport.connect", comport => {
      console.log(`comport.connect(${JSON.stringify(comport)})`);
      this.socket.emit("comport.connect", comport);
    });

    // Commands to MCP
    EventBus.$on("motor.az.state", motorState => {
      console.log(`Setting RA Motor to : ${motorState}`);
      this.socket.emit("request.az.state", motorState);
    });
    EventBus.$on("motor.alt.state", motorState => {
      console.log(`Setting DEC Motor to : ${motorState}`);
      this.socket.emit("request.alt.state", motorState);
    });

    // Events from MCP
    this.socket.on("signal.boot", value => {
      this.$store.commit("signal.boot", value);
    });
    this.socket.on("signal.motor.alt.position", value => {
      this.$store.commit("signalMororAltPosition", value);
    });
    this.socket.on("signal.motor.alt.state", value => {
      this.$store.commit("signalMororAltState", value);
    });
    this.socket.on("signal.motor.az.position", value => {
      this.$store.commit("signalMororAzPosition", value);
    });
    this.socket.on("signal.motor.az.state", value => {
      this.$store.commit("signalMororAzState", value);
    });
    this.socket.on("signal.lnb.power", value => {
      this.$store.commit("signalLNBPower", value);
    });
  },
  methods: {}
};
</script>

<style scoped>
</style>