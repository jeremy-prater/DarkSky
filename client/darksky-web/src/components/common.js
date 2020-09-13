import { coord, globe, base, sidereal } from 'astronomia';

export default {
    checkRadBounds: function(radian) {
        while (radian > 2 * Math.PI)
        {
            radian -= Math.PI;
        }
        while (radian < 0) {
            radian += Math.PI;
        }
        return radian;
    },
    convertRADec2AzAlt: function(state, coords) {
        let eqCoord = new coord.Equatorial(
            this.checkRadBounds(base.toRad(coords.ra)),
            this.checkRadBounds(base.toRad(coords.dec))
        );

        let siderealTime = sidereal.apparent(state.jde);
        let altaz = eqCoord.toHorizontal(
            new globe.Coord(state.gps.lat, state.gps.lon),
            siderealTime
        );
        return {
            az: base.toDeg(altaz.az),
            alt: base.toDeg(altaz.alt),
        };
    },
    convertAzAlt2RADec: function(state, coords) {
        let eqCoord = new coord.Horizontal(
            this.checkRadBounds(base.toRad(coords.az)),
            this.checkRadBounds(base.toRad(coords.alt))
        );

        let siderealTime = sidereal.apparent(state.jde);
        let radec = eqCoord.toEquatorial(
            new globe.Coord(state.gps.lat, state.gps.lon),
            siderealTime
        );
        return {
            ra: base.toDeg(radec.ra),
            dec: base.toDeg(radec.dec),
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
