/* $Id: disp.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to handle the display object(s)
import { createGauge } from './gauge'
import { hasProportionalFontSupport } from './css'
import { showData } from './data'
import { memorizeSettings } from '../../src/conf'

export function swapDisplay( that, direction, memorize ) {
    var direction = direction || 'up'
    var memorizeChange = memorize || true
    if ( (that.gauge.length > 0) ) {
        var removedgauge = that.gauge.pop()
        removedgauge.destroy()
    }
    var nextdisp = 'dial'
    if ( direction === 'up' ) {
        if ( that.conf.display == 'dial' )
            nextdisp = 'donut'
        else if ( that.conf.display == 'donut' )
            nextdisp = 'simple'
    }
    else {
        if ( that.conf.display == 'dial' )
            nextdisp = 'simple'
        else if ( that.conf.display == 'simple' )
            nextdisp = 'donut'
    }
    
    if ( nextdisp == 'simple' ) {
        document.getElementById('gauge0').innerHTML =
            '<div class="numgauge ' +
            (hasProportionalFontSupport?'propl ':'fixed ') +
            that.luminosity +
            '" id="numgauge0"></div>' +
            '<div class="numgunit ' +
            (hasProportionalFontSupport?'propl ':'fixed ') +
            that.luminosity +
            '" id="numgunit0"></div>'
    }
    else {
        document.getElementById('gauge0').innerHTML = ''
        if (nextdisp == 'dial') {
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
}
