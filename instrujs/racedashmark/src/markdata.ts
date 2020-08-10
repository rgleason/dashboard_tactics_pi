/* $Id: markdata.ts, v1.0 2019/11/30 VaderDarth Exp $
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
var thisleg_twa: number = -999.9
var thisleg_twa_avg_short: number = -999.9
var thisleg_twa_avg_long: number = -999.9
var nextleg_current: number = -999.9
var nextleg_twa: number = -999.9
var nextleg_twa_avg_short: number = -999.9
var nextleg_twa_avg_long: number = -999.9
var nextleg_current: number = -999.9
var next2leg_twa: number = -999.9
var next2leg_twa_avg_short: number = -999.9
var next2leg_twa_avg_long: number = -999.9
var next2leg_current: number = -999.9

export function initMarkData( that: StateMachine ) {
    console.log('racedashstart markdata initMarkData()')
    fsm = that
    cpp = (window as any).iface
    locstate = 'READY'
}

const elemGrdTable = '' +
'<table id="table" class="table table-bordered table-hover day">' +
'    <thead id="tblhdr">' +
'        <tr id="tblhr1">' +
'            <th id="tblhr1r2c1" rowspan="2" class="info text-center">' +
'                <div id="tblhr1c1d1">' +
'                    <button id="tblhr1c1b1" type="button" class="btn btn-primary btn-sm">' +
'                    </button>' +
'                </div>' +
'                <div id="tblhr1c1d2" class="text-bold"></div>' +
'            </th>' +
'            <th id="tblhr1c2" rowspan="2" class="info text-center"></th>' +
'            <th id="tblhr1c3" colspan="2" class="info text-center"></th>' +
'            <th id="tblhr1c4" class="success text-center"></th>' +
'        </tr>' +
'        <tr id="tblhr2">' +
'            <th id="tblhr2c32" class="info text-center"></th>' +
'            <th id="tblhr2c33" class="info text-center"></th>' +
'            <th id="tblhr2c34" class="info text-center"></th>' +
'        </tr>' +
'      </thead>' +
'      <tbody id="tblbdy">' +
'        <tr id="tblr1">' +
'          <td id="tblr1c1" class="success text-center">- - -</td>' +
'          <td id="tblr1c2" class="active text-center">- - -</td>' +
'          <td id="tblr1c3" class="active text-center">- - -</td>' +
'          <td id="tblr1c4" class="active text-center">- - -</td>' +
'          <td id="tblr1c5" class="active text-center">- - -</td>' +
'        </tr>' +
'        <tr id="tblr2">' +
'            <td id="tblr2c1" class="success text-center">- - -</td>' +
'            <td id="tblr2c2" class="active text-center">- - -</td>' +
'            <td id="tblr2c3" class="active text-center">- - -</td>' +
'            <td id="tblr2c4" class="active text-center">- - -</td>' +
'            <td id="tblr2c5" class="active text-center">- - -</td>' +
'        </tr>' +
'        <tr id="tblr3">' +
'            <td id="tblr3c1" class="success text-center">- - -</td>' +
'            <td id="tblr3c2" class="active text-center">- - -</td>' +
'            <td id="tblr3c3" class="active text-center">- - -</td>' +
'            <td id="tblr3c4" class="active text-center">- - -</td>' +
'            <td id="tblr3c5" class="active text-center">- - -</td>' +
'        </tr>' +
'      </tbody>' +
'    </table>'
var htmlGrdTable = Sanitizer.createSafeHTML(elemGrdTable)

const elemLegNamePopover = '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' +
'<a id="legNameHdr" href="#" role="button" ' +
'tabindex="0" data-toggle="popover0" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdmRaceMarkTblColTitle + '" ' +
'data-content="' + (window as any).instrulang.rdmRaceMarkTblColTitlePopover + '">' +
(window as any).instrulang.rdmRaceMarkTblColTitle + '</a>' +
'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'
var htmlLegNamePopover = Sanitizer.createSafeHTML( elemLegNamePopover )

const elemTwaNowPopover = (window as any).instrulang.rdmRoute + '<br/>' +
'<a id="twaNowHdr" href="#" role="button" ' +
'tabindex="1" data-toggle="popover1" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdmTwaNow + '" ' +
'data-content="' + (window as any).instrulang.rdmTwaNowPopover + '">' +
(window as any).instrulang.rdmTwaNow + '</a>'
var htmlTwaNowPopover = Sanitizer.createSafeHTML( elemTwaNowPopover )

const elemTwaAvgPopover = (window as any).instrulang.rdmRoute + ' ' +
'<a id="twaAvgHdr" href="#" role="button" ' +
'tabindex="2" data-toggle="popover2" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdmTwaAvg + '" ' +
'data-content="' + (window as any).instrulang.rdmTwaAvgPopover + '">' +
(window as any).instrulang.rdmTwaAvg + '</a>'
var htmlTwaAvgPopover = Sanitizer.createSafeHTML( elemTwaAvgPopover )

const elemTwaAvgShortPopover = '<a id="twaAvgShortHdr" href="#" role="button" ' +
'tabindex="3" data-toggle="popover3" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdmTwaAvgShort + '" ' +
'data-content="' + (window as any).instrulang.rdmTwaAvgShortPopover + '">' +
(window as any).instrulang.rdmTwaAvgShort + '</a>'
var htmlTwaAvgShortPopover = Sanitizer.createSafeHTML( elemTwaAvgShortPopover )

const elemTwaAvgLongPopover = '<a id="twaAvgLongHdr" href="#" role="button" ' +
'tabindex="4" data-toggle="popover4" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdmTwaAvgLong + '" ' +
'data-content="' + (window as any).instrulang.rdmTwaAvgLongPopover + '">' +
(window as any).instrulang.rdmTwaAvgLong + '</a>'
var htmlTwaAvgLongPopover = Sanitizer.createSafeHTML( elemTwaAvgLongPopover )

const elemCurrentPopover = '<a id="CurrentHdr" href="#" role="button" ' +
'tabindex="5" data-toggle="popover5" data-trigger="focus" data-html="false" ' +
'title="' + (window as any).instrulang.rdmCurrent + '" ' +
'data-content="' + (window as any).instrulang.rdmCurrentPopover + '">' +
(window as any).instrulang.rdmCurrent + '</a>'
var htmlCurrentPopover = Sanitizer.createSafeHTML( elemCurrentPopover )


export function armedMarkData( ) {
    console.log('racedashstart Markdata armedMarkData()')

    $('#grdTable').html( Sanitizer.unwrapSafeHTML(htmlGrdTable) )

    $('#tblhr1c1b1').text( (window as any).instrulang.rdmRaceMarkHideChart )

    $('#tblhr1c1d2').html( Sanitizer.unwrapSafeHTML(htmlLegNamePopover) )
    $('[data-toggle="popover0"]').popover(
        { trigger: 'hover', container: 'body', placement: 'right',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })

    $('#tblhr1c2').html( Sanitizer.unwrapSafeHTML(htmlTwaNowPopover) )
    $('[data-toggle="popover1"]').popover(
        { trigger: 'hover', container: 'body', placement: 'right',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })

    $('#tblhr1c3').html( Sanitizer.unwrapSafeHTML(htmlTwaAvgPopover) )
    $('[data-toggle="popover2"]').popover(
        { trigger: 'hover', container: 'body', placement: 'right',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })

    $('#tblhr2c32').html( Sanitizer.unwrapSafeHTML(htmlTwaAvgShortPopover) )
    $('[data-toggle="popover3"]').popover(
        { trigger: 'hover', container: 'body', placement: 'right',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })

    $('#tblhr2c33').html( Sanitizer.unwrapSafeHTML(htmlTwaAvgLongPopover) )
    $('[data-toggle="popover4"]').popover(
        { trigger: 'hover', container: 'body', placement: 'left',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })

    $('#tblhr2c34').html( Sanitizer.unwrapSafeHTML(htmlCurrentPopover) )
    $('[data-toggle="popover5"]').popover(
        { trigger: 'hover', container: 'body', placement: 'left',
          viewport: { selector: 'body', padding: 10 },
          delay: { 'show': 500, 'hide': 100}
        })

    locstate = 'ARMED'
}

function fadeAwayAllDataPanels() {
    console.log('racedashstart Markdata fadeAwayAllDataPanels()')
    $('#grdTable').animate({
         opacity: 0.25
    }, 1000, function() { $(this).text( '' ) })
}

export function newMarkData() {
    console.log('racedashstart Markdata newMarkData()')

    var sanitizedStr: string

    var bearingBackStr: string = '- - -'
    var bearingBack: number = cpp.getmrkbrgback()
    if ( bearingBack != -999.0 ) {
        bearingBackStr = (window as any).instrulang.rdmRteBrg +
            '&#x2193;&nbsp;' + bearingBack.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( bearingBackStr )
    $('#tblhr1c4').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk1NameStr: string = cpp.getmrk1name()
    $('#tblr1c1').text( mrk1NameStr )

    var mrk1TwaLiveStr: string = '- - -'
    var mrk1TwaLive: number = cpp.getmrk1twalive()
    if ( mrk1TwaLive != -999.0 ) {
        mrk1TwaLiveStr = mrk1TwaLive.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk1TwaLiveStr )
    $('#tblr1c2').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk1TwaShortStr: string = '- - -'
    var mrk1TwaShort: number = cpp.getmrk1twashort()
    if ( mrk1TwaShort != -999.0 ) {
        mrk1TwaShortStr = mrk1TwaShort.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk1TwaShortStr )
    $('#tblr1c3').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk1TwaLongStr: string = '- - -'
    var mrk1TwaLong: number = cpp.getmrk1twalong()
    if ( mrk1TwaLong != -999.0 ) {
        mrk1TwaLongStr = mrk1TwaLong.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk1TwaLongStr )
    $('#tblr1c4').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk1CurrentStr: string = '- - -'
    var mrk1Current: number = cpp.getmrk1current()
    if ( mrk1Current != -999.0 ) {
        mrk1CurrentStr = mrk1Current.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk1CurrentStr )
    $('#tblr1c5').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk2NameStr: string = cpp.getmrk2name()
    $('#tblr2c1').text( mrk2NameStr )

    var mrk2TwaLiveStr: string = '- - -'
    var mrk2TwaLive: number = cpp.getmrk2twalive()
    if ( mrk2TwaLive != -999.0 ) {
        mrk2TwaLiveStr = mrk2TwaLive.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk2TwaLiveStr )
    $('#tblr2c2').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk2TwaShortStr: string = '- - -'
    var mrk2TwaShort: number = cpp.getmrk2twashort()
    if ( mrk2TwaShort != -999.0 ) {
        mrk2TwaShortStr = mrk2TwaShort.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk2TwaShortStr )
    $('#tblr2c3').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk2TwaLongStr: string = '- - -'
    var mrk2TwaLong: number = cpp.getmrk2twalong()
    if ( mrk2TwaLong != -999.0 ) {
        mrk2TwaLongStr = mrk2TwaLong.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk2TwaLongStr )
    $('#tblr2c4').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk2CurrentStr: string = '- - -'
    var mrk2Current: number = cpp.getmrk2current()
    if ( mrk2Current != -999.0 ) {
        mrk2CurrentStr = mrk2Current.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk2CurrentStr )
    $('#tblr2c5').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk3NameStr: string = cpp.getmrk3name()
    $('#tblr3c1').text( mrk3NameStr )

    var mrk3TwaLiveStr: string = '- - -'
    var mrk3TwaLive: number = cpp.getmrk3twalive()
    if ( mrk3TwaLive != -999.0 ) {
        mrk3TwaLiveStr = mrk3TwaLive.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk3TwaLiveStr )
    $('#tblr3c2').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk3TwaShortStr: string = '- - -'
    var mrk3TwaShort: number = cpp.getmrk3twashort()
    if ( mrk3TwaShort != -999.0 ) {
        mrk3TwaShortStr = mrk3TwaShort.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk3TwaShortStr )
    $('#tblr3c3').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk3TwaLongStr: string = '- - -'
    var mrk3TwaLong: number = cpp.getmrk3twalong()
    if ( mrk3TwaLong != -999.0 ) {
        mrk3TwaLongStr = mrk3TwaLong.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk3TwaLongStr )
    $('#tblr3c4').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

    var mrk3CurrentStr: string = '- - -'
    var mrk3Current: number = cpp.getmrk3current()
    if ( mrk3Current != -999.0 ) {
        mrk3CurrentStr = mrk3Current.toFixed(0) + '&#x00b0;'
    }
    sanitizedStr = Sanitizer.createSafeHTML( mrk3CurrentStr )
    $('#tblr3c5').html( Sanitizer.unwrapSafeHTML( sanitizedStr ) )

}

export function startMarkData( that: StateMachine ) {
    console.log('racedashstart Markdata startMarkData()')
    fadeAwayAllDataPanels()
    $('#grdTable').promise().done(function() {
        console.log('racedashstart Markdata startMarkData(): fading over')
        $('#grdTable').css({ opacity: 1.0 })
        $('#grdTable').empty()
        armedMarkData( )
    })
}
