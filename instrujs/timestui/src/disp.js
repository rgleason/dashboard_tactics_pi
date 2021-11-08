/* $Id: disp.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to handle the display object(s)

import sanitizer from '../../src/escapeHTML'
var Sanitizer = sanitizer()

// import { createGauge } from './gauge'
import { hasProportionalFontSupport } from '../../src/css'
import { showData } from './data'
import { memorizeSettings } from '../../src/conf'

export function swapDisplay( that, direction, memorize ) {
    return // so far no alternative display format
}

export function rollDisplayToSelection( that ) {
    return // so fa no alternative display format
}
