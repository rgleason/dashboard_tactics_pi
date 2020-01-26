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
    if ( dbglevel > 0 )
        console.log('kbd.js: kbdInit()')
    controlKey = false
    arrowUpKey = false
    arrowDownKey = false
    notBothCtrlUp = true
    notBothCtrlDown = true
}

function onKeyDown( e ) {
    if (event.defaultPrevented)
        return
    var handled = false
    if ( dbglevel > 3 )
        console.log('kbd.js: onKeyDown(), e:', e)
    if ( e.ctrlKey )
        controlKey = true
    if ( (e.keyIdentifier === 'Up') || (e.key === 'Up') || (e.key === 'ArrowUp') ) {
        arrowUpKey = true
        handled = true
    }
    if ( (e.keyIdentifier === 'Down') || (e.key === 'Down') || (e.key === 'ArrowDown') ) {
        arrowDownKey = true
        handled = true
    }
    if ( notBothCtrlUp ) {
        if ( controlKey && arrowUpKey ) {
            if ( dbglevel > 2 )
                console.log('kbd.js: Ctrl+Up')
            window.iface.setswapdisp( -1 )
            notBothCtrlUp = false
        }
    }
    if ( notBothCtrlDown ) {
        if ( controlKey && arrowDownKey ) {
            if ( dbglevel > 2 )
                console.log('kbd.js: Ctrl+Dow')
            window.iface.setswapdisp( 1 )
            notBothCtrlDown = false
        }
    }
    if (handled)
        event.preventDefault()
}

function onKeyUp( e ) {
    if (event.defaultPrevented)
        return
    var handled = false
    if ( dbglevel > 3 )
        console.log('kbd.js: onKeyUp(), e:', e)
    if ( !e.ctrlKey )
        controlKey = false
    if ( (e.keyIdentifier === 'Up') || (e.key === 'Up') || (e.key === 'ArrowUp') ) {
        arrowUpKey = false
        handled = true
    }
    if ( (e.keyIdentifier === 'Down') || (e.key === 'Down') || (e.key === 'ArrowDown') ) {
        arrowDownKey = false
        handled = true
    }
    if ( !notBothCtrlUp ) {
        if ( !controlKey || !arrowUpKey )
            notBothCtrlUp = true
    }
    if ( !notBothCtrlDown ) {
        if ( !controlKey || !arrowDownKey )
            notBothCtrlDown = true
    }
    if (handled)
        event.preventDefault()
}

document.addEventListener('keydown', onKeyDown, false)
document.addEventListener('keyup', onKeyUp, false)

