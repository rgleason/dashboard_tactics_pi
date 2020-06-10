/* $Id: button.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import "bootstrap-sass"

var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts
var rdsInitMsg: string = (window as any).instrulang.rdsInitMsg

var locstate: string = ''

export function initButtons() {
    console.log('racedash buttons initButtons()')
    $('#wllMessage').text( rdsInitMsg )
    $('#btnArm').removeClass('disabled')
    locstate = 'RDY'

}

$('#btnArm').on('click', function(event) {
    console.log('racedash buttons event click #btnArm')
    $(this).button('toggle')
})
