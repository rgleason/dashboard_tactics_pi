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
    $('#pnlDistLineHdr').text( (window as any).instrulang.rdsBtnArmInitHdr )
    $('#btnArm').text( (window as any).instrulang.rdsBtnArmTxt )
    $('#btnArm').removeClass('disabled')
    locstate = 'READY'
}

$('body').on('click', '#btnArm', function(event) {
    console.log('racedashstart buttons event click #btnArm')
    console.log('racedashstart buttons - state: ', fsm.state)
    $('#btnArm').addClass('active')
    if ( fsm.is('marking') || fsm.is('onemark') )
        fsm.btnarmc()
    else
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
    locstate = 'MARKING'
}

export function btmarmcButtons( that: StateMachine ) {
    console.log('racedashstart buttons btmarmcButtons()')

    $('#grdPort').animate({
         opacity: 0.25 //use more parameter for effect
    }, 1000, function() { $(this).text( '' ) })
    $('#grdStarboard').animate({
         opacity: 0.25 //use more parameter for effect
    }, 1000, function() { $(this).text( '' ) })
    $('#grdCenter').animate({
         opacity: 0.25 //use more parameter for effect
    }, 1000, function() { $(this).text( '' ) })

    $('#grdCenter').promise().done(function() {
        console.log('racedashstart buttons btmarmcButtons(): initButtons()')
        $("#grdPort").css({ opacity: 1.0 });
        $("#grdStarboard").css({ opacity: 1.0 });
        $("#grdCenter").css({ opacity: 1.0 });
        initButtons( that )
    })
}

$('body').on('click', '#btnDropPort', function(event) {
    console.log('racedashstart buttons event click #btnDropPort')
    console.log('racedashstart buttons - state: ', fsm.state)
    if ( !$('#btnDropPort').css('disabled') ) {
        console.log('racedashstart buttons - button enabled and pressed')
        if ( fsm.is('marking') )
            fsm.btnportd1()
        else if ( fsm.is('onemark') ) {
            locstate = 'MARKED'
            fsm.btnportd2()
        }
        $('#btnDropPort').addClass('active')
        $('#btnDropPort').addClass('disabled')
    }
    console.log('racedashstart buttons - state now: ', fsm.state)
})

$('body').on('click', '#btnDropStarboard', function(event) {
    console.log('racedashstart buttons event click #btnDropStarboard')
    console.log('racedashstart buttons - state: ', fsm.state)
    if ( !$('#btnDropStarboard').css('disabled') ) {
        console.log('racedashstart buttons - button enabled and pressed')
        if ( fsm.is('marking') )
            fsm.btnstbdd1()
        else if ( fsm.is('onemark') ) {
            locstate = 'MARKED'
            fsm.btnstbdd2()
        }
        $('#btnDropStarboard').addClass('active')
        $('#btnDropStarboard').addClass('disabled')
    }
    console.log('racedashstart buttons - state now: ', fsm.state)
})
