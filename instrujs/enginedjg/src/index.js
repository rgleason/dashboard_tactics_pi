/* $Id: index.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import {packagename, version} from '../../src/version'
console.log('enginedjg ', packagename(), ' ', version())
var dbglevel = window.instrustat.debuglevel

import '../sass/style.scss'
import {kbdInit} from '../../src/kbd'
import {createStateMachine} from './statemachine'
import {setSkPathFontResizingStyle} from './css'
import visualize from '../../src/state-machine-visualize'

// we access it with window.iface but this is needed once, to get it in...
var iface = require('exports-loader?iface!../../src/iface.js')

// Use the TypeScript services, JavaScript export of it for event registering
import {
    initIfaceOps, askWaitGetUID, createSetAllPathsEvent,
    ignoreSetRescanEvent, createSetSelectedEvent, createSetAllDbEvent,
    createAckSubscriptionEvent, ignoreAckDbSchemaEvent,
    ignoreSetGetNewEvent, createNewDataEvent, ignoreSetErrDataEvent,
    ignoreSetRetryGetEvent, createSetChgConfEvent,
    createSetSwapDispEvent, createSetLuminosityEvent, createSetClosingEvent,
    ignoreSetGetFeetEvent, ignoreSetGetNoFeetEvent,
    ignoreSetChkRdyEvent, ignoreSetChkNoRdyEvent,
    ignoreSetUserslEvent, ignoreSetNoUserslEvent,
    ignoreSetMarkAckEvent, ignoreSetDataAckEvent,
    ignoreSetNewSlDataEvent, ignoreSetSldStopEvent,
    ignoreSetMarkMuteEvent, ignoreSetMarkUnMuteEvent
} from '../../src/ifaceopsJS.js'

// State Machine Service
if ( dbglevel > 0 ) console.log('index.js - creating the finite state machine')
var fsm = createStateMachine()
if ( dbglevel > 0 ) console.log('fsm created - state: ', fsm.state)
try {
    var dot = visualize( fsm )
    window.iface.setgraphwizdot( dot )
    if ( dbglevel > 0 ) console.log('index.js: state machine GraphWiz presentation available through iface.js')
}
catch( error ) {
    console.error('index.js: state machine visualize, error: ', error)
}
try {
    fsm.init()
}
catch( error ) {
    console.error('index.js: fsm.init() transition failed, error: ', error)
}

// Create the transitional events (the IE way, sorry!) for client messages
var bottom = document.getElementById ('bottom' )

// Initiate and set the event handling on events available on iface.js
initIfaceOps ( fsm, bottom )
// get the ID and conf if available
askWaitGetUID()

createSetAllPathsEvent()

createSetAllDbEvent()

ignoreSetRescanEvent()

createSetSelectedEvent()

createAckSubscriptionEvent()

ignoreAckDbSchemaEvent()

ignoreSetGetNewEvent()

createNewDataEvent()

ignoreSetErrDataEvent()

ignoreSetRetryGetEvent()

createSetChgConfEvent()

// Common housekeeping requests
createSetLuminosityEvent()
// Keyboard event requires to swap the display format
kbdInit()
// This insstrument is controlled with keyboard up/down arrows
createSetSwapDispEvent()
// The instrument has a persistent configuration object, close gracefully
createSetClosingEvent()

// Let's register to events not used here - better have a handler, though
ignoreSetGetFeetEvent()
ignoreSetGetNoFeetEvent()
ignoreSetChkRdyEvent()
ignoreSetChkNoRdyEvent()
ignoreSetUserslEvent()
ignoreSetNoUserslEvent()
ignoreSetMarkAckEvent()
ignoreSetDataAckEvent()
ignoreSetNewSlDataEvent()
ignoreSetSldStopEvent()
ignoreSetMarkMuteEvent()
ignoreSetMarkUnMuteEvent()

/* Since now no other events apart the window load(), we need to await here until
   it has been executed, before continuing to truy event driven operation */
var reloadDelay = 2
var pollinitga
(pollinitga = function() {
    if ( dbglevel > 0 )
        console.log('pollinitga() - waiting for initga, now: ', fsm.state)
    if ( fsm.is('initga') ) {
        if ( reloadDelay === 0 ) {
            try {
                fsm.initok()
            }
            catch( error ) {
                console.error(
                    'index.js:  fsm.initok() transition failed, error: ', error,
                    ' current state: ', fsm.state)
            }
        }
        else {
            reloadDelay--
            if ( dbglevel > 1 )
                console.log('pollinitga() - initga+reloadDelay: ', reloadDelay)
            setTimeout(pollinitga, 500)
        }
    }
    else {
        setTimeout(pollinitga, 500)
    }
})() // do _everything_ in the routing once condition met

/* ------------------------------------------ */

var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'hidden' // webkit
    document.body.scroll = 'no' // ie
}

window.addEventListener('load',
    function() {
        if ( dbglevel > 0 ) console.log('index.js - state: ', fsm.state)

        unloadScrollBars()

        // Loading state done
        try {
            fsm.loaded()
        }
        catch( error ) {
            console.error('loading: fsm.loaded() transition failed, error: ', error)
        }
    }, false)

/* ------------------------------------------ */
