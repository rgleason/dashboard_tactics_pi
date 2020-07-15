/* $Id: button.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/
/*eslint new-cap: ["error", { "newIsCap": false }]*/

import sanitizer from '../../src/escapeHTML'
var Sanitizer = new sanitizer()

import {StateMachine} from './statemachine'

var fsm: StateMachine
var dbglevel: number = (window as any).instrustat.debuglevel

var locstate: string = ''
var timeLeft: number = 300
var showTimeLeft = false

const elemPnlCenter = '<div id="pnlCenter" class="panel panel-default day">' +
'<div id="pnlCenterBdy" class="panel-body text-center day"></div>' +
'</div>'
var htmlPnlCenter = Sanitizer.createSafeHTML(elemPnlCenter)
const elemBtnArm = '<button id="btnArm" type="button" class="btn btn-lg btn-warning disabled">' +
'</button>'
var htmlBtnArm = Sanitizer.createSafeHTML(elemBtnArm)
const elemBtnArmed = '<button id="btnFiveMinutes" type="button" class="btn btn-lg btn-primary disabled">' +
'</button>' +
'&nbsp;<button id="btnFourMinutes" type="button" class="btn btn-lg btn-info disabled">' +
'</button>' +
'&nbsp;<button id="btnArmedQuit" type="button" class="btn btn-sm btn-warning disabled">' +
'</div>'
var htmlBtnArmed = Sanitizer.createSafeHTML(elemBtnArmed)

const elemPnlDropPort = '<div id="pnlPort" class="panel panel-default day">' +
'<div id="pnlDropPortBdy" class="panel-body text-center day"></div>' +
'</div>'
var htmlPnlDropPort = Sanitizer.createSafeHTML(elemPnlDropPort)
const elemBtnDropPort = '<button id="btnDropPort" type="button" class="btn btn-lg btn-danger disabled">' +
'</button>'
var htmlBtnDropPort = Sanitizer.createSafeHTML(elemBtnDropPort)

const elemPnlDropStarboard = '<div id="pnlStarboard" class="panel panel-default day">' +
'<div id="pnlDropStarboardBdy" class="panel-body text-center day"></div>' +
'</div>'
var htmlPnlDropStarboard = Sanitizer.createSafeHTML(elemPnlDropStarboard)
const elemBtnDropStarboard = '<button id="btnDropStarboard" type="button" class="btn btn-lg btn-success disabled">' +
'</button>'
var htmlBtnDropStarboard = Sanitizer.createSafeHTML(elemBtnDropStarboard)

const elemLicenseModal = '<a href="#" role="button" data-toggle="modal" ' +
'tabindex="0" data-target="#myLicense">LICENSE</a>' +
'<div class="modal fade" id="myLicense" role="dialog">' +
'<div class="modal-dialog modal-lg"><div class="modal-content">' +
'<div class="modal-header">' +
'<button type="button" class="close" data-dismiss="modal">&times;</button>' +
'<h4 class="modal-title">MIT License</h4></div>' +
'<div class="modal-body">' +
'<p>Copyright &#xA9 2020 Petri Mäkijärvi' +
'</p><p>' +
'Permission is hereby granted, free of charge, to any person obtaining a copy ' +
'of this software and associated documentation files (the "Software"), to deal ' +
'in the Software without restriction, including without limitation the rights ' +
'to use, copy, modify, merge, publish, distribute, sublicense, and/or sell ' +
'copies of the Software, and to permit persons to whom the Software is ' +
'furnished to do so, subject to the following conditions:' +
'</p><p>' +
'The above copyright notice and this permission notice shall be included in all ' +
'copies or substantial portions of the Software.' +
'</p><p>' +
'THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR ' +
'IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, ' +
'FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE ' +
'AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER ' +
'LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, ' +
'OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE ' +
'SOFTWARE.' +
'</p>' +
'</div>' +
'<div class="modal-footer">' +
'<button type="button" class="btn btn-default" data-dismiss="modal">Close</button>' +
'</div></div></div></div>'
var htmlLicenseModal = Sanitizer.createSafeHTML(
    (window as any).instrulang.rdsLicenseMsg + ' ' + elemLicenseModal )

export function initButtons( that: StateMachine ) {
    console.log('racedashstart buttons initButtons()')
    fsm = that
    $('#grdCenter').html( Sanitizer.unwrapSafeHTML(htmlPnlCenter) )
    $('#pnlCenterBdy').html( Sanitizer.unwrapSafeHTML(htmlBtnArm) )
    if ( (fsm.conf === null) || fsm.conf.wrnmsg ) {
        $('#pnlMsgClockBdy').text( (window as any).instrulang.rdsInitMsg )
        $('#btnArm').text( (window as any).instrulang.rdsBtnArmTxt )
    }
    else {
        $('#pnlMsgClockBdy').html( Sanitizer.unwrapSafeHTML(htmlLicenseModal) )
        $('#btnArm').text( (window as any).instrulang.rdsBtnAcceptTxt )
    }
    $('#btnArm').removeClass('disabled')
    locstate = 'READY'
}

$('body').on('click', '#btnArm', function(event) {
    console.log('racedashstart buttons event click #btnArm')
    console.log('racedashstart buttons - state: ', fsm.state)
    $('#btnArm').addClass('active')
    if ( fsm.is('marking') || fsm.is('onestbd') ||
        fsm.is('oneport') || fsm.is('onemark') )
        fsm.btnarmc()
    else {
        if ( (fsm.conf === null) || fsm.conf.wrnmsg ) {
            fsm.btnarmw()
            $('#btnArm').removeClass('active')
            $('#btnArm').addClass('disabled')
        }
        else {
            $('#pnlMsgClockBdy').text( (window as any).instrulang.rdsInitMsg )
            $('#btnArm').text( (window as any).instrulang.rdsBtnArmTxt )
            if ( fsm.conf !== null )
                fsm.conf.wrnmsg = true
            $('#btnArm').removeClass('active')
        }
    }
    console.log('racedashstart buttons - state now: ', fsm.state)
})

export function btmarmwButtons( ) {
    console.log('racedashstart buttons btmarmwButtons()')

    $('#pnlMsgClockBdy').text( (window as any).instrulang.rdsDropMarksMsg )

    $('#grdPort').html( Sanitizer.unwrapSafeHTML(htmlPnlDropPort) )
    $('#pnlDropPortBdy').html( Sanitizer.unwrapSafeHTML(htmlBtnDropPort) )
    $('#btnDropPort').text( (window as any).instrulang.rdsDropPortBtn )
    $('#btnDropPort').removeClass('disabled')

    $('#grdStarboard').html( Sanitizer.unwrapSafeHTML(htmlPnlDropStarboard) )
    $('#pnlDropStarboardBdy').html( Sanitizer.unwrapSafeHTML(htmlBtnDropStarboard) )
    $('#btnDropStarboard').text( (window as any).instrulang.rdsDropStarboardBtn )
    $('#btnDropStarboard').removeClass('disabled')

    $('#btnArm').text( (window as any).instrulang.rdsBtnArmCancel )
    $('#btnArm').removeClass('active')
    locstate = 'MARKING'
}

function fadeAwayPortCenterStarboard() {
    console.log('racedashstart buttons fadeAwayPortCenterStarboard()')
    $('#grdPort').animate({
         opacity: 0.25
    }, 900, function() { $(this).text( '' ) })
    $('#grdStarboard').animate({
         opacity: 0.25
    }, 900, function() { $(this).text( '' ) })
    $('#grdCenter').animate({
         opacity: 0.25
    }, 1100, function() { $(this).text( '' ) })
}

export function btmarmcButtons( that: StateMachine ) {
    console.log('racedashstart buttons btmarmcButtons()')
    fadeAwayPortCenterStarboard()
    $('#grdCenter').promise().done(function() {
        console.log('racedashstart buttons btmarmcButtons(): fading over')
        $('#grdCenter').css({ opacity: 1.0 })
        $('#grdCenter').empty()
        $('#grdPort').css({ opacity: 1.0 })
        $('#grdPort').empty()
        $('#grdStarboard').css({ opacity: 1.0 })
        $('#grdStarboard').empty()
        initButtons( that )
    })
}

$('body').on('click', '#btnDropPort', function(event) {
    console.log('racedashstart buttons event click #btnDropPort')
    console.log('racedashstart buttons - state: ', fsm.state)
    if ( !$('#btnDropPort').css('disabled') ) {
        console.log('racedashstart buttons - btnDropPort button enabled and pressed')
        if ( fsm.is('marking') ) {
            fsm.btnportd1()
            $('#btnDropPort').addClass('active')
            $('#btnDropPort').addClass('disabled')
        }
        else {
            locstate = 'MARKED'
            fsm.btnportd2()
        }
    }
    console.log('racedashstart buttons - btnDropPort - state now: ', fsm.state)
})

$('body').on('click', '#btnDropStarboard', function(event) {
    console.log('racedashstart buttons event click #btnDropStarboard')
    console.log('racedashstart buttons - state: ', fsm.state)
    if ( !$('#btnDropStarboard').css('disabled') ) {
        console.log('racedashstart buttons - btnDropStarboard button enabled and pressed')
        if ( fsm.is('marking') ) {
            fsm.btnstbdd1()
            $('#btnDropStarboard').addClass('active')
            $('#btnDropStarboard').addClass('disabled')
        }
        else {
            locstate = 'MARKED'
            fsm.btnstbdd2()
        }
    }
    console.log('racedashstart buttons - btnDropStarboard - state now: ', fsm.state)
})

export function btmarmedButtons( that: StateMachine ) {
    console.log('racedashstart buttons btmarmedButtons()')
    fadeAwayPortCenterStarboard()
    $('#grdCenter').promise().done(function() {
        console.log('racedashstart buttons btmarmedButtons(): fading over')
        $('#grdCenter').css({ opacity: 1.0 })
        $('#grdCenter').empty()
        $('#grdStarboard').css({ opacity: 1.0 })
        $('#grdStarboard').empty()
        $('#grdPort').css({ opacity: 1.0 })
        $('#grdPort').empty()
        $('#grdCenter').html( Sanitizer.unwrapSafeHTML(htmlPnlCenter) )
        $('#pnlCenterBdy').html( Sanitizer.unwrapSafeHTML(htmlBtnArmed) )
        $('#pnlMsgClockBdy').text( (window as any).instrulang.rdsMarkedAndArmed )
        $('#btnFiveMinutes').text( (window as any).instrulang.rdsBtnArmed5m )
        $('#btnFiveMinutes').removeClass('disabled')
        $('#btnFourMinutes').text( (window as any).instrulang.rdsBtnArmed4m )
        $('#btnFourMinutes').removeClass('disabled')
        $('#btnArmedQuit').text( (window as any).instrulang.rdsBtnArmedQuit )
        $('#btnArmedQuit').removeClass('disabled')
        if ( fsm.is('btnfade') )
            fsm.btnfaded()
        locstate = 'ARMED'
    })
}

$('body').on('click', '#btnArmedQuit', function(event) {
    console.log('racedashstart buttons event click #btnArmedQuit')
    console.log('racedashstart buttons - state: ', fsm.state)
    $('#btnArmedQuit').addClass('active')
    if ( fsm.is('armed') )
        fsm.btnarma()
    console.log('racedashstart buttons - state now: ', fsm.state)
})

export function btmarmaButtons( that: StateMachine ) {
    console.log('racedashstart buttons btmarmaButtons()')
    showTimeLeft = false
    $('#grdCenter').animate({
         opacity: 0.25
    }, 1000, function() { $(this).text( '' ) })
    $('#grdCenter').promise().done(function() {
        console.log('racedashstart buttons btmarmaButtons(): fading over')
        $('#grdCenter').css({ opacity: 1.0 })
        $('#grdCenter').empty()
        $('#pnlCenter').remove()
        $('#pnlMsgClock').removeClass('panel-danger')
        $('#pnlMsgClock').addClass('panel-primary')
        initButtons( that )
    })
}

function makeTimer() {
    if ( showTimeLeft ) {
        timeLeft--
        var minutes: number = Math.floor(timeLeft / 60)
        var seconds: number = Math.floor(timeLeft - (minutes * 60))
        var strSeconds: string = seconds.toString()
        if (seconds < 10)
            strSeconds = '0' + strSeconds
        var htlmObjCandidate = '<b>' + minutes.toString() +
            '</b><span> minutes </span><b>' +
            strSeconds + '</b><span> seconds</span>'
        if ( minutes < 0 ) {
            htlmObjCandidate = '<b>' +
                (window as any).instrulang.rdsAllTimeBurned + '</b>'
            $('#pnlMsgClock').removeClass('panel-primary')
            $('#pnlMsgClock').addClass('panel-danger')
        }
        var htmlObj = Sanitizer.createSafeHTML( htlmObjCandidate )
        $('#pnlMsgClockBdy').html( Sanitizer.unwrapSafeHTML( htmlObj ) )
    }
}

setInterval(function() { makeTimer() }, 1000)

$('body').on('click', '#btnFiveMinutes', function(event) {
    timeLeft = 300
    showTimeLeft = true
    $('#btnFiveMinutes').addClass('disabled')
})
$('body').on('click', '#btnFourMinutes', function(event) {
    timeLeft = 240
    showTimeLeft = true
    $('#btnFiveMinutes').addClass('disabled')
    $('#btnFourMinutes').addClass('disabled')
})
