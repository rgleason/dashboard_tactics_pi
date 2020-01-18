/* $Id: kbd.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor for a state machine to deal with keyboard events

var dbglevel = window.instrustat.debuglevel

var controlKey
var arrowUpKey
var arrowDownKey
var notBothCtrlUp
var notBothCtrlDown

export function kbdInit() {
    controlKey = false
    arrowUpKey = false
    arrowDownKey = false
    notBothCtrlUp = true
    notBothCtrlDown = true
}

document.addEventListener('keydown', function(e) {
    if (event.defaultPrevented)
        return
    if ( e.ctrlKey )
        controlKey = true
    if ( (e.key == 'Up') || (e.key == 'ArrowUp') )
        arrowUpKey = true
    if ( (e.key == 'Down') || (e.key == 'ArrowDown') )
        arrowDownKey = true
    if ( notBothCtrlUp ) {
        if ( controlKey && arrowUpKey ) {
            if ( dbglevel > 2 )
                console.log('kbd.js: Ctrl+Up')
            window.iface.setswapdisp()
            notBothCtrlUp = false
        }
    }
    if ( notBothCtrlDown ) {
        if ( controlKey && arrowDownKey ) {
            if ( dbglevel > 2 )
                console.log('kbd.js: Ctrl+Dow')
            // fire the event
            notBothCtrlDown = false
        }
    }
});

document.addEventListener('keyup', function(e) {
    if (event.defaultPrevented)
        return
    if ( !e.ctrlKey )
        controlKey = false
    if ( (e.key == 'Up') || (e.key == 'ArrowUp') )
        arrowUpKey = false
    if ( (e.key == 'Down') || (e.key == 'ArrowDown') )
        arrowDownKey = false
    if ( !notBothCtrlUp ) {
        if ( !controlKey || !arrowUpKey )
            notBothCtrlUp = true
    }
    if ( !notBothCtrlDown ) {
        if ( !controlKey || !arrowDownKey )
            notBothCtrlDown = true
    }
});


