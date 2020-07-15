/* $Id: linedata.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/
/*eslint new-cap: ["error", { "newIsCap": false }]*/

import sanitizer from '../../src/escapeHTML'
var Sanitizer = new sanitizer()

import {iface} from '../../src/iface'
import {StateMachine} from './statemachine'

var cpp: iface
var fsm: StateMachine
var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts

var locstate: string = ''

export function initLineData( that: StateMachine ) {
    console.log('racedashstart linedata initLineData()')
    fsm = that
    cpp = (window as any).iface
    locstate = 'READY'
}

const elemPnlDistLine = '<div id="pnlDistLine" class="panel panel-default day">' +
'<div id="pnlDistLineHdr" class="panel-heading text-center day"></div>' +
'<b><div id="pnlDistLineBdy" class="panel-body text-center day"></div></b>' +
'</div>'
var htmlPnlDistLine = Sanitizer.createSafeHTML(elemPnlDistLine)
const elemDistLinePopover = '<a id="distLinePop" href="#" role="button" ' +
'tabindex="0" data-toggle="popover0" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdsDistLine + '" ' +
'data-content="' + (window as any).instrulang.rdsDistLinePopover + '">' +
(window as any).instrulang.rdsDistLine + '</a>'
var htmlDistLinePopover = Sanitizer.createSafeHTML( elemDistLinePopover )

const elemPnlDistAbs = '<div id="pnlDistAbs" class="panel panel-default day">' +
'<div id="pnlDistAbsHdr" class="panel-heading text-center day"></div>' +
'<b><div id="pnlDistAbsBdy" class="panel-body text-center day"></div></b>' +
'</div>'
var htmlPnlDistAbs = Sanitizer.createSafeHTML(elemPnlDistAbs)
var htmlPnlDistAbs = Sanitizer.createSafeHTML(elemPnlDistAbs)
const elemDistAbsPopover = '<a id="distAbsPop" href="#" role="button" ' +
'tabindex="1" data-toggle="popover1" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdsDistAbs + '" ' +
'data-content="' + (window as any).instrulang.rdsDistAbsPopover + '">' +
(window as any).instrulang.rdsDistAbs + '</a>'
var htmlDistAbsPopover = Sanitizer.createSafeHTML( elemDistAbsPopover )

const elemPnlBiasValue = '<div id="pnlPort" class="panel panel-default day">' +
'<div id="pnlBiasValueHdr" class="panel-heading text-center day"></div>' +
'<b><div id="pnlBiasValueBdy" class="panel-body text-center day"></div></b>' +
'</div>'
var htmlPnlBiasValue = Sanitizer.createSafeHTML(elemPnlBiasValue)
const elemWindBiasPopover = '<a id="windBiasPop" href="#" role="button" ' +
'tabindex="2" data-toggle="popover2" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdsWindBias + '" ' +
'data-content="' + (window as any).instrulang.rdsWindBiasPopover + '">' +
(window as any).instrulang.rdsWindBias + '</a>'
var htmlWindBiasPopover = Sanitizer.createSafeHTML( elemWindBiasPopover )

const elemPnlBiasDist = '<div id="pnlStarboard" class="panel panel-default day">' +
'<div id="pnlBiasDistHdr" class="panel-heading text-center day"></div>' +
'<b><div id="pnlBiasDistBdy" class="panel-body text-center day"></div></b>' +
'</div>'
var htmlPnlBiasDist = Sanitizer.createSafeHTML(elemPnlBiasDist)
const elemWindBiasAdvPopover = '<a id="windBiasAdvPop"href="#" role="button" ' +
'tabindex="3" data-toggle="popover3" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdsWindBiasAdv + '" ' +
'data-content="' + (window as any).instrulang.rdsWindBiasAdvPopover + '">' +
(window as any).instrulang.rdsWindBiasAdv + '</a>'
var htmlWindBiasAdvPopover = Sanitizer.createSafeHTML( elemWindBiasAdvPopover )

export function armedLineData( ) {
    console.log('racedashstart linedata armedLineData()')

    $('#grdDistLine').html( Sanitizer.unwrapSafeHTML(htmlPnlDistLine) )
    $('#pnlDistLineHdr').html( Sanitizer.unwrapSafeHTML(htmlDistLinePopover) )
    $('[data-toggle="popover0"]').popover(
        { trigger: 'hover', container: 'body', placement: 'right',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })
    $('#pnlDistLineBdy').text( '- - -' + (fsm.feet?' feet':' meters') )

    $('#grdDistAbs').html( Sanitizer.unwrapSafeHTML(htmlPnlDistAbs) )
    $('#pnlDistAbsHdr').html( Sanitizer.unwrapSafeHTML(htmlDistAbsPopover) )
    $('[data-toggle="popover1"]').popover(
        { trigger: 'hover', container: 'body', placement: 'left',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })
    $('#pnlDistAbsBdy').text( '- - -' + (fsm.feet?' feet':' meters') )

    $('#grdPort').html( Sanitizer.unwrapSafeHTML(htmlPnlBiasValue) )
    $('#pnlBiasValueHdr').html( Sanitizer.unwrapSafeHTML(htmlWindBiasPopover) )
    $('[data-toggle="popover2"]').popover(
        { trigger: 'hover', placement: 'right', container: 'body',
          delay: { 'show': 500, 'hide': 100},
          viewport: { selector: 'body', padding: 10 }
        })
    $('#pnlBiasValueBdy').text( '- - -' )

    $('#grdStarboard').html( Sanitizer.unwrapSafeHTML(htmlPnlBiasDist) )
    $('#pnlBiasDistHdr').html( Sanitizer.unwrapSafeHTML(htmlWindBiasAdvPopover) )
    $('[data-toggle="popover3"]').popover(
        { trigger: 'hover', placement: 'left', container: 'body',
          delay: { 'show': 500, 'hide': 100},
          viewport: { selector: 'body', padding: 10 }
        })
    $('#pnlBiasDistBdy').text( '- - -' + (fsm.feet?' feet':' meters') )

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
        (fsm.feet ? (distanceToGo *= 6076.12) : (distanceToGo *= 1852.0))
        distanceToGoStr = distanceToGo.toFixed(0)
    }
    $('#pnlDistLineBdy').text( distanceToGoStr + (fsm.feet?' feet':' meters') )

    var distanceToGoAbsStr: string = '- - -'
    var distanceToGoAbs: number = cpp.getslclosestpoint()
    if ( distanceToGoAbs >= 0.0 ) {
        (fsm.feet ? (distanceToGoAbs *= 6076.12) : (distanceToGoAbs *= 1852.0))
        distanceToGoAbsStr = distanceToGoAbs.toFixed(0)
    }
    $('#pnlDistAbsBdy').text( distanceToGoAbsStr + (fsm.feet?' feet':' meters') )

    var biasStr: string = '- - -'
    var bias: number = cpp.getslwindbias()
    if ( bias >= 0.0 )
        biasStr = bias.toFixed(0)
    $('#pnlBiasValueBdy').text( biasStr )

    var biasDistanceStr: string = '- - -'
    var biasDistance: number = cpp.getsladvantage()
    if ( biasDistance >= 0.0 ) {
        (fsm.feet ? (biasDistance *= 6076.12) : (biasDistance *= 1852.0))
        biasDistanceStr = biasDistance.toFixed(0)
    }
    $('#pnlBiasDistBdy').text( biasDistanceStr + (fsm.feet?' feet':' meters') )

}


export function quitLineData( that: StateMachine ) {
    console.log('racedashstart linedata quitLineData()')
    fadeAwayAllDataPanels()
    $('#grdDistAbs').promise().done(function() {
        console.log('racedashstart linedata quitLineData(): fading over')
        $('#grdPort').css({ opacity: 1.0 })
        $('#grdPort').empty()
        $('#grdStarboard').css({ opacity: 1.0 })
        $('#grdStarboard').empty()
        $('#grdDistLine').css({ opacity: 1.0 })
        $('#grdDistLine').empty()
        $('#grdDistAbs').css({ opacity: 1.0 })
        $('#grdDistAbs').empty()
        initLineData( that )
    })
}
