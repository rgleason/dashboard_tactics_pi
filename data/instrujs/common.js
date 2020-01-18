/* $Id: common.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in / WebKit based instruments
 * Licensed under MIT - see distribution.
 */
/* >>>> If you plan to modify this file, please make a backup first! <<<< */
/* ---- Find a path missing? Contribute/report https://git.io/JejKQ  ---- */

var  instrustat = {
    theme : 'default',
    debuglevel : 3,
    alerts : true,
    alertdelay : 3,
    knownpaths: [
        {
            version    : 1,
            path       : 'propulsion.port.revolutions',
            title      : 'Engine Speed',
            unit       : 'r.p.m.',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 3000,
            multiplier : 60,
            divider    : 1,
            offset     : 0
        }, 
        {
            version    : 1,
            path       : 'propulsion.starboard.revolutions',
            title      : 'Stbd Engine Speed',
            unit       : 'r.p.m.',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 3000,
            multiplier : 60,
            divider    : 1,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.port.temperature',
            title      : 'Engine Temperature',
            unit       : 'Celcius',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 95,
            maxval     : 100,
            multiplier : 1,
            divider    : 1,
            offset     : -273.2
        },
        {
            version    : 1,
            path       : 'propulsion.starboard.temperature',
            title      : 'Stbd Engine Temperature',
            unit       : 'Celcius',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 95,
            maxval     : 100,
            multiplier : 1,
            divider    : 1,
            offset     : -273.2
        },
        {
            version    : 1,
            path       : 'propulsion.port.oilPressure',
            title      : 'Oil Pressure',
            unit       : 'bar',
            decimals   : 1,
            minval     : 0,
            loalert    : 1,
            hialert    : 0,
            maxval     : 4,
            multiplier : 1,
            divider    : 100000,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.starboard.oilPressure',
            title      : 'Stbd Oil Pressure',
            unit       : 'bar',
            decimals   : 1,
            minval     : 0,
            loalert    : 1,
            hialert    : 0,
            maxval     : 4,
            multiplier : 1,
            divider    : 100000,
            offset     : 0
        },
    ],
    // --- Do not modify below this line ---
    skpathlookup: function ( path ) {
        for ( var i = 0; i < this.knownpaths.length; i++  ) {
            if ( path === this.knownpaths[ i ].path ) {
                return {
                    version    : this.knownpaths[ i ].version,
                    path       : this.knownpaths[ i ].path,
                    title      : this.knownpaths[ i ].title,
                    unit       : this.knownpaths[ i ].unit,
                    decimals   : this.knownpaths[ i ].decimals,
                    minval     : this.knownpaths[ i ].minval,
                    loalert    : this.knownpaths[ i ].loalert,
                    hialert    : this.knownpaths[ i ].hialert,
                    maxval     : this.knownpaths[ i ].maxval,
                    multiplier : this.knownpaths[ i ].multiplier,
                    divider    : this.knownpaths[ i ].divider,
                    offset     : this.knownpaths[ i ].offset
                }
            }                
        }
        return null
    }
}
window.instrustat = instrustat
