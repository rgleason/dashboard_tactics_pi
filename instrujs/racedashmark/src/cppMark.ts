/* $Id: cppMark.ts, v1.0 2019/11/30 VaderDarth Exp $
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

export function instruNotRdyAlert() {
    if ( alerts )
        alert ( (window as any).instrulang.alertTitle + '\n' + '\n' +
                (window as any).instrulang.rdmInstruNotRdy + '\n' + '\n' +
                (window as any).instrulang.rdmCheckRequiredData + '\n' +
                (window as any).instrulang.rdmCheckAvgInstru )
}

export function cppGetData() {
    console.log( 'cppMark - cppGetData()' )
    cpp.setFlag( 'bottom', 'getmrkdata' )
}
export function cppAckGetData() {
    cpp.clearFlagById( 'bottom' )
    console.log( 'cppMark - cppAckGetData()' )
}
export function cppMuteChart() {
    console.log( 'cppMark - cppMuteChart()' )
    cpp.setFlag( 'bottom', 'mutechart' )
}
export function cppAckMuteChart() {
    cpp.clearFlagById( 'bottom' )
    console.log( 'cppMark - cppAckMuteChart()' )
}
export function cppResumeChart() {
    console.log( 'cppMark - cppResumeChart()' )
    cpp.setFlag( 'bottom', 'resumechart' )
}
export function cppAckResumeChart() {
    cpp.clearFlagById( 'bottom' )
    console.log( 'cppMark - cppAckResumeChart()' )
}
