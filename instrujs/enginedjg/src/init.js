/* $Id: init.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to init supporting members

import getLocInfo from '../../src/location'
import { createGauge } from './gauge'
import { setSkPathFontResizingStyle } from './css'
import { createEmptyConf } from './conf'

export function initLoad( that ) {
    that.locInfo = getLocInfo()
    that.gauge.push ( createGauge('gauge0', 0, 1, '[init]') )
    setSkPathFontResizingStyle()
    that.conf = createEmptyConf()
}

