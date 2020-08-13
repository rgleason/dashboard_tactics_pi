/* $Id: init.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to init supporting members

import getLocInfo from '../../src/location'
import { createGauge } from './gauge'
import { setSkPathFontResizingStyle } from '../../src/css'
import { createEmptyConf } from '../../src/conf'

export function initLoad( that ) {
    that.locInfo = getLocInfo()
    that.gauge.push ( createGauge('gauge0', 0, 1, '[init]', false, '') )
    that.glastvalue[0] = 0
    setSkPathFontResizingStyle()
    that.conf = createEmptyConf()
}
