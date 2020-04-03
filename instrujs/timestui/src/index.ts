/* $Id: index.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import {packagename, version} from '../../src/version'
console.log('timestui ', packagename(), ' ', version())
var dbglevel = (window as any).instrustat.debuglevel

import '../../src/iface.js'
import '../sass/style.scss'
import {setSkPathFontResizingStyle} from './css'

import {kbdInit} from '../../src/kbd'
import unloadWebKitIEScrollBars from './unloadwebkitiescrollbars'

// InfluxDB client module
import {initIdbClient} from './idbclient'
initIdbClient()

// State Machine Services
import {createStateMachine} from './statemachine'
import visualize from '../../src/state-machine-visualize'

if ( dbglevel > 0 ) console.log('index.js - creating the finite state machine')
var fsm = createStateMachine()
if ( dbglevel > 0 ) console.log('fsm created - state: ', fsm.state)
try {
    var dot: any = visualize( fsm );
    (window as any).iface.setgraphwizdot( dot )
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

// Create the transitional events (the IE way, sorry!) for clieant messages
var bottom: HTMLElement | null = document.getElementById( 'bottom' )
if (!bottom) {
    throw 'timestui: init: no element: bottom'
}

// UID and configuration file
var eventsetid: Event = document.createEvent('Event')
eventsetid.initEvent('setid', false, false)
bottom.addEventListener('setid', ((event: CustomEvent) => {
    if ( dbglevel > 1 ) console.log('pollhascfg() - attempt to setid() transition.')
    try {
        fsm.setid()
    }
    catch( error ) {
        console.error(
            'Event:  setid: fsm.setid() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
    var pollhascfg = (function () {
        if ( dbglevel > 1 ) console.log('pollhascfg() - waiting on "hasid" state')
        if ( fsm.is('hasid') ) {
            var hascfg = true
            if ( fsm.conf === null )
                hascfg = false
            else if ( (fsm.conf.path === null) || (fsm.conf.path === '') )
                hascfg = false
            if ( hascfg ) {
                try {
                    fsm.hascfg()
                }
                catch( error ) {
                    console.error(
                        'index.js:  pollhascfg(): fsm.hascfg() transition failed, error: ', error,
                        ' current state: ', fsm.state)
                }
            }
            else {
                try {
                    fsm.nocfg()
                }
                catch( error ) {
                    console.error(
                        'index.js: pollhascfg(): fsm.nocfg() transition failed, error: ', error,
                        ' current state: ', fsm.state)
                }
            }
        }
        else {
            var n: number
            n = window.setTimeout(() => pollhascfg, 100)
        }
    }()) // do selection of the next action in the routing once ID has been set, or not
}) as EventListener);  // hey non-semicolon-TS-person - this is needed!
(window as any).iface.regeventsetid( bottom, eventsetid )

// All available paths have been set
var eventsetall: Event = document.createEvent('Event')
eventsetall.initEvent('setall', false, false)
bottom.addEventListener('setall', ((event: Event) => {
    console.error(
        'Event:  setall: error: timestui does not require all paths')
}) as EventListener);  // hey non-semicolon-TS-person - this is needed!
(window as any).iface.regeventsetall( bottom, eventsetall )

// All available DB schema paths have been set
var eventsetalldb: Event = document.createEvent('Event')
eventsetalldb.initEvent('setalldb', false, false)
bottom.addEventListener('setalldb', ((event: Event) => {
    try {
        fsm.setalldb()
    }
    catch( error ) {
        console.error(
            'Event:  setalldb: fsm.setalldb() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventsetalldb( bottom, eventsetalldb )

// DB path did not contain wanted path, rescan
var eventrescan: Event = document.createEvent('Event')
eventrescan.initEvent('rescan', false, false)
bottom.addEventListener('rescan', ((event: Event) => {
    try {
        fsm.rescan()
    }
    catch( error ) {
        console.error(
            'Event:  rescan: fsm.rescan() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventrescan( bottom, eventrescan )

// Selection of a path has been made
var eventselected: Event = document.createEvent('Event')
eventselected.initEvent('selected', false, false)
bottom.addEventListener('selected', ((event: Event) => {
    try {
        fsm.selected()
    }
    catch( error ) {
        console.error(
            'Event:  selected: fsm.selected() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventselected( bottom, eventselected )

// Selection of a path has been acknowledged
var eventacksubs: Event = document.createEvent('Event')
eventacksubs.initEvent('acksubs', false, false)
bottom.addEventListener('acksubs', ((event: Event) => {
    console.error(
        'Event:  acksubs: error: timestui does not ask for path subscription')
}) as EventListener);
(window as any).iface.regeventacksubs( bottom, eventacksubs )

// Requested database schema is now available
var eventackschema: Event = document.createEvent('Event')
eventackschema.initEvent('ackschema', false, false)
bottom.addEventListener('ackschema', ((event: Event) => {
    try {
        fsm.ackschema()
    }
    catch( error ) {
        console.error(
            'Event:  acksubs: fsm.ackschema() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventackschema( bottom, eventackschema )

// Request to fetch new data from DB
var eventgetnew: Event = document.createEvent('Event')
eventgetnew.initEvent('getnew', false, false)
bottom.addEventListener('getnew', ((event: Event) => {
    try {
        fsm.getnew()
    }
    catch( error ) {
        console.error(
            'Event: getnew: fsm.getnew() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventgetnew( bottom, eventgetnew )

// New data fetch has been launched
var eventgetlaunch: Event = document.createEvent('Event')
eventgetlaunch.initEvent('getlaunch', false, false)
bottom.addEventListener('getlaunch', ((event: Event) => {
    try {
        fsm.getlaunch()
    }
    catch( error ) {
        console.error(
            'Event: getlaunch: fsm.getlaunch() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventgetlaunch( bottom, eventgetlaunch )

// New data is availale
var eventnewdata: Event = document.createEvent('Event')
eventnewdata.initEvent('newdata', false, false)
bottom.addEventListener('newdata', ((event: Event) => {
    try {
        fsm.newdata()
    }
    catch( error ) {
        console.error(
            'Event: newdata: fsm.newdata() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
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
            'Event: errdata: fsm.errdata() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventerrdata( bottom, eventerrdata )

// Retry Asynchronous data retrieval from DB
var eventretryget: Event = document.createEvent('Event')
eventretryget.initEvent('retryget', false, false)
bottom.addEventListener('retryget', ((event: Event) => {
    try {
        fsm.retryget()
    }
    catch( error ) {
        console.error(
            'Event: retryget: fsm.retryget() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventretryget( bottom, eventretryget )

// Change of configuration has been requested
var eventchgconf: Event = document.createEvent('Event')
eventchgconf.initEvent('chgconf', false, false)
bottom.addEventListener('chgconf', ((event: Event) => {
    try {
        fsm.chgconf()
    }
    catch( error ) {
        console.error(
            'Event:  chgconf: fsm.chgconf() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventchgconf( bottom, eventchgconf )

// Luminosity
var eventluminsty: Event = document.createEvent('Event')
eventluminsty.initEvent('luminsty', false, false)
bottom.addEventListener('luminsty', ((event: Event) => {
    try {
        fsm.luminsty()
    }
    catch( error ) {
        console.error(
            'Event:  luminsty: fsm.luminsty() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventluminsty( bottom, eventluminsty )

// Keyboard event requires to swap the display format
kbdInit()
var eventswapdisp: Event = document.createEvent('Event')
eventswapdisp.initEvent('swapdisp', false, false)
bottom.addEventListener('swapdisp', ((event: Event) => {
    console.error(
        'Event:  swapdisp: error: timestui does not deal with this event (for now)')
}) as EventListener);
(window as any).iface.regeventswapdisp( bottom, eventswapdisp )

// The instrument has a persistent configuration object, close gracefully
var eventclosing: Event = document.createEvent('Event')
eventclosing.initEvent('closing', false, false)
bottom.addEventListener('closing', ((event: Event) => {
    try {
        fsm.closing()
    }
    catch( error ) {
        console.error(
            'Event:  closing: fsm.closing() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventclosing( bottom, eventclosing )

/* Since now no other events apart the window load(), we need to await here until
   it has been executed, before continuing to truy event driven operation */
function pollinitga () {
    if ( dbglevel > 0 ) console.log('pollinitga() - waiting for initga, now: ', fsm.state)
    if ( fsm.is('initga') ) {
        try {
            fsm.initok()
        }
        catch( error ) {
            console.error(
                'index.js:  fsm.initok() transition failed, error: ', error,
                ' current state: ', fsm.state)
        }
    } else {
        setTimeout(pollinitga, 100)
    }
}

pollinitga() // do _everything_ in the routing once condition met

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
            console.error('loading: fsm.loaded() transition failed, error: ', error)
        }
    }, false)

/* ------------------------------------------ */
