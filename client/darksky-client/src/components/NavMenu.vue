<template>
  <div>
    <Navbar bg="dark" expand="lg" class="navbar-dark StatusBar">
      <FontAwesomeIcon icon="{faSatelliteDish}" size="2x" color="orange" class="LogoIcon" />
      <Navbar class="Brand LogoText" href="#home">
        <h2>DarkSky</h2>
      </Navbar>
      <Navbar class="Toggle" aria-controls="basic-navbar-nav" />
      <Navbar class="Collapse" id="basic-navbar-nav">
        <Nav class="mr-auto">
          <Nav class="Link" href="#manual">Manual Tracking</Nav>
          <Nav class="Link" href="#target">Align to target</Nav>

          <ComportMenu state="{this.state}" />

          <Button variant="outline-danger">
            <FontAwesomeIcon icon="{faPlug}" size="lg" color="orange" />
          </Button>

          <NavDropdown title="LNB Power" id="lnb_power">
            <NavDropdown class="Item" href="#lnb/off">Off</NavDropdown>
            <NavDropdown class="Item" href="#lnb/auto">Auto</NavDropdown>
            <NavDropdown class="Divider" />
            <NavDropdown class="Item" href="#lnb/0">Mode 0 : +13 vdc @ 0 Hz</NavDropdown>
            <NavDropdown class="Item" href="#lnb/1">Mode 1 : +13 vdc @ 22 KHz</NavDropdown>
            <NavDropdown class="Item" href="#lnb/2">Mode 2 : +18 vdc @ 22 KHz</NavDropdown>
            <NavDropdown class="Item" href="#lnb/3">Mode 3 : +18 vdc @ 0 KHz</NavDropdown>
          </NavDropdown>
          <Button variant="outline-danger">
            <FontAwesomeIcon icon="{faPowerOff}" size="lg" color="gray" />
          </Button>
        </Nav>
        <Form inline>
          <FormControl type="text" placeholder="Search" class="mr-sm-2" />
          <Button variant="outline-success">Search</Button>
        </Form>
      </Navbar>
    </Navbar>
  </div>
</template>

<script>
console.log("Creating NavMenu...");

import { mapState } from "vuex";

export default {
  name: "NavMenu",
  data() {
    return {
    };
  },
  //components: {  },
  computed: mapState({
    project: state => state.project,
    currentWorkspace: state => state.currentWorkspace
  }),
  methods: {
  }
};
</script>


<style scoped>
</style>