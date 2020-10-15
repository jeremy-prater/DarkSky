<template>
  <div>
    <div
      id="celestial-map"
      class="celestialmap"
      @mousemove="updateCoords"
      @contextmenu="mapMenu"
    ></div>
    <div class="overlaypanel mappanel">
      <div class="overlaypanel-title">
        <font-awesome-icon
          :icon="['fa', 'map-marked-alt']"
          size="lg"
          class="statusicon"
        />Map overlay
      </div>
      <div class="overlaypanel-text overlaypanel-item maplist">
        <ul>
          <li>Lat : {{ state.image["gps.lat"] }}</li>
          <li>Lng : {{ state.image["gps.lon"] }}</li>
          <li>Time : {{ state.image["gps.time"] }}</li>
          <li>JDE : {{ state.image["time.jde"] }}</li>
          <li>Sidereal: {{ state.image["time.sidereal.gmt"] }}</li>
          <li>
            RA : {{ common.deg2hms(mouseRADec[0]) }} ({{
              common.deg2dms(mouseRADec[0])
            }})
          </li>
          <li>Dec : {{ common.deg2dms(mouseRADec[1]) }}</li>
          <li>Az : {{ common.deg2dms(mouseAzAlt[0]) }}</li>
          <li>Alt : {{ common.deg2dms(mouseAzAlt[1]) }}</li>
        </ul>
      </div>

      <b-form-checkbox
        class="overlaypanel-text"
        v-model="celestialConfig.stars.show"
        @input="mapConfigChanged"
        >Stars</b-form-checkbox
      >
      <b-form-checkbox
        class="overlaypanel-text"
        v-model="celestialConfig.dsos.show"
        @input="mapConfigChanged"
        >Deep space objects</b-form-checkbox
      >
      <b-form-checkbox
        class="overlaypanel-text"
        v-model="celestialConfig.planets.show"
        @input="mapConfigChanged"
        >Planets</b-form-checkbox
      >
      <b-form-checkbox
        class="overlaypanel-text"
        v-model="celestialConfig.mw.show"
        @input="mapConfigChanged"
        >Milkyway</b-form-checkbox
      >

      <b-button @click="resetView">Reset View</b-button>
      <!-- v-color-picker v-model="color"></v-color-picker -->
    </div>
  </div>
</template>

<script>
import { mapState } from "vuex";
import { library } from "@fortawesome/fontawesome-svg-core";
import { faMapMarkedAlt } from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";
import { Celestial } from "d3-celestial";
// import { julian } from "astronomia";
import Rainbow from "rainbowvis.js";
import common from "./common";

library.add(faMapMarkedAlt);

export default {
  name: "Scanhome",
  data() {
    return {
      common: common,
      mouseRADec: [0, 0],
      mouseAzAlt: [0, 0],
      posSet: false,
      dishPos: {
        alt: 0,
        az: 0
      },
      dishGradient: new Rainbow(),
      dishTrack: {
        type: "FeatureCollection",
        // this is an array, add as many objects as you want
        features: []
      },
      celestialConfig: {
        width: 0, // Default width, 0 = full parent element width;
        // height is determined by projection
        projection: "armadillo", // Map projection used: see below
        transform: "equatorial", // Coordinate transformation: equatorial (default),
        // ecliptic, galactic, supergalactic
        center: null, // Initial center coordinates in set transform
        // [longitude, latitude, orientation] all in degrees
        // null = default center [0,0,0]
        orientationfixed: true, // Keep orientation angle the same as center[2]
        geopos: null, // optional initial geographic position [lat,lon] in degrees,
        // overrides center
        follow: "zenith", // on which coordinates to center the map, default: zenith, if location enabled,
        // otherwise center
        zoomlevel: 1, // initial zoom level 0...zoomextend; 0|null = default, 1 = 100%, 0 < x <= zoomextend
        zoomextend: 10, // maximum zoom level
        adaptable: true, // Sizes are increased with higher zoom-levels
        interactive: true, // Enable zooming and rotation with mousewheel and dragging
        form: false, // Display form for interactive settings. Needs a div with
        // id="celestial-form", created automatically if not present
        location: false, // Display location settings. Deprecated, use formFields below
        formFields: {
          location: true, // Set visiblity for each group of fields with the respective id
          general: true,
          stars: true,
          dsos: true,
          constellations: true,
          lines: true,
          other: true,
          download: false
        },
        advanced: false, // Display fewer form fields if false
        daterange: [], // Calender date range; null: displaydate-+10; [n<100]: displaydate-+n; [yr]: yr-+10;
        // [yr, n<100]: [yr-n, yr+n]; [yr0, yr1]
        controls: true, // Display zoom controls
        lang: "", // Global language override for names, any name setting that has the chosen language available
        // Default: desig or empty string for designations, other languages as used anywhere else
        culture: "", // Source of constellations and star names, default "iau", other: "cn" Traditional Chinese
        container: "celestial-map", // ID of parent element, e.g. div, null = html-body
        datapath: "data/", // Path/URL to data files, empty = subfolder 'data'
        stars: {
          show: true, // Show stars
          limit: 5, // Show only stars brighter than limit magnitude
          colors: true, // Show stars in spectral colors, if not use default color
          style: { fill: "#ffffff", opacity: 1 }, // Default style for stars
          designation: true, // Show star names (Bayer, Flamsteed, Variable star, Gliese or designation,
          // i.e. whichever of the previous applies first); may vary with culture setting
          designationType: "desig", // Which kind of name is displayed as designation (fieldname in starnames.json)
          designationStyle: {
            fill: "#ddddbb",
            font:
              "11px 'Palatino Linotype', Georgia, Times, 'Times Roman', serif",
            align: "left",
            baseline: "top"
          },
          designationLimit: 2.5, // Show only names for stars brighter than nameLimit
          propername: false, // Show proper name (if present)
          propernameType: "name", // Languge for proper name, default IAU name; may vary with culture setting
          // (see list below of languages codes available for stars)
          propernameStyle: {
            fill: "#ddddbb",
            font:
              "13px 'Palatino Linotype', Georgia, Times, 'Times Roman', serif",
            align: "right",
            baseline: "bottom"
          },
          propernameLimit: 1.5, // Show proper names for stars brighter than propernameLimit
          size: 7, // Maximum size (radius) of star circle in pixels
          exponent: -0.28, // Scale exponent for star size, larger = more linear
          data: "stars.6.json" // Data source for stellar data,
          // number indicates limit magnitude
        },
        dsos: {
          show: false, // Show Deep Space Objects
          limit: 10, // Show only DSOs brighter than limit magnitude
          colors: true, // // Show DSOs in symbol colors if true, use style setting below if false
          style: { fill: "#cccccc", stroke: "#cccccc", width: 2, opacity: 1 }, // Default style for dsos
          names: true, // Show DSO names
          namesType: "name", // Type of DSO ('desig' or language) name shown
          // (see list below for languages codes available for dsos)
          nameStyle: {
            fill: "#cccccc",
            font: "11px Helvetica, Arial, serif",
            align: "left",
            baseline: "top"
          }, // Style for DSO names
          nameLimit: 6, // Show only names for DSOs brighter than namelimit
          size: null, // Optional seperate scale size for DSOs, null = stars.size
          exponent: 1.4, // Scale exponent for DSO size, larger = more non-linear
          data: "dsos.bright.json", // Data source for DSOs,
          // opt. number indicates limit magnitude
          symbols: {
            //DSO symbol styles, 'stroke'-parameter present = outline
            gg: { shape: "circle", fill: "#ff0000" }, // Galaxy cluster
            g: { shape: "ellipse", fill: "#ff0000" }, // Generic galaxy
            s: { shape: "ellipse", fill: "#ff0000" }, // Spiral galaxy
            s0: { shape: "ellipse", fill: "#ff0000" }, // Lenticular galaxy
            sd: { shape: "ellipse", fill: "#ff0000" }, // Dwarf galaxy
            e: { shape: "ellipse", fill: "#ff0000" }, // Elliptical galaxy
            i: { shape: "ellipse", fill: "#ff0000" }, // Irregular galaxy
            oc: {
              shape: "circle",
              fill: "#is:closed ffcc00",
              stroke: "#ffcc00",
              width: 1.5
            }, // Open cluster
            gc: { shape: "circle", fill: "#ff9900" }, // Globular cluster
            en: { shape: "square", fill: "#ff00cc" }, // Emission nebula
            bn: {
              shape: "square",
              fill: "#ff00cc",
              stroke: "#ff00cc",
              width: 2
            }, // Generic bright nebula
            sfr: {
              shape: "square",
              fill: "#cc00ff",
              stroke: "#cc00ff",
              width: 2
            }, // Star forming region
            rn: { shape: "square", fill: "#00ooff" }, // Reflection nebula
            pn: { shape: "diamond", fill: "#00cccc" }, // Planetary nebula
            snr: { shape: "diamond", fill: "#ff00cc" }, // Supernova remnant
            dn: {
              shape: "square",
              fill: "#999999",
              stroke: "#999999",
              width: 2
            }, // Dark nebula grey
            pos: {
              shape: "marker",
              fill: "#cccccc",
              stroke: "#cccccc",
              width: 1.5
            } // Generic marker
          }
        },
        planets: {
          //Show planet locations, if date-time is set
          show: false,
          // List of all objects to show
          which: [
            "sol",
            "mer",
            "ven",
            "ter",
            "lun",
            "mar",
            "jup",
            "sat",
            "ura",
            "nep"
          ],
          // Font styles for planetary symbols
          symbols: {
            // Character and color for each symbol in 'which' above (simple circle: \u25cf), optional size override for Sun & Moon
            sol: { symbol: "\u2609", letter: "Su", fill: "#ffff00", size: "" },
            mer: { symbol: "\u263f", letter: "Me", fill: "#cccccc" },
            ven: { symbol: "\u2640", letter: "V", fill: "#eeeecc" },
            ter: { symbol: "\u2295", letter: "T", fill: "#00ccff" },
            lun: { symbol: "\u25cf", letter: "L", fill: "#ffffff", size: "" }, // overridden by generated crecent, except letter & size
            mar: { symbol: "\u2642", letter: "Ma", fill: "#ff6600" },
            cer: { symbol: "\u26b3", letter: "C", fill: "#cccccc" },
            ves: { symbol: "\u26b6", letter: "Ma", fill: "#cccccc" },
            jup: { symbol: "\u2643", letter: "J", fill: "#ffaa33" },
            sat: { symbol: "\u2644", letter: "Sa", fill: "#ffdd66" },
            ura: { symbol: "\u2645", letter: "U", fill: "#66ccff" },
            nep: { symbol: "\u2646", letter: "N", fill: "#6666ff" },
            plu: { symbol: "\u2647", letter: "P", fill: "#aaaaaa" },
            eri: { symbol: "\u26aa", letter: "E", fill: "#eeeeee" }
          },
          symbolStyle: {
            fill: "#00ccff",
            font: "bold 17px 'Lucida Sans Unicode', Consolas, sans-serif",
            align: "center",
            baseline: "middle"
          },
          symbolType: "symbol", // Type of planet symbol: 'symbol' graphic planet sign, 'disk' filled circle scaled by magnitude
          // 'letter': 1 or 2 letters S Me V L Ma J S U N
          names: false, // Show name in nameType language next to symbol
          nameStyle: {
            fill: "#00ccff",
            font: "14px 'Lucida Sans Unicode', Consolas, sans-serif",
            align: "right",
            baseline: "top"
          },
          namesType: "desig" // Language of planet name (see list below of language codes available for planets),
          // or desig = 3-letter designation
        },
        constellations: {
          names: true, // Show constellation names
          namesType: "desig", // Type of name Latin (iau, default), 3 letter designation (desig) or other language (see list below)
          nameStyle: {
            fill: "#cccc99",
            align: "center",
            baseline: "middle",
            font: [
              "14px Helvetica, Arial, sans-serif", // Style for constellations
              "12px Helvetica, Arial, sans-serif", // Different fonts for diff.
              "11px Helvetica, Arial, sans-serif"
            ]
          }, // ranked constellations
          lines: true, // Show constellation lines, style below
          lineStyle: { stroke: "#cccccc", width: 1, opacity: 0.6 },
          bounds: true, // Show constellation boundaries, style below
          boundStyle: {
            stroke: "#cccc00",
            width: 0.5,
            opacity: 0.8,
            dash: [2, 4]
          }
        },
        mw: {
          show: false, // Show Milky Way as filled multi-polygon outlines
          style: { fill: "#ffffff", opacity: 0.15 } // Style for MW layers
        },
        lines: {
          // Display & styles for graticule & some planes
          graticule: {
            show: true,
            stroke: "#cccccc",
            width: 0.6,
            opacity: 0.8,
            // grid values: "outline", "center", or [lat,...] specific position
            lon: {
              pos: [""],
              fill: "#eee",
              font: "10px Helvetica, Arial, sans-serif"
            },
            // grid values: "outline", "center", or [lon,...] specific position
            lat: {
              pos: [""],
              fill: "#eee",
              font: "10px Helvetica, Arial, sans-serif"
            }
          },
          equatorial: {
            show: true,
            stroke: "#aaaaaa",
            width: 1.3,
            opacity: 0.7
          },
          ecliptic: { show: true, stroke: "#66cc66", width: 1.3, opacity: 0.7 },
          galactic: {
            show: false,
            stroke: "#cc6666",
            width: 1.3,
            opacity: 0.7
          },
          supergalactic: {
            show: false,
            stroke: "#cc66cc",
            width: 1.3,
            opacity: 0.7
          }
        },
        background: {
          // Background style
          fill: "#000000", // Area fill
          opacity: 1,
          stroke: "#000000", // Outline
          width: 1.5
        },
        horizon: {
          //Show horizon marker, if location is set and map projection is all-sky
          show: true,
          stroke: "#cccccc", // Line
          width: 1.0,
          fill: "#000000", // Area below horizon
          opacity: 0.5
        },
        daylight: {
          //Show day sky as a gradient, if location is set and map projection is hemispheric
          show: false
        }
      },
      mapColors: "#000000"
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
        return this[this.mapColors];
      },
      set(v) {
        this[this.mapColors] = v;
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

    this.celestial = Celestial();
    this.celestial.add({
      file: "",
      type: "json",
      callback: this.generateDishTrack,
      redraw: this.redrawDishTrack
    });
    this.celestial.display(this.celestialConfig);
    setInterval(this.tick, 1000);
  },
  methods: {
    mapMenu(event) {
      console.log(event);
    },
    updateCoords(event) {
      let radec = this.celestial.mapProjection.invert([
        event.offsetX,
        event.offsetY
      ]);
      if (radec != null) {
        this.mouseRADec = radec;
      }
      this.updateAzAlt();
    },
    updateAzAlt() {
      if (this.state.image["gps.mode"] >= 2) {
        const azalt = common.convertRADec2AzAlt(this.state, {
          ra: this.mouseRADec[0],
          dec: this.mouseRADec[1]
        });
        if (azalt.alt >= 180)
          azalt.alt -= 360;
        this.mouseAzAlt = [azalt.az, azalt.alt];
      }
    },
    mapConfigChanged() {
      console.log("Map config changed!");
      console.log(this.celestialConfig);
      this.celestial.apply(this.celestialConfig);
    },
    resetView() {
      console.log("Resetting View");
      this.posSet = true;
    },
    generateDishTrack(error, json) {
      error;
      json;
      // console.log("generate dish track");
      this.dishTrack.features = [];
      // Convert incoming lineString from backend
      // this.dishTrack.features[0].geometry.coordinates[0] = [];
      // this.dishTrack.features[0].geometry.strength[0] = this.state.actual.dish.historyStrength;
      let lastRADec = null;
      let dcCounter = 0;
      this.state.image["dish.historyPath"].forEach(coords => {
        if (lastRADec == null) {
          lastRADec = common.convertAzAlt2RADec(this.state, {
            az: coords[0],
            alt: coords[1]
          });
          return;
        }

        let currentRADec = common.convertAzAlt2RADec(this.state, {
          az: coords[0],
          alt: coords[1]
        });

        this.dishTrack.features.push({
          type: "Feature",
          id: "DishTrack+" + dcCounter,
          properties: {
            strokeColor:
              "#" +
              this.dishGradient.colorAt(
                this.state.image["dish.historyStrength"][dcCounter++]
              )
          },
          geometry: {
            // the line object as an array of point coordinates,
            // always as [ra -180..180 degrees, dec -90..90 degrees]
            type: "LineString",
            coordinates: [
              [lastRADec.ra + 180, lastRADec.dec],
              [currentRADec.ra + 180, currentRADec.dec]
            ]
          }
        });
        lastRADec = currentRADec;

        // const radec = this.convertAzAlt2RADec({
        //   az: coords[0],
        //   alt: coords[1]
        // });
        // this.dishTrack.features[0].geometry.coordinates[0].push([
        //   radec.ra,
        //   radec.dec
        // ]);
        // this.dishTrack.features[0].geometry.strength;
      });
      // this.dishTrack.features[1].geometry.coordinates = this.dishTrack.features[0].geometry.coordinates[0][0];
      let dishTarget = common.convertAzAlt2RADec(this.state, {
        az: this.state.image["dish.az"],
        alt: this.state.image["dish.alt"]
      });

      this.dishTrack.features.push({
        type: "Feature",
        id: "DishTarget",
        properties: {
          // Name
          name: "Dish Target",
          // magnitude, dimension in arcseconds or any other size criterion
          mag: 1,
          dim: 30
        },
        geometry: {
          // the location of the object as a [ra, dec] array in degrees [-180..180, -90..90]
          type: "Point",
          coordinates: [dishTarget.ra + 180, dishTarget.dec]
        }
      });

      // Load the geoJSON file and transform to correct coordinate system, if necessary
      const dishTrack = this.celestial.getData(
        this.dishTrack,
        this.celestialConfig.transform
      );

      // Add to celestial objects container in d3
      this.celestial.container
        .selectAll(".dishtrack")
        .data(dishTrack.features)
        .enter()
        .append("path")
        .attr("class", "dishtrack");
    },
    redrawDishTrack() {
      // console.log("redraw dish track");
      const textStyle = {
        stroke: "#4040ff",
        font: "bold 15px Helvetica, Arial, sans-serif",
        align: "center",
        baseline: "bottom"
      };

      // Add more line segments!!
      if (this.state.image["gps.mode"] >= 2) {
        this.generateDishTrack(null, null);
      }

      // Select the added objects by class name as given previously
      this.celestial.container.selectAll(".dishtrack").each(
        function(d) {
          const geometry = d.geometry.type;
          // console.log(JSON.stringify(d));
          switch (geometry) {
            case "LineString":
              this.celestial.setStyle({
                stroke: d.properties.strokeColor,
                width: 3
              });
              // Project objects on map
              this.celestial.map(d);
              // draw on canvas
              // this.celestial.context.fill();
              this.celestial.context.stroke();
              break;

            case "Point":
              if (this.celestial.clip(d.geometry.coordinates)) {
                // get point coordinates
                var pt = this.celestial.mapProjection(d.geometry.coordinates);
                // object radius in pixel, could be varable depending on e.g. dimension or magnitude
                var r = Math.pow(20 - d.properties.mag, 0.7); // replace 20 with dimmest magnitude in the data

                // draw on canvas
                //  Set object styles fill color, line color & width etc.
                this.celestial.setStyle({
                  stroke:
                    "#" +
                    this.dishGradient.colorAt(this.state.image["lnb.strength"]),
                  width: 3
                });
                // Start the drawing path
                this.celestial.context.beginPath();
                // Thats a circle in html5 canvas
                this.celestial.context.arc(pt[0], pt[1], r, 0, 2 * Math.PI);
                // Finish the drawing path
                this.celestial.context.closePath();
                // Draw a line along the path with the prevoiusly set stroke color and line width
                this.celestial.context.stroke();

                // Set text styles
                this.celestial.setTextStyle(textStyle);
                // and draw text on canvas
                this.celestial.context.fillText(
                  d.properties.name,
                  pt[0] + r - 1,
                  pt[1] - r + 1
                );
              }
              break;
          }
        }.bind(this)
      );
    },
    tick() {
      // What is this for?
      if (this.state.image["gps.mode"] >= 2) {
        const gpsLat = this.state.image["gps.lat"];
        const gpsLon = this.state.image["gps.lon"];
        if (
          this.celestialConfig.geopos == null ||
          this.celestialConfig.geopos[0] != gpsLat ||
          this.celestialConfig.geopos[1] != gpsLon
        ) {
          this.posSet = true;
          const curGPS = [gpsLat, gpsLon];
          console.log(
            "GPS Coords changed @ Lat : " + curGPS[0] + " Lon : " + curGPS[1]
          );
          this.celestialConfig.geopos = curGPS;
          this.celestial.reload(this.celestialConfig);
        }
      }

      if (this.posSet) {
        this.posSet = false;
        this.celestial.skyview({
          date: this.state.image["gps.time"],
          location: [this.state.image["gps.lat"], this.state.image["gps.lon"]]
        });
      }

      this.celestial.redraw();
      this.updateAzAlt();
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
    load() {
      console.log("load");
    },
    unload() {
      console.info(`unload`);
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

.celestialmap {
  padding: 0px;
  margin: 0px;
  width: 100vw;
  background-color: #000000;
}
</style>
