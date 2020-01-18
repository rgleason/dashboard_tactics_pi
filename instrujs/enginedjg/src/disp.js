/* $Id: disp.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to handle the display object(s)

import { createGauge } from './gauge'
import { hasProportionalFontSupport } from './css'
import { showData } from './data'
import { memorizeSettings } from '../../src/conf'

export function swapDisplay( that, memorize ) {
    var memorizeChange = memorize || true
    if ( (that.gauge.length > 0) ) {
        var removedgauge = that.gauge.pop()
        removedgauge.destroy()
        document.getElementById('gauge0').innerHTML =
            '<div class="numgauge ' +
            (hasProportionalFontSupport?'propl ':'fixed ') +
            that.luminosity +
            '" id="numgauge0"></div>' +
            '<div class="numgunit ' +
            (hasProportionalFontSupport?'propl ':'fixed ') +
            that.luminosity +
            '" id="numgunit0"></div>'
        showData( that )
        that.conf.display = 'simple'
        if ( memorizeChange )
            memorizeSettings( that )
    }
    else {
        document.getElementById('gauge0').innerHTML = ''
        that.gauge.push (
            createGauge('gauge0', 0, that.conf.decimals, that.conf.unit ) )
        showData( that )
        that.conf.display = 'dial180'
        if ( memorizeChange )
            memorizeSettings( that )
    }
}
