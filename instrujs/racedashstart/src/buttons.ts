/* $Id: button.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

 import {StateMachine} from "./statemachine"

var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts

var locstate: string = ''

var $elemPnlCenter = $('<div id="pnlDistLine" class="panel panel-default day">' +
'<div id="pnlDistLineHdr" class="panel-heading text-center day"></div>' +
'<div id="pnlDistLineBdy" class="panel-body text-center day"></div>' +
'</div>')

var $elemBtnArm = $('<button id="btnArm" type="button" class="btn btn-lg btn-warning disabled">' +
'</button>' )


export function initButtons( that: StateMachine ) {
    console.log('racedash buttons initButtons()')
    $('#pnlMsgClockBdy').text( (window as any).instrulang.rdsInitMsg )
    $elemPnlCenter.appendTo( $('#grdCenter') )
    $elemBtnArm.appendTo( $('#pnlDistLineBdy') )
    $('#btnArm').text( (window as any).instrulang.rdsBtnArmTxt )
    $('#btnArm').removeClass('disabled')
    locstate = 'RDY'
}

$('#btnArm').on('click', function(event) {
    console.log('racedash buttons event click #btnArm')
    $(this).button('toggle')
})

// Button ready
// <button id="btnArm" type="button" class="btn btn-lg btn-warning disabled">
//     Ready?
// </button>


// Port side
// <button id="btnDropport" type="button" class="btn btn-lg btn-danger disabled">
// Port Mark
// </button>

// Starboard side
// <button id="btnDropstbd" type="button" class="btn btn-lg btn-success disabled">
//     Stbd Mark
// </button>

// Panel
// <div id="pnlDistLine" class="panel panel-default day">
//     <div id="pnlDistLineHdr" class="panel-heading text-center day"></div>
//     <div id="pnlDistLineBdy" class="panel-body text-center day"></div>
// </div>
