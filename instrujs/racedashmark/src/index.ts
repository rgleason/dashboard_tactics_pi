/* $Id: index.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import {packagename, version} from '../../src/version'
console.log('racedashstart ', packagename(), ' ', version())
var dbglevel = (window as any).instrustat.debuglevel

import '../../src/iface.js'
// High level  operations available on iface.js
import {
    initIfaceOps, askWaitGetUID, ignoreSetAllPathsEvent, ignoreSetAllDbEvent,
    ignoreSetRescanEvent, ignoreSetSelectedEvent,
    ignoreAckDbSchemaEvent, ignoreSetGetNewEvent,
    ignoreNewDataEvent, ignoreSetErrDataEvent,
    ignoreSetChgConfEvent, ignoreSetRetryGetEvent,
    createSetMrkDataAckEvent, ignoreAckSubscriptionEvent,
    ignoreSetSwapDispEvent, createSetLuminosityEvent, createSetClosingEvent,
    ignoreSetGetFeetEvent, ignoreSetGetNoFeetEvent,
    ignoreSetChkRdyEvent, ignoreSetChkNoRdyEvent,
    ignoreSetUserslEvent, ignoreSetNoUserslEvent,
    ignoreSetMarkAckEvent, ignoreSetDataAckEvent,
    ignoreSetNewSlDataEvent, ignoreSetSldStopEvent
} from '../../src/ifaceops'


import '../sass/style.scss'

import {kbdInit} from '../../src/kbd'
import unloadWebKitIEScrollBars from './unloadwebkitiescrollbars'

// State Machine Services
import {createStateMachine} from './statemachine'
import visualize from '../../src/state-machine-visualize'

import { instruNotRdyAlert, cppAckMuteChart, cppAckResumeChart } from './cppMark'

if ( dbglevel > 0 )
    console.log('index.ts - creating the finite state machine')
var fsm = createStateMachine()
if ( dbglevel > 0 )
    console.log('fsm created - state: ', fsm.state)
try {
    var dot: any = visualize( fsm );
    (window as any).iface.setgraphwizdot( dot )
    if ( dbglevel > 0 )
        console.log('index.ts: state machine GraphWiz presentation available through iface.js w/ getgraphwizdot()')
}
catch( error ) {
    console.error('index.ts: state machine visualize, error: ',
                  error.message)
}
try {
    fsm.init()
}
catch( error ) {
    console.error('index.ts: fsm.init() transition failed, error: ',
                  error.message)
}

// Create the transitional events (the IE way, sorry!) for client messages
var bottom: HTMLElement | null = document.getElementById( 'bottom' )
if (bottom === null) {
    throw 'racedashstart: init: no element: bottom'
}

// Initiate and set the event handling on events available on iface.js
initIfaceOps ( fsm, bottom )
// get the ID and conf if available
askWaitGetUID()

ignoreSetAllPathsEvent()

ignoreSetAllDbEvent()

ignoreSetRescanEvent()

ignoreSetSelectedEvent()

ignoreAckSubscriptionEvent()

ignoreAckDbSchemaEvent()

ignoreSetGetNewEvent()

ignoreNewDataEvent()

ignoreSetErrDataEvent()

ignoreSetChgConfEvent()

ignoreSetRetryGetEvent()

createSetMrkDataAckEvent()

// New set of  data and back-end process status information has arrived
var eventnewmrkdata: Event = document.createEvent('Event')
var instruHasBeenRdy: boolean = true
var pollingGoingOnMsgDone: boolean = false
eventnewmrkdata.initEvent('newmrkdata', false, false)
bottom.addEventListener('newmrkdata', ((event: Event) => {
    // alert user if the instrument are not receiving data
    var instOK: boolean = (window as any).iface.getmrkmrkinstrurdy()
    if ( !instOK && instruHasBeenRdy )
        instruNotRdyAlert()
    instruHasBeenRdy = instOK
    // survey by polling the active route and change state accordingly
    var activeRte: boolean = (window as any).iface.getmrkhasactiveroute()
    console.log( 'activeRte: ', activeRte, ' fsm.state: ', fsm.state )
    if ( fsm.is('running') && activeRte && instOK ) {
        try {
            fsm.newmrkdata()
        }
        catch( error ) {
            console.error(
                'Event:  eventnewmrkdata: fsm.newmrkdata() transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }
    else if ( fsm.is('waiting') && activeRte ) {
        try {
            fsm.actvrte()
        }
        catch( error ) {
            console.error(
                'Event:  eventnewmrkdata: fsm.actvrte() transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }
    else if ( fsm.is('running') && !activeRte ) {
        try {
            fsm.noroute()
        }
        catch( error ) {
            console.error(
                'Event:  eventnewmrkdata: fsm.noroute() transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }
    else if ( !pollingGoingOnMsgDone) {
        console.log(
            'Event:  eventnewmrkdata: polling has started, current state: ',
            fsm.state)
        pollingGoingOnMsgDone = true
    }
}) as EventListener);
(window as any).iface.regeventnewmrkdata( bottom, eventnewmrkdata )

// Request to mute the chart overlay - just clear after the acknowledgement
var eventmrkmteaack: Event = document.createEvent('Event')
eventmrkmteaack.initEvent('mrkmteaack', false, false)
bottom.addEventListener('mrkmteaack', ((event: Event) => {
    cppAckMuteChart()
}) as EventListener);
(window as any).iface.regeventmrkmteaack( bottom, eventmrkmteaack )

// Request to unmute the chart overlay - just clear after the acknowledgement
var eventmrkumteack: Event = document.createEvent('Event')
eventmrkumteack.initEvent('mrkumteack', false, false)
bottom.addEventListener('mrkumteack', ((event: Event) => {
    cppAckResumeChart()
}) as EventListener);
(window as any).iface.regeventmrkumteack( bottom, eventmrkumteack )

// Common housekeeping requests
createSetLuminosityEvent()
// Keyboard event requires to swap the display format
kbdInit()
// Not served (yet)
ignoreSetSwapDispEvent()
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

/* Since now no other events apart the window load(), we need to await here until
   it has been executed, before continuing to truy event driven operation */


var reloadDelay:number = 2
var pollinitga: () => void
(pollinitga = function() {
    if ( dbglevel > 0 )
        console.log('pollinitga() - waiting for initga, now: ',
         fsm.state)
    if ( fsm.is('initga') ) {
        if ( reloadDelay === 0 ) {
            try {
                fsm.initok()
            }
            catch( error ) {
                console.error(
                    'index.ts:  fsm.initok() transition failed, error: ',
                    error.message, ' current state: ', fsm.state)
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
        setTimeout(pollinitga, 100)
    }
})() // do _everything_ in the routing once condition met



/* ------------------------------------------ */

var unloadScrollBars = function() {
    unloadWebKitIEScrollBars()
}

window.addEventListener('load',
    function() {
        // if ( dbglevel > 0 ) console.log('index.ts - state: ', fsm.state)

        unloadScrollBars()

        // Loading state done
        try {
            fsm.loaded()
        }
        catch( error ) {
            console.error('loading: fsm.loaded() transition failed, error: ',
            error.message)
        }
    }, false)

/* ------------------------------------------ */
