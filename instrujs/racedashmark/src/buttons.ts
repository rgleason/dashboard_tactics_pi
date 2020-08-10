/* $Id: button.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/
/*eslint new-cap: ["error", { "newIsCap": false }]*/

import sanitizer from '../../src/escapeHTML'
var Sanitizer = new sanitizer()

import {StateMachine} from './statemachine'
import {memorizeSettings} from '../../src/conf'

import { cppGetData, cppMuteChart, cppResumeChart } from './cppMark'

var fsm: StateMachine
var dbglevel: number = (window as any).instrustat.debuglevel

var locstate: string = ''
var timeLeft: number = 300
var showTimeLeft = false
var btnChartMuted = false

const elemWelcomeWait = '' +
'<div id="rowWelcome" class="row">' +
'    <div id="colPadLeft" class="col-sm-3">' +
'    </div>' +
'    <div id="colWelcome" class="col-sm-6">' +
'        <div id="pnlMsg" class="panel panel-primary day">' +
'            <div id="pnlMsgHeader" class="panel-heading text-center day">' +
'            Race Dash Mark' +
'            </div>' +
'            <div id="pnlMsgBody" class="panel-body text-center day">' +
'            </div>' +
'        </div>' +
'    </div>' +
'    <div id="colPadRight" class="col-sm-3">' +
'   </div>' +
'</div>'
var htmlWelcomeWait = Sanitizer.createSafeHTML(elemWelcomeWait)

const elemMsgBodyAndButton = '<div id="pnlMsgBodyTxt"></div>' +
'<button id="btnArm" type="button" class="btn btn-lg btn-warning disabled">' +
'</button>'
var htmlMsgBodyAndButton = Sanitizer.createSafeHTML(elemMsgBodyAndButton)

const elemMsgBodyOnly = '<div id="pnlMsgBodyTxt"></div>'
var htmlMsgBody = Sanitizer.createSafeHTML(elemMsgBodyOnly)

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

const elemInitMsgPopover = '<a id="initMsg" href="#" role="button" ' +
'tabindex="0" data-toggle="popover0" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdmInitMsg + '" ' +
'data-content="' + (window as any).instrulang.rdmInitMsgPopover + '">' +
(window as any).instrulang.rdmInitMsg + '</a>'
var htmlInitMsgPopover = Sanitizer.createSafeHTML( elemInitMsgPopover )

function setBodyForWait( firstStart: boolean ) {
    if ( !firstStart)
        $('#grdTable').html( Sanitizer.unwrapSafeHTML(htmlWelcomeWait) )
    $('#pnlMsgBody').html( Sanitizer.unwrapSafeHTML(htmlMsgBody) )
    $('#pnlMsgBodyTxt').html( Sanitizer.unwrapSafeHTML(htmlInitMsgPopover) )
    $('[data-toggle="popover0"]').popover(
        { trigger: 'hover', container: 'body', placement: 'right',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })
    if ( firstStart )
        cppGetData()
}

export function initButtons( that: StateMachine ) {
    console.log('racedashmark buttons initButtons()')
    fsm = that
    if ( (fsm.conf === null) || fsm.conf.wrnmsg ) {
        setBodyForWait( true )
    }
    else {
        $('#pnlMsgBody').html( Sanitizer.unwrapSafeHTML(htmlMsgBodyAndButton) )
        $('#pnlMsgBodyTxt').html( Sanitizer.unwrapSafeHTML(htmlLicenseModal) )
        $('#btnArm').text( (window as any).instrulang.rdsBtnAcceptTxt )
    }
    $('#btnArm').removeClass('disabled')
    locstate = 'READY'
}

$('body').on('click', '#btnArm', function(event) {
    console.log('racedashmark buttons event click #btnArm')
    console.log('racedashmark buttons - state: ', fsm.state)
    $('#btnArm').addClass('active')
    if ( !(fsm.conf === null) ) {
        if ( !fsm.conf.wrnmsg ) {
            setBodyForWait( true )
            fsm.conf.wrnmsg = true
            memorizeSettings( fsm )
        }
    }
    else {
        setBodyForWait( true )
    }
    console.log('racedashmark buttons - state now: ', fsm.state)
})

$('body').on('click', '#tblhr1c1b1', function(event) {
    console.log('racedashmark buttons event click #tblhr1c1b1')
    console.log('racedashmark buttons - state: ', fsm.state,
                ', btnChartMuted: ', btnChartMuted )
    $('#tblhr1c1b1').addClass('active')
    if ( btnChartMuted ) {
        cppResumeChart()
        btnChartMuted = false
        $('#tblhr1c1b1').text( (window as any).instrulang.rdmRaceMarkHideChart )
        $('#tblhr1c1b1').removeClass('btn-warning')
        $('#tblhr1c1b1').addClass('btn-primary')
    }
    else {
        cppMuteChart()
        btnChartMuted = true
        $('#tblhr1c1b1').text( (window as any).instrulang.rdmRaceMarkShowOnChart )
        $('#tblhr1c1b1').removeClass('btn-primary')
        $('#tblhr1c1b1').addClass('btn-warning')
    }
    $('#tblhr1c1b1').removeClass('active')
})

function fadeAwayAllDataPanels() {
    console.log('racedashstart button fadeAwayAllDataPanels()')
    $('#grdTable').animate({
         opacity: 0.25
    }, 1000, function() { $(this).text( '' ) })
}

export function startWaitData( that: StateMachine ) {
    console.log('racedashstart buttons startWaitData()')
    fadeAwayAllDataPanels()
    $('#grdTable').promise().done(function() {
        console.log('racedashstart buttons startWaitData(): fading over')
        $('#grdTable').css({ opacity: 1.0 })
        $('#grdTable').empty()
        setBodyForWait( false ) // data keeps coming now, don't ask again
    })
}
