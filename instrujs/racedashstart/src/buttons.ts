/* $Id: button.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import {StateMachine} from "./statemachine"

var fsm: StateMachine
var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts

var locstate: string = ''

var $elemPnlCenter = $('<div id="pnlDistLine" class="panel panel-default day">' +
'<div id="pnlDistLineHdr" class="panel-heading text-center day"></div>' +
'<div id="pnlDistLineBdy" class="panel-body text-center day"></div>' +
'</div>')
var $elemBtnArm = $('<button id="btnArm" type="button" class="btn btn-lg btn-warning disabled">' +
'</button>' )

var $elemPnlDropPort = $('<div id="pnlPort" class="panel panel-default day">' +
'<div id="pnlDropPortHdr" class="panel-heading text-center day"></div>' +
'<div id="pnlDropPortBdy" class="panel-body text-center day"></div>' +
'</div>')
var $elemBtnDropPort = $('<button id="btnDropPort" type="button" class="btn btn-lg btn-danger disabled">' +
'</button>' )

var $elemPnlDropStarboard = $('<div id="pnlStarboard" class="panel panel-default day">' +
'<div id="pnlDropStarboardHdr" class="panel-heading text-center day"></div>' +
'<div id="pnlDropStarboardBdy" class="panel-body text-center day"></div>' +
'</div>')
var $elemBtnDropStarboard = $('<button id="btnDropStarboard" type="button" class="btn btn-lg btn-success disabled">' +
'</button>' )


export function initButtons( that: StateMachine ) {
    fsm = that
    console.log('racedashstart buttons initButtons()')
    $('#pnlMsgClockBdy').text( (window as any).instrulang.rdsInitMsg )
    $elemPnlCenter.appendTo( $('#grdCenter') )
    $elemBtnArm.appendTo( $('#pnlDistLineBdy') )
    $('#btnArm').text( (window as any).instrulang.rdsBtnArmTxt )
    $('#btnArm').removeClass('disabled')
    locstate = 'RDY'
}

$('body').on('click', '#btnArm', function(event) {
    console.log('racedashstart buttons event click #btnArm')
    console.log('racedashstart buttons - state: ', fsm.state)
    $('#btnArm').addClass('active')
    fsm.btnarmw()
    console.log('racedashstart buttons - state now: ', fsm.state)
})

export function btmarmwButtons( ) {
    console.log('racedashstart buttons btmarmwButtons()')

    $('#pnlMsgClockBdy').text( (window as any).instrulang.rdsDropMarksMsg )

    $elemPnlDropPort.appendTo( $('#grdPort') )
    $elemBtnDropPort.appendTo( $('#pnlDropPortBdy') )
    $('#pnlDropPortHdr').text( (window as any).instrulang.rdsDropPortHdr )
    $('#btnDropPort').text( (window as any).instrulang.rdsDropPortBtn )
    $('#btnDropPort').removeClass('disabled')

    $elemPnlDropStarboard.appendTo( $('#grdStarboard') )
    $elemBtnDropStarboard.appendTo( $('#pnlDropStarboardBdy') )
    $('#pnlDropStarboardHdr').text( (window as any).instrulang.rdsDropStarboardHdr )
    $('#btnDropStarboard').text( (window as any).instrulang.rdsDropStarboardBtn )
    $('#btnDropStarboard').removeClass('disabled')

    $('#pnlDistLineHdr').text( (window as any).instrulang.rdsBtnArmCancelHdr )
    $('#btnArm').text( (window as any).instrulang.rdsBtnArmCancel )
    $('#btnArm').removeClass('active')
}
