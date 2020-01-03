/* $Id: common.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in / WebKit based instruments
 * Licensed under MIT - see distribution.
 */

const instrustat = {
    skin : 'default',
    skpathlookup: function ( skpath ) {
        for ( var i = 0; i < this.knownskpaths.length; i++  ) {
            if ( skpath === this.knownskpaths[ i ] ) {
                return {
                    path: this.knownskpaths[ i ].path,
                    ttle: this.knownskpaths[ i ].ttle,
                    unit: this.knownskpaths[ i ].unit,
                    mltp: this.knownskpaths[ i ].mltp,
                    divs: this.knownskpaths[ i ].divs,
                    offs: this.knownskpaths[ i ].offs
                }
            }                
        }
    },
    knownskpaths: [
        {
            path: 'propulsion.port.revolutions',
            ttle: 'Engine Speed',
            unit: 'r.p.m.',
            mltp: 60,
            divs: 1,
            offs: 0
        }, 
        {
            path: 'propulsion.starboard.revolutions',
            ttle: 'Stbd Engine Speed',
            unit: 'r.p.m.',
            mltp: 60,
            divs: 1,
            offs: 0
        },
       {
           path: 'propulsion.port.temperature',
           ttle: 'Engine Temperature',
           unit: 'Celcius',
           mltp: 1,
           divs: 1,
           offs: -273.2
       },
       {
           path: 'propulsion.starboard.temperature',
           ttle: 'Stbd Engine Temperature',
           unit: 'Celcius',
           mltp: 1,
           divs: 1,
           offs: -273.2
       },
       {
           path: 'propulsion.port.oilPressure',
           ttle: 'Oil Pressure',
           unit: 'bar',
           mltp: 1,
           divs: 100000,
           offs: 0
       },
       {
           path: 'propulsion.starboard.oilPressure',
           ttle: 'Stbd Oil Pressure',
           unit: 'bar',
           mltp: 1,
           divs: 100000,
           offs: 0
       },
    ]
}
window.instrustat = instrustat
