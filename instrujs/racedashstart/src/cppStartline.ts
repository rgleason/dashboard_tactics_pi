/* $Id: cppcomm.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/
/*eslint new-cap: ["error", { "newIsCap": false }]*/

import {iface} from '../../src/iface'
import {StateMachine} from './statemachine'

var cpp: iface
var fsm: StateMachine
var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts

export function initCppComm( that: StateMachine ) {
    console.log('racedashstart ccpStartline initCppComm()')
    fsm = that
    cpp = (window as any).iface
}

function instruNotRdyAlert() {
    if ( alerts )
        alert ( (window as any).instrulang.alertTitle + '\n' + '\n' +
                (window as any).instrulang.rdsInstruNotRdy + '\n' + '\n' +
                (window as any).instrulang.rdsCheckRequiredData + '\n' +
                (window as any).instrulang.rdsCheckAvgInstru )
}


export function cppGetIsDistanceUnitFeet() {
    console.log( 'cppStartline - cppGetIsDistanceUnitFeet()' )
    cpp.setFlag( 'bottom', 'getdisf' )
}
export function cppInstruNoDistFeet() {
    cpp.clearFlagById( 'bottom' )
    fsm.feet = false
    console.log( 'cppStartline - cppInstruNoDistFeet(), fsm.feet: ', fsm.feet )
}
export function cppInstruDistFeet() {
    cpp.clearFlagById( 'bottom' )
    fsm.feet = true
    console.log( 'cppStartline - cppInstruDistFeet(), fsm.feet: ', fsm.feet )
}

export function cppCheckForInstruRdy() {
    console.log( 'cppStartline - cppCheckForInstruRdy()' )
    cpp.setFlag( 'bottom', 'getrdy' )
}
export function cppInstruNotRdy() {
    cpp.clearFlagById( 'bottom' )
    fsm.instrurdy = false
    console.log( 'cppStartline - cppInstruNotRdy(), fsm.instrurdy: ', fsm.instrurdy )
    instruNotRdyAlert()
}
export function cppInstruRdy() {
    cpp.clearFlagById( 'bottom' )
    fsm.instrurdy = true
    console.log( 'cppStartline - cppInstruRdy(), fsm.instrurdy: ', fsm.instrurdy )
}

export function cppCheckForUserStartline() {
    console.log( 'cppStartline - cppCheckForUserStartline()' )
    cpp.setFlag( 'bottom', 'getusrsl' )
}
export function cppNoUserStartline() {
    cpp.clearFlagById( 'bottom' )
    fsm.gotusrsl = false
    console.log( 'cppStartline - cppNoUserStartline(), fsm.gotusrsl: ', fsm.gotusrsl )
    fsm.stbdmark = false
    fsm.portmark = false
}
export function cppUserStartline() {
    cpp.clearFlagById( 'bottom' )
    fsm.gotusrsl = true
    console.log( 'cppStartline - cppUserStartline(), fsm.gotusrsl: ', fsm.gotusrsl )
    fsm.stbdmark = true
    fsm.portmark = true
}

function checkMarkAck(): boolean {
    var successMark: boolean = cpp.getmarkack()
    if ( !successMark )
        instruNotRdyAlert()
    return successMark
}

export function cppDropStbdMark() {
    console.log( 'cppStartline - cppDropStbdMark()' )
    cpp.setFlag( 'bottom', 'dropstbd' )
}
export function cppAckStbdMark() {
    cpp.clearFlagById( 'bottom' )
    fsm.stbdmark = checkMarkAck()
    console.log( 'cppStartline - cppAckStbdMark()', fsm.stbdmark )
}
export function cppDropPortMark() {
    console.log( 'cppStartline - cppDropPortMark()' )
    cpp.setFlag( 'bottom', 'dropport' )
}
export function cppAckPortMark() {
    cpp.clearFlagById( 'bottom' )
    fsm.portmark = checkMarkAck()
    console.log( 'cppStartline - cppAckPortMark()', fsm.portmark )
}

export function cppGetData() {
    console.log( 'cppStartline - cppGetData()' )
    cpp.setFlag( 'bottom', 'getsldata' )
}
export function cppAckGetData() {
    cpp.clearFlagById( 'bottom' )
    console.log( 'cppStartline - cppAckGetData()' )
}
export function cppStopData() {
    console.log( 'cppStartline - cppStopData()' )
    cpp.setFlag( 'bottom', 'stopsldata' )
}
export function cppAckStopData() {
    cpp.clearFlagById( 'bottom' )
    console.log( 'cppStartline - cppAckStopData()' )
}
