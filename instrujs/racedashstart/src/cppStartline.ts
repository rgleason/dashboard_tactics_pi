/* $Id: cppcomm.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/

import {iface} from '../../src/iface'
import {StateMachine} from './statemachine'

var cpp: iface
var fsm: StateMachine
var dbglevel: number = (window as any).instrustat.debuglevel

export function initCppComm( that: StateMachine ) {
    console.log('racedashstart ccpStartline initCppComm()')
    fsm = that
    cpp = (window as any).iface
}

export function cppCheckForUserStartline() {
    console.log( 'cppStartline - cppCheckForUserStartline()' )
    cpp.setFlag( 'bottom', 'getusrsl' )
}
export function cppNoUserStartline() {
    fsm.gotusrsl = false
    console.log( 'cppStartline - cppNoUserStartline(), fsm.gotusrsl: ', fsm.gotusrsl )
    cpp.clearFlagById( 'bottom' )
    fsm.stbdmark = false
    fsm.portmark = false
}
export function cppUserStartline() {
    fsm.gotusrsl = true
    console.log( 'cppStartline - cppUserStartline(), fsm.gotusrsl: ', fsm.gotusrsl )
    cpp.clearFlagById( 'bottom' )
    fsm.stbdmark = true
    fsm.portmark = true
}

export function cppDropStbdMark() {
    console.log( 'cppStartline - cppDropStbdMark()' )
    cpp.setFlag( 'bottom', 'dropstbd' )
}
export function cppAckStbdMark() {
    fsm.stbdmark = true
    console.log( 'cppStartline - cppAckStbdMark()', fsm.stbdmark )
    cpp.clearFlagById( 'bottom' )
}
export function cppDropPortMark() {
    console.log( 'cppStartline - cppDropPortMark()' )
    cpp.setFlag( 'bottom', 'dropport' )
}
export function cppAckPortMark() {
    fsm.portmark = true
    console.log( 'cppStartline - cppAckPortMark()', fsm.portmark )
    cpp.clearFlagById( 'bottom' )
}

export function cppGetData() {
    console.log( 'cppStartline - cppGetData()' )
    cpp.setFlag( 'bottom', 'getsldata' )
}
export function cppAckGetData() {
    console.log( 'cppStartline - cppAckGetData()' )
    cpp.clearFlagById( 'bottom' )
}
export function cppStopData() {
    console.log( 'cppStartline - cppStopData()' )
    cpp.setFlag( 'bottom', 'stopsldata' )
}
export function cppAckStopData() {
    console.log( 'cppStartline - cppAckStopData()' )
    cpp.clearFlagById( 'bottom' )
}
