/* $Id: common.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in / WebKit based instruments
 * Licensed under MIT - see distribution.
 */
/* >>>> If you plan to modify this file, please make a backup first! <<<< */
/* ---- Find a path missing? Contribute/report https://git.io/JejKQ  ---- */

var  instrustat = {
    theme : 'default',
    debuglevel : 4,
    alerts : true,
    alertdelay : 5,
    knownpaths: [
        {
            version    : 1,
            path       : 'propulsion.port.revolutions',
            title      : 'Engine Speed',
            symbol     : '',
            unit       : 'r.p.m.',
            display    : 'dial',
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
            symbol     : '',
            unit       : 'r.p.m.',
            display    : 'dial',
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
            symbol     : '°',
            unit       : 'Celsius',
            display    : 'dial',
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
            symbol     : '°',
            unit       : 'Celsius',
            display    : 'dial',
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
            symbol     : '',
            unit       : 'bar',
            display    : 'dial',
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
            symbol     : '',
            unit       : 'bar',
            display    : 'dial',
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
            if ( path === this.knownpaths[ parseInt(i) ].path ) {
                return {
                    version    : this.knownpaths[ parseInt(i) ].version,
                    path       : this.knownpaths[ parseInt(i) ].path,
                    title      : this.knownpaths[ parseInt(i) ].title,
                    symbol     : this.knownpaths[ parseInt(i) ].symbol,
                    unit       : this.knownpaths[ parseInt(i) ].unit,
                    display    : this.knownpaths[ parseInt(i) ].display,
                    decimals   : this.knownpaths[ parseInt(i) ].decimals,
                    minval     : this.knownpaths[ parseInt(i) ].minval,
                    loalert    : this.knownpaths[ parseInt(i) ].loalert,
                    hialert    : this.knownpaths[ parseInt(i) ].hialert,
                    maxval     : this.knownpaths[ parseInt(i) ].maxval,
                    multiplier : this.knownpaths[ parseInt(i) ].multiplier,
                    divider    : this.knownpaths[ parseInt(i) ].divider,
                    offset     : this.knownpaths[ parseInt(i) ].offset
                }
            }                
        }
        return null
    }
}
window.instrustat = instrustat
