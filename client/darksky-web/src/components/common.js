import { coord, globe, base } from 'astronomia';
const lodash = require('lodash');

function diff(obj1, obj2) {
    return lodash.reduce(
        obj1,
        function(result, value, key) {
            if (lodash.isPlainObject(value)) {
                result[key] = diff(value, obj2[key]);
            } else if (!lodash.isEqual(value, obj2[key])) {
                result[key] = value;
            }
            return result;
        },
        {}
    );
}

export default {
    diffObjects: diff,
    checkRadBounds: function(radian) {
        while (radian > 2 * Math.PI) {
            radian -= 2 * Math.PI;
        }
        while (radian < 0) {
            radian += 2 * Math.PI;
        }
        return radian;
    },
    checkDegBounds: function(degrees) {
        while (degrees > 360) {
            degrees -= 360;
        }
        while (degrees < 0) {
            degrees += 360;
        }
        return degrees;
    },
    convertRADec2AzAlt: function(state, coords) {
        let eqCoord = new coord.Equatorial(
            this.checkRadBounds(base.toRad(coords.ra)),
            this.checkRadBounds(base.toRad(coords.dec))
        );

        let altaz = eqCoord.toHorizontal(
            new globe.Coord(
                state.image['gps.lat'],
                state.image['gps.lon']
            ),
            state.image['time.sidereal.gmt']
        );
        return {
            az: this.checkDegBounds(base.toDeg(altaz.az)),
            alt: this.checkDegBounds(base.toDeg(altaz.alt)),
        };
    },
    convertAzAlt2RADec: function(state, coords) {
        let eqCoord = new coord.Horizontal(
            this.checkRadBounds(base.toRad(coords.az)),
            this.checkRadBounds(base.toRad(coords.alt))
        );

        let radec = eqCoord.toEquatorial(
            new globe.Coord(state.image['gps.lat'], state.image['gps.lon']),
            state.image['time.sidereal.gmt']
        );
        return {
            ra: this.checkDegBounds(base.toDeg(radec.ra)),
            dec: this.checkDegBounds(base.toDeg(radec.dec)),
        };
    },
    deg2hms(deg) {
        if (deg === null || isNaN(parseFloat(deg))) return;
        let ra = deg < 0 ? (deg + 360) / 15 : deg / 15,
            h = Math.floor(ra),
            rest1 = (ra - h) * 60,
            m = Math.floor(rest1),
            rest2 = (rest1 - m) * 60,
            s = Math.round(rest2);
        return '' + this.pad(h) + 'ʰ ' + this.pad(m) + 'ᵐ ' + this.pad(s) + 'ˢ';
    },
    deg2dms(deg) {
        if (deg === null || isNaN(parseFloat(deg))) return;
        let d = Math.floor(deg),
            rest1 = (deg - d) * 60,
            m = Math.floor(rest1),
            rest2 = (rest1 - m) * 60,
            s = Math.round(rest2);
        return '' + this.pad(d) + '° ' + this.pad(m) + '′ ' + this.pad(s) + '″';
    },

    pad(n) {
        if (n < 0) return n > -10 ? '-0' + Math.abs(n) : n;
        return n < 10 ? '0' + n : n;
    },
};
