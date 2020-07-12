/* $Id: linedata.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/

import sanitizer from '../../src/escapeHTML'
var Sanitizer = new sanitizer()

import {iface} from '../../src/iface'
import {StateMachine} from './statemachine'

var cpp: iface
var fsm: StateMachine
var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts
var rdsfeet: boolean = (window as any).instrustat.rdsfeet

var locstate: string = ''

const elemPnlDistLine = '<div id="pnlDistLine" class="panel panel-default day">' +
'<div id="pnlDistLineHdr" class="panel-heading text-center day"></div>' +
'<b><div id="pnlDistLineBdy" class="panel-body text-center day"></div></b>' +
'</div>'
var htmlPnlDistLine = Sanitizer.createSafeHTML(elemPnlDistLine)

const elemPnlDistAbs = '<div id="pnlDistAbs" class="panel panel-default day">' +
'<div id="pnlDistAbsHdr" class="panel-heading text-center day"></div>' +
'<b><div id="pnlDistAbsBdy" class="panel-body text-center day"></div></b>' +
'</div>'
var htmlPnlDistAbs = Sanitizer.createSafeHTML(elemPnlDistAbs)

const elemPnlBiasValue = '<div id="pnlPort" class="panel panel-default day">' +
'<div id="pnlBiasValueHdr" class="panel-heading text-center day"></div>' +
'<b><div id="pnlBiasValueBdy" class="panel-body text-center day"></div></b>' +
'</div>'
var htmlPnlBiasValue = Sanitizer.createSafeHTML(elemPnlBiasValue)

const elemPnlBiasDist = '<div id="pnlStarboard" class="panel panel-default day">' +
'<div id="pnlBiasDistHdr" class="panel-heading text-center day"></div>' +
'<b><div id="pnlBiasDistBdy" class="panel-body text-center day"></div></b>' +
'</div>'
var htmlPnlBiasDist = Sanitizer.createSafeHTML(elemPnlBiasDist)

export function initLineData( that: StateMachine ) {
    console.log('racedashstart linedata initLineData()')
    fsm = that
    cpp = (window as any).iface
    locstate = 'READY'
}

export function armedLineData( ) {
    console.log('racedashstart linedata armedLineData()')

    $('#grdDistLine').html( Sanitizer.unwrapSafeHTML(htmlPnlDistLine) )
    $('#pnlDistLineHdr').text( (window as any).instrulang.rdsDistLine )
    $('#pnlDistLineBdy').text( '- - -' + (rdsfeet?' feet':' meters') )

    $('#grdDistAbs').html( Sanitizer.unwrapSafeHTML(htmlPnlDistAbs) )
    $('#pnlDistAbsHdr').text( (window as any).instrulang.rdsDistAbs )
    $('#pnlDistAbsBdy').text( '- - -' + (rdsfeet?' feet':' meters') )

    $('#grdPort').html( Sanitizer.unwrapSafeHTML(htmlPnlBiasValue) )
    $('#pnlBiasValueHdr').text( (window as any).instrulang.rdsWindBias )
    $('#pnlBiasValueBdy').text( '- - -' )

    $('#grdStarboard').html( Sanitizer.unwrapSafeHTML(htmlPnlBiasDist) )
    $('#pnlBiasDistHdr').text( (window as any).instrulang.rdswindBiasAdv )
    $('#pnlBiasDistBdy').text( '- - -' + (rdsfeet?' feet':' meters') )

    locstate = 'ARMED'
}

function fadeAwayAllDataPanels() {
    console.log('racedashstart linedata fadeAwayAllDataPanels()')
    $('#grdPort').animate({
         opacity: 0.25
    }, 800, function() { $(this).text( '' ) })
    $('#grdStarboard').animate({
         opacity: 0.25
    }, 850, function() { $(this).text( '' ) })
    $('#grdDistLine').animate({
         opacity: 0.25
    }, 900, function() { $(this).text( '' ) })
    $('#grdDistAbs').animate({
         opacity: 0.25
    }, 1000, function() { $(this).text( '' ) })
}

export function newLineData() {
    console.log('racedashstart linedata newLineData()')

    var distanceToGoStr: string = '- - -'
    var distanceToGo: number = cpp.getsldistancetogo()
    if ( distanceToGo >= 0.0 ) {
        (rdsfeet ? (distanceToGo *= 6076.12) : (distanceToGo *= 1852.0))
        distanceToGoStr = distanceToGo.toFixed(0)
    }
    $('#pnlDistLineBdy').text( distanceToGoStr + (rdsfeet?' feet':' meters') )

    var distanceToGoAbsStr: string = '- - -'
    var distanceToGoAbs: number = cpp.getslclosestpoint()
    if ( distanceToGoAbs >= 0.0 ) {
        (rdsfeet ? (distanceToGoAbs *= 6076.12) : (distanceToGoAbs *= 1852.0))
        distanceToGoAbsStr = distanceToGoAbs.toFixed(0)
    }
    $('#pnlDistAbsBdy').text( distanceToGoAbsStr + (rdsfeet?' feet':' meters') )

    var biasStr: string = '- - -'
    var bias: number = cpp.getslwindbias()
    if ( bias >= 0.0 )
        biasStr = bias.toFixed(0)
    $('#pnlBiasValueBdy').text( biasStr )

    var biasDistanceStr: string = '- - -'
    var biasDistance: number = cpp.getsladvantage()
    if ( biasDistance >= 0.0 ) {
        (rdsfeet ? (biasDistance *= 6076.12) : (biasDistance *= 1852.0))
        biasDistanceStr = biasDistance.toFixed(0)
    }
    $('#pnlBiasDistBdy').text( biasDistanceStr + (rdsfeet?' feet':' meters') )

}


export function quitLineData( that: StateMachine ) {
    console.log('racedashstart linedata quitLineData()')
    fadeAwayAllDataPanels()
    $('#grdDistAbs').promise().done(function() {
        console.log('racedashstart linedata quitLineData(): fading over')
        $("#grdPort").css({ opacity: 1.0 })
        $("#grdPort").empty()
        $("#grdStarboard").css({ opacity: 1.0 })
        $("#grdStarboard").empty()
        $("#grdDistLine").css({ opacity: 1.0 })
        $("#grdDistLine").empty()
        $("#grdDistAbs").css({ opacity: 1.0 })
        $("#grdDistAbs").empty()
        initLineData( that )
    })
}
