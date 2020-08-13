/* $Id: init.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/
/*eslint new-cap: ["error", { "newIsCap": false }]*/

import {iface} from '../../src/iface'
import {StateMachine} from './statemachine'

import { createEmptyConf } from '../../src/conf'
import getLocInfo from '../../src/location'

var cpp: iface
var fsm: StateMachine
var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts

export function initInit( that: StateMachine ) {
    console.log('racedashstart init initInit()')
    fsm = that
    cpp = (window as any).iface
    that.locInfo = getLocInfo()
    that.conf = createEmptyConf()
}
