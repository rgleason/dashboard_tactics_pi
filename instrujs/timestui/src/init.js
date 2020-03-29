/* $Id: init.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to init supporting members

import getLocInfo from '../../src/location'
import { startTimesTuiChart } from './chart'
import { setSkPathFontResizingStyle } from './css'
import { createEmptyConf } from '../../src/conf'

export function initLoad( that ) {
    that.locInfo = getLocInfo()
    startTimesTuiChart()
    that.glastvalue[0] = 0
    setSkPathFontResizingStyle()
    that.conf = createEmptyConf()
}
