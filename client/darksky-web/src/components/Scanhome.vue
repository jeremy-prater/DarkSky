<template>
  <div class="home" ref="scanhome3dparent" @resize="resize">
    <canvas
      ref="scanhome3d"
      class="home"
      v-observe-visibility="visibilityChanged"
      @mousedown="mousedown"
      @mouseup="mouseup"
      @mousemove="mousemove"
      @mousewheel="mousewheel"
    />
  </div>
</template>

<script>
import * as THREE from "three";
import { mapState } from "vuex";
import Vue from "vue";
import VueObserveVisibility from "vue-observe-visibility";
Vue.use(VueObserveVisibility);

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
      size: {
        width: 0,
        height: 0
      },
      loaded: false,
      sphereQuality: 256,
      render: {},
      assetFile: "/darksky-sphere.gltf",
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
    //FontAwesomeIcon
  },
  computed: mapState({
    state: state => state
  }),
  mounted() {
    console.log("Created Scanhome...");
    window.addEventListener(
      "resize",
      function(event) {
        this.resize(event);
      }.bind(this)
    );
  },
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

        let scale = (this.viewVector.zoom + 1) * 0.01;

        this.viewVector.y += dx * scale;
        this.viewVector.x += dy * scale;
      }
    },
    mousewheel(event) {
      let scale = 0.001;
      this.viewVector.zoom += scale * event.deltaY;

      if (this.viewVector.zoom > 0.99) {
        this.viewVector.zoom = 0.99;
      }

      if (this.viewVector.zoom < -0.99) {
        this.viewVector.zoom = -0.99;
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

      this.render.renderer.render(this.render.scene, this.render.camera);

      let object = this.render.scene.children[0];
      if (object !== undefined) {
        object.rotation.x = this.viewVector.x;
        object.rotation.y = this.viewVector.y;
        object.rotation.z = this.viewVector.z;

        this.render.camera.position.z = this.viewVector.zoom;
      }
      requestAnimationFrame(this.doRender);
    },
    remapMaterials(parent) {
      if (Object.prototype.hasOwnProperty.call(parent, "material")) {
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

      let domObject = this.$refs["scanhome3dparent"];
      this.render.width = domObject.clientWidth;
      this.render.height = domObject.clientHeight;

      console.info(`Loading darksky-sphere.gltf`);

      this.render.scene = new THREE.Scene();
      this.render.camera = new THREE.PerspectiveCamera(
        60,
        this.render.width / this.render.height,
        0.0001,
        10
      );

      this.render.renderer = new THREE.WebGLRenderer({
        canvas: this.$refs["scanhome3d"]
      });
      this.render.renderer.setSize(this.render.width, this.render.height);

      var geometry = new THREE.SphereGeometry(
        1,
        this.sphereQuality,
        this.sphereQuality
      );
      var material = new THREE.MeshNormalMaterial({
        wireframe: true
      });
      material.side = THREE.BackSide;
      var sphere = new THREE.Mesh(geometry, material);
      this.render.scene.add(sphere);
      requestAnimationFrame(this.doRender);
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
  width: 100vw;
  height: 100vh;
  background-color: #303050;
}
</style>
