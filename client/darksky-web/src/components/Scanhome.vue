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
// const vertexShader = `
//   varying vec2 vUV;

//   void main() {
//     vUV = uv;

//     gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
//   }
// `;

// const fragmentShader = `
//   uniform vec3 gridColor;
//   uniform vec3 figuresColor;
//   uniform vec3 boundariesColor;
//   uniform vec3 starmapColor;
//   uniform vec3 lnbColor;

//   uniform sampler2D grid;
//   uniform sampler2D figures;
//   uniform sampler2D boundaries;
//   uniform sampler2D starmap;
//   uniform sampler2D lnb;

//   varying vec2 vUV;

//   void main()
//   {
//     gl_FragColor =
//       (vec4(gridColor,1) * texture2D(grid, vUV)) +
//       (vec4(figuresColor,1) * texture2D(figures, vUV)) +
//       (vec4(boundariesColor,1) * texture2D(boundaries, vUV)) +
//       (vec4(starmapColor,1) * texture2D(starmap, vUV)) +
//       (vec4(lnbColor,1) *  texture2D(lnb, vUV));
//   }
// `;

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
      sphereQuality: 256,
      render: {},
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
      }.bind(this)
    );

    // this.geometry = new THREE.SphereGeometry(
    //   1,
    //   this.sphereQuality,
    //   this.sphereQuality
    // );
    // this.assignUVs(this.geometry);

    // this.texture_grid = new THREE.TextureLoader().load("/celestial_grid.png");
    // this.texture_figures = new THREE.TextureLoader().load(
    //   "/constellation_figures.png"
    // );
    // this.texture_boundaries = new THREE.TextureLoader().load(
    //   "/constellation_boundaries.png"
    // );
    // this.texture_starmap = new THREE.TextureLoader().load("/starmap_8k.jpg");

    setInterval(this.tick, 1000);
  },
  methods: {
    tick() {
      // this.$store.commit("updateTime", new Date(Date.now()).toISOString());
    },
    // mousedown(event) {
    //   if (event.buttons == 1) {
    //     this.viewVector.dragging = true;
    //     this.viewVector.dx = event.clientX;
    //     this.viewVector.dy = event.clientY;
    //   }
    // },
    // mouseup(event) {
    //   if (event.buttons == 0) {
    //     this.viewVector.dragging = false;
    //   }
    // },
    // mousemove(event) {
    //   if (this.viewVector.dragging) {
    //     let dx = event.clientX - this.viewVector.dx;
    //     let dy = event.clientY - this.viewVector.dy;
    //     this.viewVector.dx = event.clientX;
    //     this.viewVector.dy = event.clientY;

    //     let scale = (this.viewVector.zoom + 1) * 0.001;

    //     this.viewVector.y += -dx * scale;
    //     this.viewVector.x += -dy * scale;
    //   }
    // },
    // mousewheel(event) {
    //   let scale = 0.001;
    //   this.viewVector.zoom += scale * event.deltaY;

    //   if (this.viewVector.zoom > 0.99) {
    //     this.viewVector.zoom = 0.99;
    //   }

    //   if (this.viewVector.zoom < -0.99) {
    //     this.viewVector.zoom = -0.99;
    //   }
    // },
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
      this.render.trackball.rotateCamera();
      this.render.trackball.zoomCamera();
      this.render.renderer.render(this.render.scene, this.render.camera);

      // this.render.camera.lookAt(
      //   this.viewVector.x,
      //   this.viewVector.y,
      //   this.viewVector.z
      // );
      // this.render.camera.position.z = this.viewVector.zoom;

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

      console.info(`Creating darksky scene`);

      this.render.scene = new THREE.Scene();
      this.render.camera = new THREE.PerspectiveCamera(
        60,
        this.render.width / this.render.height,
        0.0001,
        10
      );

      this.render.trackball = new TrackballControls(
        this.render.camera,
        domObject
      );

      this.render.renderer = new THREE.WebGLRenderer({
        canvas: this.$refs["scanhome3d"]
      });
      this.render.renderer.setSize(this.render.width, this.render.height);

      // Version : 1 - built in wireframe shader
      // let material = new THREE.MeshNormalMaterial({
      //   wireframe: true
      // });

      // Version : 2 - built in texture mapper
      // immediately use the texture for material creation
      // let material = new THREE.MeshBasicMaterial({ map: this.texture_starmap });

      // Verion : 3 - Custom texture layers
      // let material = new THREE.ShaderMaterial({
      //   uniforms: {
      //     gridColor: { value: new Vector3(1, 0, 0) },
      //     boundariesColor: { value: new Vector3(0, 0.5, 0) },
      //     figuresColor: { value: new Vector3(0, 0, 1) },
      //     starmapColor: { value: new Vector3(1, 1, 1) },
      //     lnbColor: { value: new Vector3(0, 0, 0) },
      //     grid: { value: this.texture_grid },
      //     boundaries: { value: this.texture_boundaries },
      //     figures: { value: this.texture_figures },
      //     starmap: { value: this.texture_starmap },
      //     lnb: { value: this.texture_starmap }
      //   },
      //   vertexShader: vertexShader,
      //   fragmentShader: fragmentShader
      // });
      // material.side = THREE.DoubleSide;
      // let sphere = new THREE.Mesh(this.geometry, material);
      // this.render.scene.add(sphere);

      // Version : 4 - All custom

      let geometry = new THREE.CylinderGeometry(1, 1, 0.01, 64, 1, true);
      let material = new THREE.MeshBasicMaterial({
        color: 0xff0000,
        side: THREE.DoubleSide
      });
      let mesh = new THREE.Mesh(geometry, material);
      // mesh.position.z = -2;
      this.render.scene.add(mesh);

      let geometry2 = new THREE.CylinderGeometry(1, 1, 0.01, 64, 1, true);
      let material2 = new THREE.MeshBasicMaterial({
        color: 0xffff00,
        side: THREE.DoubleSide
      });
      let mesh2 = new THREE.Mesh(geometry2, material2);
      // mesh2.position.z = -2;
      mesh2.rotation.z = Math.PI / 2;
      this.render.scene.add(mesh2);

      // let segments = 32;
      // for (let segment = 0; segment <= segments; segment++) {
      //   let x = segment / segments;
      //   console.log(x);
      //   let phi = Math.sin(Math.PI * x);
      //   console.log(phi);
      //   if (phi > 0) {
      //     let geometry = new THREE.CylinderGeometry(
      //       phi,
      //       phi,
      //       0.01,
      //       64,
      //       1,
      //       true
      //     );
      //     let mesh = new THREE.Mesh(geometry, material);
      //     mesh.position.y = x - 0.5;
      //     mesh.position.z = -2;
      //     this.render.scene.add(mesh);
      //   }
      // }

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
