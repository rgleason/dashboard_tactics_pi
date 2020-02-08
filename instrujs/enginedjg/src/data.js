/* $Id: data.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to ask and retrieve from a client a unique ID for this instance

import sanitizer from '../../src/escapeHTML'
var Sanitizer = sanitizer()

import {rollDisplayToSelection} from './disp'
import { getPathDefaultsIfNew } from '../../src/conf'

var dbglevel = window.instrustat.debuglevel
var alerts = window.instrustat.alerts
var alertdelay = window.instrustat.alertdelay

export function onWaitdataFinalCheck( that ) {
    var elem = document.getElementById('skPath')
    var htmlObj = null
    var htmlCandidate = null
    var getSetDefTitle = (function () {
        getPathDefaultsIfNew( that )
        if ( (that.conf.title !== null) && (that.conf.title !== '' ) )
            htmlCandidate = that.conf.title
        else
            htmlCandidate = that.conf.path
    })
    if ( that.conf !== null ) {
        if ( (that.conf.title !== null) && (that.conf.title !== '' ) )
            htmlCandidate = that.conf.title
        else if ( (that.conf.path !== null) && (that.conf.path !== '' ) )
            getSetDefTitle()
    }
    else if ( (that.path !== null) && (that.path !== '' ) )
        getSetDefTitle()
    else if ( dbglevel > 1 )
        console.error('onWaitdataFinalCheck(): no path, no conf!')

    if ( htmlCandidate !== null ) {
        htmlObj = Sanitizer.createSafeHTML(htmlCandidate)
        elem.innerHTML = Sanitizer.unwrapSafeHTML(htmlObj)
    }
}

var alertcondition = false
var alertcounter = 0
var alertthreshold = alertdelay
var suppressShowData = false

export function showData( that ) {
    if ( suppressShowData )
        return
    that.glastvalue = window.iface.getdata()
    var dispvalue = that.glastvalue
    console.log('dispvalue : ', dispvalue)
    if ( that.conf !== null ) {
        dispvalue *= that.conf.multiplier
        if ( dbglevel > 2 )
            console.log('dispvalue x: ', dispvalue, 'multiplier: ', that.conf.multiplier)
        if ( that.conf.divider >0 )
            dispvalue /= that.conf.divider
        if ( dbglevel > 2 )
            console.log('dispvalue /: ', dispvalue, 'divider: ', that.conf.divider)
        if ( that.conf.offset !== 0)
            dispvalue += that.conf.offset
        if ( dbglevel > 2 )
            console.log('dispvalue +: ', dispvalue, 'offset: ', that.conf.offset)
        if ( alerts ) {
            if ( !alertcondition ) {
                var alertSource
                if ( (that.conf.title !== null) && (that.conf.title !== '' ) )
                    alertSource = that.conf.title
                else
                    alertSource = that.conf.path
                if ( that.conf.loalert !== 0 ) {
                    if ( dispvalue < that.conf.loalert ) {
                        if ( alertcounter >= alertthreshold) {
                            alertcondition = true
                            alert ( window.instrulang.alertTitle + '\n' +
                                    alertSource + '\n' +
                                    window.instrulang.alertLolimit + '\n' +
                                    dispvalue + ' ' + that.conf.unit )
                        }
                        else {
                            alertcounter += 1
                        }
                    }
                }
                if ( that.conf.hialert !== 0 ) {
                    if ( dispvalue > that.conf.hialert ) {
                        if ( alertcounter >= alertthreshold ) {
                            alertcondition = true
                            alert ( window.instrulang.alertTitle + '\n' +
                                    alertSource + '\n' +
                                    window.instrulang.alertHilimit + '\n' +
                                    dispvalue + ' ' + that.conf.unit )
                        }
                        else {
                            alertcounter += 1
                        }
                    }
                }
            }
            else {
                if ( (dispvalue > that.conf.loalert) &&
                     ( (dispvalue < that.conf.hialert) ||
                       ( that.conf.hialert === 0) ) ) {
                    alertcondition = false
                    alertcounter = 0
                }
            }
        }
    }
    if ( (that.gauge.length > 0) && (that.glastvalue !== null) ) {
        that.gauge[0].refresh(
            dispvalue,
            that.conf.maxval,
            that.conf.minval,
            that.conf.unit )
    }
    else {
        var elemnum = document.getElementById('numgauge0')
        var htmlCandidate
        var htmlObj
        if ( elemnum !== null ) {
            var roundedval = dispvalue.toFixed( that.conf.decimals )
            htmlCandidate = roundedval + that.conf.symbol
            htmlObj = Sanitizer.createSafeHTML(htmlCandidate)
            elemnum.innerHTML = Sanitizer.unwrapSafeHTML(htmlObj)
        }
        var elemunit = document.getElementById('numgunit0')
        if ( elemunit !== null ) {
            htmlCandidate = that.conf.unit
            htmlObj = Sanitizer.createSafeHTML(htmlCandidate)
            elemunit.innerHTML = Sanitizer.unwrapSafeHTML(htmlObj)
        }
    }
}

export function clearData( that ) {
    that.glastvalue = 0
    if ( that.gauge.length > 0 ) {
        that.gauge[0].symbol = ''
        that.gauge[0].refresh( 0, 100, 0, '' )
    }
    else {
        document.getElementById('numgauge0').innerHTML = '&nbsp;'
        document.getElementById('numgunit0').innerHTML = '&nbsp;'
    }
    suppressShowData = true // otherwise the rolling dial will check for value
    rollDisplayToSelection( that )
    suppressShowData = false
}

export function prepareDataHalt( that ) {
    clearData( that )
}
