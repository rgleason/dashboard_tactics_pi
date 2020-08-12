/* $Id: css.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import { hasProportionalFontSupport } from '../../src/css'

// Please refer to local ../sass/style.scss

export function getNewLuminosity( that ) {

    var newluminosity = window.iface.getluminsty()

    if ( (newluminosity === 'day') ||
         (newluminosity === 'dusk') ||
         (newluminosity === 'night') ) {

        that.luminosity  = newluminosity

        var newclass = 'instrument ' + newluminosity
        var elem = document.getElementById('instrument')
        var oldclass = elem.className
        if ( !(newclass === oldclass) )
            elem.className = newclass
        document.getElementById('bottom').className = 'bottom ' + that.luminosity
    }
    return
}
