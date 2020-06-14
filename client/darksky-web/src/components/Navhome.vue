<template>
  <div class="navbar">
    <b-nav class="navbar fixed-bottom navbar-light bg-light">
      <b-nav-item>Fixed bottom</b-nav-item>
    </b-nav>
  </div>
</template>

<script>
import { mapState } from "vuex";

export default {
  name: "Navhome",
  data() {
    return {};
  },
  components: {
    //FontAwesomeIcon
  },
  computed: mapState({
    state: state => state
  }),
  mounted() {
    console.log("Created Navhome...");
  },
  methods: {}
};
</script>

<style scoped>
navbar {
}
</style>
