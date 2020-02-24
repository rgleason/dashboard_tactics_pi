/* $Id: common.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in / WebKit based instruments
 * Licensed under MIT - see distribution.
 */
/* >>>> If you plan to modify this file, please make a backup first! <<<< */
/* ---- Find a path missing?
 Contribute/report here, please: https://git.io/JejKQ
 - with a screenshot and a short description of your installation, thanks!
 SignalK Path keys: https://git.io/JvsYw
 The Signal K values are always in SI units (like m/s, not knots).
 Conversion to a wanted unit is made with multipier/division/offset.
 Avoid using floating point values like 0.000000003 in JavaScript!
 UTF8 - do _not_ change encoding, cf. degree character. Notepad++ recommended.
 Usage example: enginedjg/index.html loads a minimized version, common.min.js
        - make a copy of common.min.js by renaming it;
        - make a copy of this one with name common.min.js and modify it;
        - (no need for compression with this non-executing file!)
        - or, modify enginedjg/index.html to load your own file, no problem!
        - issues? open the index.html in a browser, hit Shift+Ctrl+I and reload;
                  * Console gives you the reason why it does not load anymore:
                  * Look for messages in red, a typo, missing comma?
        - note: next update/reinstallation overrides this file, keep backups!
*/

var  instrustat = {
    theme : 'default',
    debuglevel : 4,
    alerts : true,
    alertdelay : 5,
    knownpaths: [
        {
            version    : 1,
            path       : 'electrical.batteries.*.current',
            title      : 'Battery Current',
            symbol     : '',
            unit       : 'Amps',
            display    : 'dial',
            decimals   : 1,
            minval     : -20,
            loalert    : 0,
            hialert    : 0,
            maxval     : 20,
            multiplier : 1,
            divider    : 1,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'electrical.batteries.*.temperature',
            title      : 'Battery Temperature',
            symbol     : '°',
            unit       : 'Celsius',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 100,
            multiplier : 1,
            divider    : 1,
            offset     : -273.2
        },
        {
            version    : 1,
            path       : 'electrical.batteries.*.voltage',
            title      : 'Battery Voltage',
            symbol     : '',
            unit       : 'Volts',
            display    : 'dial',
            decimals   : 1,
            minval     : 0,
            loalert    : 11,
            hialert    : 0,
            maxval     : 16,
            multiplier : 1,
            divider    : 1,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.alternatorVoltage',
            title      : 'Alternator Voltage',
            symbol     : '',
            unit       : 'Volts',
            display    : 'dial',
            decimals   : 1,
            minval     : 0,
            loalert    : 11,
            hialert    : 0,
            maxval     : 16,
            multiplier : 1,
            divider    : 1,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.boostPressure',
            title      : 'Boost Pressure',
            symbol     : '',
            unit       : 'kPa',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 100,
            multiplier : 1,
            divider    : 100000,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.coolantPressure',
            title      : 'Coolant Pressure',
            symbol     : '',
            unit       : 'kPa',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 100,
            multiplier : 1,
            divider    : 1000000,
            offset     : 0
        },
/*  ***
    Testing revealed an anomaly between negative and postive values
    in propulsion.*.drive.trimState path:
    - 0.01 scaled when positive, but no scaling when negative
    - fixed in instrujs.cpp data callback by scaling also negative
      values by 0.01, consequently below multiplier 100 works both
    If you observe wrong behaviour, report to https://git.io/JejKQ
    *** */
        {
            version    : 1,
            path       : 'propulsion.*.drive.trimState',
            title      : 'Engine Drive Trim',
            symbol     : '%',
            unit       : 'ratio',
            display    : 'dial',
            decimals   : 0,
            minval     : -100,
            loalert    : 0,
            hialert    : 0,
            maxval     : 100,
            multiplier : 100,
            divider    : 1,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.coolantTemperature',
            title      : 'Coolant Temperature',
            symbol     : '°',
            unit       : 'Celsius',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 95,
            maxval     : 110,
            multiplier : 1,
            divider    : 1,
            offset     : -273.2
        },
        {
            version    : 1,
            path       : 'propulsion.*.engineLoad',
            title      : 'Engine Load',
            symbol     : '%',
            unit       : 'ratio',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 100,
            multiplier : 100,
            divider    : 1,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.engineTorque',
            title      : 'Engine Torque',
            symbol     : '%',
            unit       : 'ratio',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 125,
            multiplier : 100,
            divider    : 1,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.exhaustTemperature',
            title      : 'Exhaust Temperature',
            symbol     : '°',
            unit       : 'Celsius',
            display    : 'dial',
            decimals   : 0,
            minval     : 200,
            loalert    : 0,
            hialert    : 0,
            maxval     : 800,
            multiplier : 1,
            divider    : 1,
            offset     : -273.2
        },
        {
            version    : 1,
            path       : 'propulsion.*.fuel.pressure',
            title      : 'Fuel Pressure',
            symbol     : '',
            unit       : 'kPa',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 100,
            multiplier : 1,
            divider    : 1000,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.fuel.rate',
            title      : 'Fuel Rate',
            symbol     : '',
            unit       : 'l/h',
            display    : 'dial',
            decimals   : 1,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 50,
            multiplier : 3600000,
            divider    : 1,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.oilPressure',
            title      : 'Oil Pressure',
            symbol     : '',
            unit       : 'kPa',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 100,
            hialert    : 0,
            maxval     : 400,
            multiplier : 1,
            divider    : 1000,
            offset     : 0
        },
        {
            version    : 1,
            path       : 'propulsion.*.oilTemperature',
            title      : 'Engine Oil Temperature',
            symbol     : '°',
            unit       : 'Celsius',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 110,
            maxval     : 130,
            multiplier : 1,
            divider    : 1,
            offset     : -273.2
        },
        {
            version    : 1,
            path       : 'propulsion.*.revolutions',
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
/*  ***
    Did not pass tests for distribution, please feel free to test
    and report if you get this path working to https://git.io/JejKQ
        {
            version    : 1,
            path       : 'propulsion.*.runTime',
            title      : 'Engine Run Time',
            symbol     : '',
            unit       : 'hours',
            display    : 'simple',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 10000,
            multiplier : 3600,
            divider    : 1,
            offset     : 0
        },
 *** */
        {
            version    : 1,
            path       : 'propulsion.*.temperature',
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
            path       : 'tanks.fuel.*.currentLevel',
            title      : 'Fuel Level',
            symbol     : '%',
            unit       : 'ratio',
            display    : 'dial',
            decimals   : 0,
            minval     : 0,
            loalert    : 0,
            hialert    : 0,
            maxval     : 100,
            multiplier : 100,
            divider    : 1,
            offset     : 0
        },
    ],
    // --- Do not modify below this line ---
    hasPathEntry: function ( path ) {
        var paths  = path.split('.')
        for ( var i = 0; i < this.knownpaths.length; i++  ) {
            var pathk = this.knownpaths[ parseInt(i) ].path
            var pathks = pathk.split('.')
            if ( paths.length !== pathks.length )
                continue
            var bAllMatch = true
            for ( var j = 0; j < pathks.length; j++ )
                if ( pathks[parseInt(j)] !== '*' )
                    if ( pathks[parseInt(j)] !== paths[parseInt(j)] ) {
                        bAllMatch = false
                        continue
                    }
            if ( bAllMatch )
                return parseInt(i)
        }
        return -1
    },
    skpathlookup: function ( path ) {
        var i = this.hasPathEntry( path )
        if ( i < 0 )
            return null
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
window.instrustat = instrustat
