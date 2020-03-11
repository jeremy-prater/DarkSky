<template>
  <canvas
    ref="scanhome3d"
    class="home"
    v-observe-visibility="visibilityChanged"
    @mousedown="mousedown"
    @mouseup="mouseup"
    @mousemove="mousemove"
  />
</template>

<script>
import * as THREE from "three";
import { mapState } from "vuex";
// import { EventBus } from "../eventbus.js";
import { GLTFLoader } from "three/examples/jsm/loaders/GLTFLoader";

const remote = require("electron").remote;
const fs = remote.require("fs");

// import { library } from "@fortawesome/fontawesome-svg-core";
// import {
//   faSatelliteDish,
//   faPowerOff,
//   faPlug,
//   faLink
// } from "@fortawesome/free-solid-svg-icons";
// import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";

// library.add(faSatelliteDish);
// library.add(faPowerOff);
// library.add(faPlug);
// library.add(faLink);

export default {
  name: "Scanhome",
  data() {
    return {
      loaded: false,
      render: {},
      assetFile: "./src/assets/darksky-sphere.gltf",
      viewVector: {
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
    //FontAwesomeIcon
  },
  computed: mapState({
    state: state => state
  }),
  methods: {
    mousedown(event) {
      if (event.buttons == 1) {
        this.viewVector.dragging = true;
        this.viewVector.dx = event.clientX;
        this.viewVector.dy = event.clientY;
      }
    },
    mouseup(event) {
      if (event.buttons == 0) {
        this.viewVector.dragging = false;
      }
    },
    mousemove(event) {
      if (this.viewVector.dragging) {
        let dx = event.clientX - this.viewVector.dx;
        let dy = event.clientY - this.viewVector.dy;
        this.viewVector.dx = event.clientX;
        this.viewVector.dy = event.clientY;

        let scale = 0.01;

        this.viewVector.y += dx * scale;
        this.viewVector.x += dy * scale;
      }
    },
    visibilityChanged(isVisible) {
      if (isVisible === true) {
        this.load();
      } else {
        this.unload();
      }
    },
    doRender() {
      if (this.loaded == false || this.render == undefined) {
        return;
      }

      this.render.renderer.render(this.render.scene, this.render.camera);

      let object = this.render.scene.children[0];
      if (object !== undefined) {
        object.rotation.x = this.viewVector.x;
        object.rotation.y = this.viewVector.y;
        object.rotation.z = this.viewVector.z;
      }
      requestAnimationFrame(this.doRender);
    },
    remapMaterials(parent) {
      if (parent.hasOwnProperty("material")) {
        parent.material = new THREE.MeshNormalMaterial({
          wireframe: false
        });
      }
      // Process all children
      parent.children.forEach(child => {
        this.remapMaterials(child);
      });
    },
    load() {
      if (this.loaded === false) {
        this.loaded = true;
      } else {
        return;
      }

      let domObject = this.$refs["scanhome3d"];
      this.render.width = domObject.clientWidth;
      this.render.height = domObject.clientHeight;

      console.info(`Loading darksky-sphere.gltf`);

      this.render.scene = new THREE.Scene();
      this.render.camera = new THREE.PerspectiveCamera(
        60,
        this.render.width / this.render.height,
        0.1,
        10
      );

      this.render.renderer = new THREE.WebGLRenderer({
        canvas: this.$refs["scanhome3d"]
      });
      this.render.renderer.setSize(this.render.width, this.render.height);

      let loader = new GLTFLoader();
      let data = undefined;
      try {
        data = fs.readFileSync(this.assetFile);
      } catch (error) {
        console.error(`Error loading ScanHome 3D GLTF [${error}]`);
        return;
      }
      loader.parse(
        data,
        undefined,
        function(gltf) {
          console.info(`Loaded ScanHome 3D GLTF`);
          this.remapMaterials(gltf.scene);

          this.render.scene.add(gltf.scene);

          requestAnimationFrame(this.doRender);
        }.bind(this),
        function(error) {
          console.error(`Error loading [${this.assetFile}] ${error}`);
        }.bind(this)
      );
    },
    unload() {
      this.loaded = false;
      this.render = {};
      console.info(`Unloaded ScanHome 3D`);
    }
  }
};
</script>

<style scoped>
.home {
  height: 100vh;
  width: 100vw;
  background-color: #303050;
}
</style>