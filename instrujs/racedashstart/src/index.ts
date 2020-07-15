/* $Id: index.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import {packagename, version} from '../../src/version'
console.log('racedashstart ', packagename(), ' ', version())
var dbglevel = (window as any).instrustat.debuglevel

import '../../src/iface.js'
import '../sass/style.scss'

import {kbdInit} from '../../src/kbd'
import unloadWebKitIEScrollBars from './unloadwebkitiescrollbars'

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

// Create the transitional events (the IE way, sorry!) for clieant messages
var bottom: HTMLElement | null = document.getElementById( 'bottom' )
if (bottom === null) {
    throw 'racedashstart: init: no element: bottom'
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
            'Event:  setid: fsm.setid() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
    var pollhascfg: () => void
    (pollhascfg= function () {
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
                        'index.ts:  pollhascfg(): fsm.hascfg() transition failed, error: ',
                        error.message, ' current state: ', fsm.state)
                }
            }
            else {
                try {
                    fsm.nocfg()
                }
                catch( error ) {
                    console.error(
                        'index.ts: pollhascfg(): fsm.nocfg() transition failed, error: ',
                        error.message, ' current state: ', fsm.state)
                }
            }
        }
        else {
            window.setTimeout(pollhascfg, 100)
        }
    })(); // do selection of the next action in the routing once ID has been set, or not
}) as EventListener);  // hey non-semicolon-TS-person - this is needed!
(window as any).iface.regeventsetid( bottom, eventsetid )

// All available paths have been set
// var eventsetall: Event = document.createEvent('Event')
// eventsetall.initEvent('setall', false, false)
// bottom.addEventListener('setall', ((event: Event) => {
//     console.error(
//         'Event:  setall: error: racedashstart does not require all paths')
// }) as EventListener);  // hey non-semicolon-TS-person - this is needed!
// (window as any).iface.regeventsetall( bottom, eventsetall )

// All available DB schema paths have been set
// var eventsetalldb: Event = document.createEvent('Event')
// eventsetalldb.initEvent('setalldb', false, false)
// bottom.addEventListener('setalldb', ((event: Event) => {
//     try {
//         fsm.setalldb()
//     }
//     catch( error ) {
//         console.error(
//             'Event:  setalldb: fsm.setalldb() transition failed, error: ',
//             error.message, ' current state: ', fsm.state)
//     }
// }) as EventListener);
// (window as any).iface.regeventsetalldb( bottom, eventsetalldb )

// DB path did not contain wanted path, rescan
// var eventrescan: Event = document.createEvent('Event')
// eventrescan.initEvent('rescan', false, false)
// bottom.addEventListener('rescan', ((event: Event) => {
//     try {
//         fsm.rescan()
//     }
//     catch( error ) {
//         console.error(
//             'Event:  rescan: fsm.rescan() transition failed, error: ',
//             error.message, ' current state: ', fsm.state)
//     }
// }) as EventListener);
// (window as any).iface.regeventrescan( bottom, eventrescan )

// Selection of a path has been made
// var eventselected: Event = document.createEvent('Event')
// eventselected.initEvent('selected', false, false)
// bottom.addEventListener('selected', ((event: Event) => {
//     try {
//         fsm.selected()
//     }
//     catch( error ) {
//         console.error(
//             'Event:  selected: fsm.selected() transition failed, error: ',
//             error.message, ' current state: ', fsm.state)
//     }
// }) as EventListener);
// (window as any).iface.regeventselected( bottom, eventselected )

// Selection of a path has been acknowledged
// var eventacksubs: Event = document.createEvent('Event')
// eventacksubs.initEvent('acksubs', false, false)
// bottom.addEventListener('acksubs', ((event: Event) => {
//     console.error(
//         'Event:  acksubs: error: racedashstart does not ask for path subscription')
// }) as EventListener);
// (window as any).iface.regeventacksubs( bottom, eventacksubs )

// Requested database schema is now available
// var eventackschema: Event = document.createEvent('Event')
// eventackschema.initEvent('ackschema', false, false)
// bottom.addEventListener('ackschema', ((event: Event) => {
//     try {
//         fsm.ackschema()
//     }
//     catch( error ) {
//         console.error(
//             'Event:  ackschema: fsm.ackschema() transition failed, error: ',
//             error.message, ' current state: ', fsm.state)
//     }
// }) as EventListener);
// (window as any).iface.regeventackschema( bottom, eventackschema )

// Request to fetch new data from DB (pro-forma, see pollshowdata())
// var eventgetnew: Event = document.createEvent('Event')
// eventgetnew.initEvent('getnew', false, false)
// bottom.addEventListener('getnew', ((event: Event) => {
//     console.error(
//             'Event: getnew: this should not be triggered from outside!')
// }) as EventListener);
// (window as any).iface.regeventgetnew( bottom, eventgetnew )

// New data is availale
// var eventnewdata: Event = document.createEvent('Event')
// eventnewdata.initEvent('newdata', false, false)
// bottom.addEventListener('newdata', ((event: Event) => {
//     try {
//         fsm.newdata()
//     }
//     catch( error ) {
//         console.error(
//             'Event: newdata: fsm.newdata() transition failed, error: ',
//             error.message, ' current state: ', fsm.state)
//     }
//     /*
//       Showdata is a transitional state - it has one entry transition
//       1/ newdata - from waitdata - DB query has succeeded
//       Two possible exits
//       2/ getnew - new data query cycle
//       3/ chgconf - mouse event, if occurs to ask for a config change
//       Since FSM cannot launch its own event we need to serve getnew transition
//     */
//     var pollshowdata: () => void
//     var prevNewDataCnt: number = 0
//     var nofRemainedSame: number = 0;
//     (pollshowdata = function () {
//         var newDataCnt = getAddedDataCount()
//         var painting: boolean = true
//         if ( newDataCnt != prevNewDataCnt) {
//             prevNewDataCnt = newDataCnt
//         }
//         else {
//             nofRemainedSame++
//             if ( nofRemainedSame >= 1 ) // (450ms+del)=~500ms when chart follows
//                 painting = false
//         }
//         if ( dbglevel > 4 )
//             console.log('pollshowdata() - waiting for showdata, now: ',
//             fsm.state, ' painting ', painting, ' (',
//             newDataCnt, ',', prevNewDataCnt, ',', nofRemainedSame, ')')
//         if ( fsm.is('showdata') && !painting ) {
//             try {
//                 fsm.getnew()
//             }
//             catch( error ) {
//                 console.error(
//                     'index.ts:  fsm.newget() transition failed, error: ',
//                     error.message, ' current state: ', fsm.state)
//             }
//         } else {
//             window.setTimeout(pollshowdata, 450)
//         }
//     })(); // make the transition from newdata to getnew by polling the state
// }) as EventListener);
// (window as any).iface.regeventnewdata( bottom, eventnewdata )

// Asynchronous data retrieval from DB has failed
// var eventerrdata: Event = document.createEvent('Event')
// eventerrdata.initEvent('errdata', false, false)
// bottom.addEventListener('errdata', ((event: Event) => {
//     try {
//         fsm.errdata()
//     }
//     catch( error ) {
//         console.error(
//             'Event: errdata: fsm.errdata() transition failed, error: ',
//             error.message, ' current state: ', fsm.state)
//     }
//     /*
//       Nodata is a transitional state - it has one entry transition
//       1/ errdata - from waitdata - DB query has failed
//       Two possible exits
//       2/ retryget - new data query cycle launched despite error
//       3/ chgconf - mouse event, if occurs to ask for a config change
//       Since FSM cannot launch its own event we need to serve retryget transition
//     */
//     var pollerrdata: () => void
//     (pollerrdata = function () {
//         if ( dbglevel > 0 )
//             console.log('pollerrdata() - waiting for nodata, now: ', fsm.state)
//         if ( fsm.is('nodata') ) {
//             try {
//                 fsm.retryget()
//             }
//             catch( error ) {
//                 console.error(
//                     'index.ts:  fsm.retryget() transition failed, error: ',
//                     error.message, ' current state: ', fsm.state)
//             }
//         } else {
//             window.setTimeout(pollerrdata, 100)
//         }
//     })() // make the transition from errdata to retryget by polling the state
// }) as EventListener);
// (window as any).iface.regeventerrdata( bottom, eventerrdata )

// Retry Asynchronous data retrieval from DB
// var eventretryget: Event = document.createEvent('Event')
// eventretryget.initEvent('retryget', false, false)
// bottom.addEventListener('retryget', ((event: Event) => {
//     try {
//         fsm.retryget()
//     }
//     catch( error ) {
//         console.error(
//             'Event: retryget: fsm.retryget() transition failed, error: ',
//             error.message, ' current state: ', fsm.state)
//     }
// }) as EventListener);
// (window as any).iface.regeventretryget( bottom, eventretryget )
//
// // Change of configuration has been requested
// var eventchgconf: Event = document.createEvent('Event')
// eventchgconf.initEvent('chgconf', false, false)
// bottom.addEventListener('chgconf', ((event: Event) => {
//     try {
//         fsm.chgconf()
//     }
//     catch( error ) {
//         console.error(
//             'Event:  chgconf: fsm.chgconf() transition failed, error: ',
//             error.message, ' current state: ', fsm.state)
//     }
// }) as EventListener);
// (window as any).iface.regeventchgconf( bottom, eventchgconf )

// The instrument is set to feet unit
var eventgetfeet: Event = document.createEvent('Event')
eventgetfeet.initEvent('getfeet', false, false)
bottom.addEventListener('getfeet', ((event: Event) => {
    try {
        fsm.getfeet()
    }
    catch( error ) {
        console.error(
            'Event:  eventgetfeet: fsm.getfeet() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventgetfeet( bottom, eventgetfeet )

// The instrument is not active and ready
var eventnogetfeet: Event = document.createEvent('Event')
eventnogetfeet.initEvent('nogetfeet', false, false)
bottom.addEventListener('nogetfeet', ((event: Event) => {
    try {
        fsm.nogetfeet()
    }
    catch( error ) {
        console.error(
            'Event:  eventnogetfeet: fsm.nogetfeet() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventnogetfeet( bottom, eventnogetfeet )

// The instrument is active and ready
var eventchkrdy: Event = document.createEvent('Event')
eventchkrdy.initEvent('chkrdy', false, false)
bottom.addEventListener('chkrdy', ((event: Event) => {
    try {
        fsm.chkrdy()
    }
    catch( error ) {
        console.error(
            'Event:  eventchkrdy: fsm.chkrdy() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventchkrdy( bottom, eventchkrdy )

// The instrument is not active and ready
var eventnochkrdy: Event = document.createEvent('Event')
eventnochkrdy.initEvent('nochkrdy', false, false)
bottom.addEventListener('nochkrdy', ((event: Event) => {
    try {
        fsm.nochkrdy()
    }
    catch( error ) {
        console.error(
            'Event:  eventnochkrdy: fsm.nochkrdy() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventnochkrdy( bottom, eventnochkrdy )

// A user set startline has been selected
var eventusersl: Event = document.createEvent('Event')
eventusersl.initEvent('usersl', false, false)
bottom.addEventListener('usersl', ((event: Event) => {
    try {
        fsm.usersl()
    }
    catch( error ) {
        console.error(
            'Event:  eventusersl: fsm.usersl() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventusersl( bottom, eventusersl )

// No user set startline has been selected
var eventnousersl: Event = document.createEvent('Event')
eventnousersl.initEvent('nousersl', false, false)
bottom.addEventListener('nousersl', ((event: Event) => {
    try {
        fsm.nousersl()
    }
    catch( error ) {
        console.error(
            'Event:  eventnousersl: fsm.nousersl() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventnousersl( bottom, eventnousersl )

// Dropped marked has been acknowledged
var eventmarkack: Event = document.createEvent('Event')
eventmarkack.initEvent('markack', false, false)
bottom.addEventListener('markack', ((event: Event) => {
    try {
        fsm.markack()
    }
    catch( error ) {
        console.error(
            'Event:  eventmarkack: fsm.markack() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventmarkack( bottom, eventmarkack )

// Request to get data has been acknowledged
var eventsldataack: Event = document.createEvent('Event')
eventsldataack.initEvent('sldataack', false, false)
bottom.addEventListener('sldataack', ((event: Event) => {
    try {
        fsm.sldataack()
    }
    catch( error ) {
        console.error(
            'Event:  eventsldataack: fsm.sldataack() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventsldataack( bottom, eventsldataack )

// New startline data has arrived
var eventnewsldata: Event = document.createEvent('Event')
eventnewsldata.initEvent('newsldata', false, false)
bottom.addEventListener('newsldata', ((event: Event) => {
    try {
        fsm.newsldata()
    }
    catch( error ) {
        console.error(
            'Event:  eventnewsldata: fsm.newsldata() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventnewsldata( bottom, eventnewsldata )

// Request to stop getting data has been acknowledged
var eventsldstopack: Event = document.createEvent('Event')
eventsldstopack.initEvent('sldstopack', false, false)
bottom.addEventListener('sldstopack', ((event: Event) => {
    try {
        fsm.sldstopack()
    }
    catch( error ) {
        console.error(
            'Event:  eventsldstopack: fsm.sldstopack() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventsldstopack( bottom, eventsldstopack )

// Luminosity
var eventluminsty: Event = document.createEvent('Event')
eventluminsty.initEvent('luminsty', false, false)
bottom.addEventListener('luminsty', ((event: Event) => {
    try {
        fsm.luminsty()
    }
    catch( error ) {
        console.error(
            'Event:  luminsty: fsm.luminsty() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventluminsty( bottom, eventluminsty )

// Keyboard event requires to swap the display format
kbdInit()
var eventswapdisp: Event = document.createEvent('Event')
eventswapdisp.initEvent('swapdisp', false, false)
bottom.addEventListener('swapdisp', ((event: Event) => {
    console.error(
        'Event:  swapdisp: error: racedashstart does not deal with this event (for now)')
}) as EventListener);
(window as any).iface.regeventswapdisp( bottom, eventswapdisp )

// Although the instrument has no persistent configuration object, close gracefully
var eventclosing: Event = document.createEvent('Event')
eventclosing.initEvent('closing', false, false)
bottom.addEventListener('closing', ((event: Event) => {
    try {
        fsm.closing()
    }
    catch( error ) {
        console.error(
            'Event:  closing: fsm.closing() transition failed, error: ',
            error.message, ' current state: ', fsm.state)
    }
}) as EventListener);
(window as any).iface.regeventclosing( bottom, eventclosing )

/* Since now no other events apart the window load(), we need to await here until
   it has been executed, before continuing to truy event driven operation */

// var pollwaiting: () => void
// (pollwaiting = function() {
//     if ( dbglevel > 0 )
//         console.log('pollinitga() - waiting for waiting-state, now: ',
//          fsm.state)
//     if ( !fsm.is('waiting') ) {
//         setTimeout(pollwaiting, 100)
//     }
// })() // do _everything_ in the routing once condition met


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
})(); // do _everything_ in the routing once condition met



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
