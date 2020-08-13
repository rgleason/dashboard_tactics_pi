/* $Id: getid.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to ask and retrieve from a client a unique ID for this instance

/*eslint camelcase: ['error', {'properties': 'never'}]*/
/*eslint new-cap: ["error", { "newIsCap": false }]*/

import {StateMachine} from './statemachine'

var dbglevel: number = (window as any).instrustat.debuglevel

export function getidAskClient() {
    (window as any).iface.setFlag('bottom', 'getid')
}

export function getidClientAnswer( that: StateMachine ) {
    (window as any).iface.clearFlagById( 'bottom' )
    that.uid = (window as any).iface.getid()
    if ( dbglevel > 1 ) console.log('getidClientAnswer(): that.uid : ', that.uid )
}
