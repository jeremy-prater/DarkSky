<template>
  <div>
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
    <div class="overlaypanel mappanel">
      <div class="overlaypanel-title">
        <font-awesome-icon
          :icon="['fa', 'map-marked-alt']"
          size="lg"
          class="statusicon"
        />Map overlay
      </div>
      <v-color-picker v-model="color"></v-color-picker>
    </div>
  </div>
</template>

<script>
import * as THREE from "three";
import { mapState } from "vuex";
import { Vector3 } from "three";
import { library } from "@fortawesome/fontawesome-svg-core";
import { faMapMarkedAlt } from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";

library.add(faMapMarkedAlt);

function cartesian2polar(position) {
  var r = Math.sqrt(
    position.x * position.x + position.z * position.z + position.y * position.y
  );

  return {
    r: r,
    phi: Math.acos(position.y / r),
    theta: Math.atan2(position.z, -position.x)
  };
}
// function polar2cartesian(polar) {
//   return {
//     x: polar.distance * Math.cos(polar.radians),
//     z: polar.distance * Math.sin(polar.radians)
//   };
// }
function polar2canvas(polarPoint) {
  return {
    y: polarPoint.phi / Math.PI,
    x: (polarPoint.theta + Math.PI) / (2 * Math.PI)
  };
}

const vertexShader = `
  varying vec2 vUV;

  void main() {
    vUV = uv;

    gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
  }
`;

const fragmentShader = `
  uniform vec3 gridColor;
  uniform vec3 figuresColor;
  uniform vec3 boundariesColor;
  uniform vec3 starmapColor;
  uniform vec3 lnbColor;
 
  uniform sampler2D grid;
  uniform sampler2D figures;
  uniform sampler2D boundaries;
  uniform sampler2D starmap;
  uniform sampler2D lnb;

  varying vec2 vUV;

  void main()
  {
    gl_FragColor =
      (vec4(gridColor,1) * texture2D(grid, vUV)) +
      (vec4(figuresColor,1) * texture2D(figures, vUV)) +
      (vec4(boundariesColor,1) * texture2D(boundaries, vUV)) +
      (vec4(starmapColor,1) * texture2D(starmap, vUV)) +
      (vec4(lnbColor,1) *  texture2D(lnb, vUV));
  }
`;

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
      texture_grid: null,
      texture_figures: null,
      texture_boundaries: null,
      texture_starmap: null,
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

    this.geometry = new THREE.SphereGeometry(
      1,
      this.sphereQuality,
      this.sphereQuality
    );
    this.assignUVs(this.geometry);

    this.texture_grid = new THREE.TextureLoader().load("/celestial_grid.png");
    this.texture_figures = new THREE.TextureLoader().load(
      "/constellation_figures.png"
    );
    this.texture_boundaries = new THREE.TextureLoader().load(
      "/constellation_boundaries.png"
    );
    this.texture_starmap = new THREE.TextureLoader().load("/starmap_8k.jpg");
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

        let scale = (this.viewVector.zoom + 1) * 0.001;

        this.viewVector.y += -dx * scale;
        this.viewVector.x += -dy * scale;
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
    assignUVs(geometry) {
      // This function is based on the code found at (the original source doesn't work well)
      // http://stackoverflow.com/questions/20774648/three-js-generate-uv-coordinate
      //
      // She following page explains how UV map should be calculated
      // https://solutiondesign.com/blog/-/blogs/webgl-and-three-js-texture-mappi-1/
      //
      // The following documentation shows what a apherical UV map should look like
      // https://threejs.org/examples/#misc_uv_tests

      // converting all vertices into polar coordinates
      var polarVertices = geometry.vertices.map(cartesian2polar);

      geometry.faceVertexUvs[0] = []; // This clears out any UV mapping that may have already existed on the object

      // walking through all the faces defined by the object
      // ... we need to define a UV map for each of them
      geometry.faces.forEach(function(face) {
        var uvs = [];

        // Each face is a triangle defined by three points or vertices (point a, b and c).
        // Instead of storing the three points (vertices) by itself,
        // a face uses points from the [vertices] array.
        // The 'a', 'b' and 'c' properties of the [face] object in fact represent
        // index at which each of the three points is stored in the [vertices] array
        var ids = ["a", "b", "c"];

        for (var i = 0; i < ids.length; i++) {
          // using the point to access the vertice
          var vertexIndex = face[ids[i]];
          var vertex = polarVertices[vertexIndex];

          // If the vertice is located at the top or the bottom
          // of the sphere, the x coordinates will always be 0
          // This isn't good, since it will make all the faces
          // which meet at this point use the same starting point
          // for their texture ...
          // this is a bit difficult to explainm, so try to comment out
          // the following block and take look at the top of the
          // spehere to see how it is mapped. Also have a look
          // at the following image: https://dev.ngit.hr/vr/textures/sphere-uv.png
          if (
            vertex.theta === 0 &&
            (vertex.phi === 0 || vertex.phi === Math.PI)
          ) {
            // at the sphere bottom and at the top different
            // points are alligned differently - have a look at the
            // following image https://dev.ngit.hr/vr/textures/sphere-uv.png
            var alignedVertice = vertex.phi === 0 ? face.b : face.a;

            vertex = {
              phi: vertex.phi,
              theta: polarVertices[alignedVertice].theta
            };
          }

          // Fixing vertices, which close the gap in the circle
          // These are the last vertices in a row, and are at identical position as
          // vertices which are at the first position in the row.
          // This causes the [theta] angle to be miscalculated
          if (
            vertex.theta === Math.PI &&
            cartesian2polar(face.normal).theta < Math.PI / 2
          ) {
            vertex.theta = -Math.PI;
          }

          var canvasPoint = polar2canvas(vertex);

          uvs.push(new THREE.Vector2(1 - canvasPoint.x, 1 - canvasPoint.y));
        }

        geometry.faceVertexUvs[0].push(uvs);
      });
      geometry.uvsNeedUpdate = true;
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
      let material = new THREE.ShaderMaterial({
        uniforms: {
          gridColor: { value: new Vector3(1, 0, 0) },
          boundariesColor: { value: new Vector3(0, 0.5, 0) },
          figuresColor: { value: new Vector3(0, 0, 1) },
          starmapColor: { value: new Vector3(1, 1, 1) },
          lnbColor: { value: new Vector3(0, 0, 0) },
          grid: { value: this.texture_grid },
          boundaries: { value: this.texture_boundaries },
          figures: { value: this.texture_figures },
          starmap: { value: this.texture_starmap },
          lnb: { value: this.texture_starmap }
        },
        vertexShader: vertexShader,
        fragmentShader: fragmentShader
      });

      // Generate scene
      material.side = THREE.DoubleSide;
      let sphere = new THREE.Mesh(this.geometry, material);
      this.render.scene.add(sphere);
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
  bottom: 79vh;
  left: 79vw;
  height: 20vh;
  width: 20vw;
}
</style>
