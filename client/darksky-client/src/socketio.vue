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

    // Events from MCP
    this.socket.on("signal.boot", value => {
      this.$store.commit("signalBoot", value);
    });
    this.socket.on("signal.motor.dec.position", value => {
      this.$store.commit("signalMotorDecPosition", value);
    });
    this.socket.on("signal.motor.dec.state", value => {
      this.$store.commit("signalMotorDecState", value);
    });
    this.socket.on("signal.motor.ra.position", value => {
      this.$store.commit("signalMotorRaPosition", value);
    });
    this.socket.on("signal.motor.ra.state", value => {
      this.$store.commit("signalMotorRaState", value);
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