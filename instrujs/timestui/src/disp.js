/* $Id: disp.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to handle the display object(s)

import sanitizer from '../../src/escapeHTML'
var Sanitizer = sanitizer()

// import { createGauge } from './gauge'
import { hasProportionalFontSupport } from './css'
import { showData } from './data'
import { memorizeSettings } from '../../src/conf'

export function swapDisplay( that, direction, memorize ) {
/*
    var dir = direction || 'up'
    var memorizeChange = memorize || true
    if ( (that.gauge.length > 0) ) {
        var removedgauge = that.gauge.pop()
        removedgauge.destroy()
    }
    var nextdisp = 'dial'
    if ( dir === 'up' ) {
        if ( that.conf.display === 'dial' )
            nextdisp = 'donut'
        else if ( that.conf.display === 'donut' )
            nextdisp = 'simple'
    }
    else {
        if ( that.conf.display === 'dial' )
            nextdisp = 'simple'
        else if ( that.conf.display === 'simple' )
            nextdisp = 'donut'
    }

    if ( nextdisp === 'simple' ) {
        var htmlCandidate =
            '<div class="numgauge ' +
            (hasProportionalFontSupport?'propl ':'fixed ') +
            that.luminosity +
            '" id="numgauge0"></div>' +
            '<div class="numgunit ' +
            (hasProportionalFontSupport?'propl ':'fixed ') +
            that.luminosity +
            '" id="numgunit0"></div>'
        var htmlObj = Sanitizer.createSafeHTML(htmlCandidate)
        document.getElementById('gauge0').innerHTML =
            Sanitizer.unwrapSafeHTML(htmlObj)
    }
    else {
        document.getElementById('gauge0').innerHTML = ''
        if (nextdisp === 'dial') {
            that.gauge.push (
                createGauge('gauge0', 0, that.conf.decimals,
                            that.conf.unit, false, that.conf.symbol ) )
        }
        else {
            that.gauge.push (
                createGauge('gauge0', 0, that.conf.decimals,
                            that.conf.unit, true, that.conf.symbol ) )
        }
    }

    showData( that )
    that.conf.display = nextdisp
    if ( memorizeChange )
        memorizeSettings( that )
    return
*/
}

export function rollDisplayToSelection( that ) {
    if ( that.conf.display === 'dial' ) {
        swapDisplay( that, 'down', false ) // re-init
        swapDisplay( that, 'down', false )
        swapDisplay( that, 'down', false )
    }
    else if ( that.conf.display === 'simple' ) {
        that.conf.display = 'dial'
        swapDisplay( that, 'down', true )
    }
    else {
        that.conf.display = 'dial'
        swapDisplay( that, 'up', true )
    }
}
