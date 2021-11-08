/* $Id: index.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import {packagename, version} from '../../src/version'
console.log('timestui ', packagename(), ' ', version())
var dbglevel = (window as any).instrustat.debuglevel

import '../../src/iface.js'
// High level  operations available on iface.js
import {
    initIfaceOps, askWaitGetUID, ignoreSetAllPathsEvent, createSetAllDbEvent,
    createSetRescanEvent, createSetSelectedEvent,
    ignoreAckSubscriptionEvent, createAckDbSchemaEvent,
    ignoreSetGetNewEvent, createSetRetryGetEvent, createSetChgConfEvent,
    ignoreSetSwapDispEvent, createSetLuminosityEvent, createSetClosingEvent,
    ignoreSetGetFeetEvent, ignoreSetGetNoFeetEvent,
    ignoreSetChkRdyEvent, ignoreSetChkNoRdyEvent,
    ignoreSetUserslEvent, ignoreSetNoUserslEvent,
    ignoreSetMarkAckEvent, ignoreSetDataAckEvent,
    ignoreSetNewSlDataEvent, ignoreSetSldStopEvent,
    ignoreSetMrkDataAckEvent, ignoreSetNewMrkDataEvent,
    ignoreSetMarkMuteEvent, ignoreSetMarkUnMuteEvent
} from '../../src/ifaceops'

import '../sass/style.scss'
import {setSkPathFontResizingStyle} from './css'

import {kbdInit} from '../../src/kbd'
import unloadWebKitIEScrollBars from './unloadwebkitiescrollbars'

import {getAddedDataCount} from './chart'

// InfluxDB client module
import {initIdbClient} from './idbclient'
initIdbClient()

// State Machine Services
import {createStateMachine} from './statemachine'
import visualize from '../../src/state-machine-visualize'

if ( dbglevel > 0 )
    console.log('index.ts - creating the finite state machine')
var fsm = createStateMachine()
if ( dbglevel > 0 )
    console.log('fsm created - state: ', fsm.state)
try {
    var dot: any = visualize( fsm );
    (window as any).iface.setgraphwizdot( dot )
    if ( dbglevel > 0 )
        console.log('index.ts: state machine GraphWiz presentation available through iface.js')
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
    throw 'timestui: init: no element: bottom'
}

// Initiate and set the event handling on events available on iface.js
initIfaceOps ( fsm, bottom )

askWaitGetUID()

ignoreSetAllPathsEvent()

createSetAllDbEvent()

createSetRescanEvent()

createSetSelectedEvent()

ignoreAckSubscriptionEvent()

createAckDbSchemaEvent()

ignoreSetGetNewEvent() // see pollshowdata()

// New data is available - keep up the data streaming by transitional states
var eventnewdata: Event = document.createEvent('Event')
eventnewdata.initEvent('newdata', false, false)
bottom.addEventListener('newdata', ((event: Event) => {
    try {
        fsm.newdata()
    }
    catch( error ) {
        console.error(
            'Event: newdata: fsm.newdata() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
    /*
      Showdata is a transitional state - it has one entry transition
      1/ newdata - from waitdata - DB query has succeeded
      Two possible exits
      2/ getnew - new data query cycle
      3/ chgconf - mouse event, if occurs to ask for a config change
      Since FSM cannot launch its own event we need to serve getnew transition
    */
    var pollshowdata: () => void
    var prevNewDataCnt: number = 0
    var nofRemainedSame: number = 0;
    (pollshowdata = function () {
        var newDataCnt = getAddedDataCount()
        var painting: boolean = true
        if ( newDataCnt !== prevNewDataCnt) {
            prevNewDataCnt = newDataCnt
        }
        else {
            nofRemainedSame++
            if ( nofRemainedSame >= 1 ) // (450ms+del)=~500ms when chart follows
                painting = false
        }
        if ( dbglevel > 4 )
            console.log('pollshowdata() - waiting for showdata, now: ',
            fsm.state, ' painting ', painting, ' (',
            newDataCnt, ',', prevNewDataCnt, ',', nofRemainedSame, ')')
        if ( fsm.is('showdata') && !painting ) {
            try {
                fsm.getnew()
            }
            catch( error ) {
                console.error(
                    'index.ts:  fsm.newget() transition failed, error: ',
                    error.message, ' current state: ', fsm.state)
            }
        } else {
            window.setTimeout(pollshowdata, 450)
        }
    })() // make the transition from newdata to getnew by polling the state
}) as EventListener);
(window as any).iface.regeventnewdata( bottom, eventnewdata )

// Asynchronous data retrieval from DB has failed
var eventerrdata: Event = document.createEvent('Event')
eventerrdata.initEvent('errdata', false, false)
bottom.addEventListener('errdata', ((event: Event) => {
    try {
        fsm.errdata()
    }
    catch( error ) {
        console.error(
            'Event: errdata: fsm.errdata() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
    /*
      Nodata is a transitional state - it has one entry transition
      1/ errdata - from waitdata - DB query has failed
      Two possible exits
      2/ retryget - new data query cycle launched despite error
      3/ chgconf - mouse event, if occurs to ask for a config change
      Since FSM cannot launch its own event we need to serve retryget transition
    */
    var pollerrdata: () => void
    (pollerrdata = function () {
        if ( dbglevel > 0 )
            console.log('pollerrdata() - waiting for nodata, now: ', fsm.state)
        if ( fsm.is('nodata') ) {
            try {
                fsm.retryget()
            }
            catch( error ) {
                console.error(
                    'index.ts:  fsm.retryget() transition failed, error: ',
                    error.message, ' current state: ', fsm.state)
            }
        } else {
            window.setTimeout(pollerrdata, 100)
        }
    })() // make the transition from errdata to retryget by polling the state
}) as EventListener);
(window as any).iface.regeventerrdata( bottom, eventerrdata )

// Retry Asynchronous data retrieval from DB
createSetRetryGetEvent()

createSetChgConfEvent()

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
ignoreSetMrkDataAckEvent()
ignoreSetNewMrkDataEvent()
ignoreSetMarkMuteEvent()
ignoreSetMarkUnMuteEvent()

/* Since now no other events apart the window load(), we need to await here until
   it has been executed, before continuing to try event driven operation */
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
        if ( dbglevel > 0 ) console.log('index.ts - state: ', fsm.state)

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
