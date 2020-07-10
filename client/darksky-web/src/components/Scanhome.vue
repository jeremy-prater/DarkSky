<template>
  <div>
    <div class="home" ref="scanhome3dparent" @resize="resize">
      <canvas ref="scanhome3d" class="home" v-observe-visibility="visibilityChanged" />
    </div>
    <div class="overlaypanel mappanel">
      <div class="overlaypanel-title">
        <font-awesome-icon :icon="['fa', 'map-marked-alt']" size="lg" class="statusicon" />Map overlay
      </div>
      <div class="overlaypanel-item maplist">
        <ul>
          <li>Lat : {{ state.gps.lat }}</li>
          <li>Lng : {{ state.gps.lon }}</li>
          <li>Time : {{ state.gps.time }}</li>
        </ul>
      </div>

      <b-button>Button</b-button>
      <v-color-picker v-model="color"></v-color-picker>
    </div>
  </div>
</template>

<script>
import * as THREE from "three";
import { TrackballControls } from "three/examples/jsm/controls/TrackballControls.js";
import { mapState } from "vuex";
import { library } from "@fortawesome/fontawesome-svg-core";
import { faMapMarkedAlt } from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";

library.add(faMapMarkedAlt);

export default {
  name: "Scanhome",
  data() {
    return {
      size: {
        width: 0,
        height: 0
      },
      mapColors: {
        grid: {
          types: ["hex", "hexa", "rgba", "hsla", "hsva"],
          type: "hex",
          hex: "#FF00FF",
          hexa: "#FF00FFFF",
          rgba: { r: 255, g: 0, b: 255, a: 1 },
          hsla: { h: 300, s: 1, l: 0.5, a: 1 },
          hsva: { h: 300, s: 1, v: 1, a: 1 }
        }
      },
      geometry: null,
      // texture_grid: null,
      // texture_figures: null,
      // texture_boundaries: null,
      // texture_starmap: null,
      loaded: false,
      render: {},
      objects: {},
      viewVector: {
        zoom: 0,
        dragging: false,
        dx: 0,
        dy: 0,
        x: 0,
        y: 0,
        z: 0
      }
    };
  },
  components: {
    FontAwesomeIcon
  },
  computed: {
    ...mapState({
      state: state => state
    }),
    color: {
      get() {
        return this[this.mapColors.grid];
      },
      set(v) {
        this[this.mapColors.grid] = v;
      }
    }
  },
  mounted() {
    console.log("Created Scanhome...");
    window.addEventListener(
      "resize",
      function(event) {
        this.resize(event);
        this.render.trackball.handleResize();
      }.bind(this)
    );

    setInterval(this.tick, 1000);
  },
  methods: {
    tick() {
      if (this.state.gps.mode >= 2) {
        // Roll
        this.objects["horizonMesh"].rotation.x = this.state.sky.ra;
        // Pitch
        this.objects["horizonMesh"].rotation.z = this.state.sky.dec;
      }
    },
    visibilityChanged(isVisible) {
      if (isVisible === true) {
        this.load();
      } else {
        this.unload();
      }
    },
    resize() {
      this.visibilityChanged(false);
      this.visibilityChanged(true);
    },
    doRender() {
      if (this.loaded == false || this.render == undefined) {
        return;
      }

      this.render.trackball.update();
      this.render.renderer.render(this.render.scene, this.render.camera);

      requestAnimationFrame(this.doRender);
    },
    load() {
      if (this.loaded === false) {
        this.loaded = true;
      } else {
        return;
      }

      let domObject = this.$refs["scanhome3dparent"];
      this.render.width = domObject.clientWidth;
      this.render.height = domObject.clientHeight;

      console.info(`Creating darksky scene`);

      this.render.scene = new THREE.Scene();
      this.render.camera = new THREE.PerspectiveCamera(
        60,
        this.render.width / this.render.height,
        0.0001,
        10
      );

      this.render.camera.position.x = 1;

      this.render.trackball = new TrackballControls(
        this.render.camera,
        domObject
      );

      this.render.trackball.rotateSpeed = 5;
      this.render.trackball.minDistance = 0.1;
      this.render.trackball.maxDistance = 2;

      this.render.renderer = new THREE.WebGLRenderer({
        canvas: this.$refs["scanhome3d"]
      });

      this.render.renderer.setSize(this.render.width, this.render.height);

      let sliceSize = 0.015;
      let numSlices = 128;
      for (let dec = -90; dec < 90; dec += 15) {
        let position = dec / 90;
        let radius = Math.sqrt(1 - position * position);

        if (radius === 0) continue;

        let geometry = new THREE.CylinderGeometry(
          radius,
          radius,
          sliceSize,
          numSlices,
          1,
          true
        );
        let material = new THREE.MeshBasicMaterial({
          color: 0x3030ff,
          side: THREE.BackSide
        });
        let mesh = new THREE.Mesh(geometry, material);
        mesh.position.y = position;
        this.render.scene.add(mesh);
      }

      for (let lon = 0.0; lon < 180; lon += 15) {
        let radius = 1;
        let geometryn = new THREE.CylinderGeometry(
          radius,
          radius,
          sliceSize,
          numSlices,
          1,
          true
        );
        let materialn = new THREE.MeshBasicMaterial({
          color: 0xffff00,
          side: THREE.BackSide
        });
        let meshn = new THREE.Mesh(geometryn, materialn);

        meshn.rotation.x = Math.PI / 2;
        meshn.rotation.z = Math.PI * (lon / 180);
        this.render.scene.add(meshn);
      }

      // Generate horizon
      let horizonRadius = 1.01;
      // let horizonGeometry = new THREE.CylinderGeometry(
      //   horizonRadius,
      //   horizonRadius,
      //   sliceSize,
      //   numSlices,
      //   1,
      //   true
      // );
      let horizonGeometry = new THREE.SphereGeometry(
        horizonRadius,
        numSlices,
        numSlices,
        0,
        Math.PI * 2,
        Math.PI / 2,
        Math.PI
      );

      let horizonMaterial = new THREE.MeshBasicMaterial({
        color: 0x803080,
        side: THREE.BackSide
      });
      this.objects["horizonMesh"] = new THREE.Mesh(
        horizonGeometry,
        horizonMaterial
      );
      this.objects["horizonMesh"].rotation.x = Math.PI;
      this.render.scene.add(this.objects["horizonMesh"]);

      // Generate scene
      requestAnimationFrame(this.doRender);
      console.log("Renderer started!");
    },
    unload() {
      this.loaded = false;
      this.render = {};
      console.info(`Destroying darksky scene`);
    }
  }
};
</script>

<style scoped>
.home {
  width: 100vw;
  height: 100vh;
  background-color: #303050;
}

.mappanel {
  top: 0.5vh;
  right: 0.5vw;
  padding: 0.5vw;
}

.maplist {
  padding: 0px;
}
</style>
