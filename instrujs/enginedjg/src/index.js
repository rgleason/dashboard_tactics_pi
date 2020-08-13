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

// UID and configuration file
var eventsetid = document.createEvent('Event')
eventsetid.initEvent('setid', false, false)
bottom.addEventListener('setid', function (e) {
    if ( dbglevel > 1 ) console.log('pollhascfg() - attempt to setid() transition.')
    try {
        fsm.setid()
    }
    catch( error ) {
        console.error(
            'Event:  setid: fsm.setid() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
    var pollhascfg
    (pollhascfg = function () {
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
            setTimeout(pollhascfg, 100)
        }
    })() // do selection of the next action in the routing once ID has been set, or not
}, true)
window.iface.regeventsetid( bottom, eventsetid )

// All available paths have been set
var eventsetall = document.createEvent('Event')
eventsetall.initEvent('setall', false, false)
bottom.addEventListener('setall', function (e) {
    try {
        fsm.setall()
    }
    catch( error ) {
        console.error(
            'Event:  setall: fsm.setall() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true)
window.iface.regeventsetall( bottom, eventsetall )

// All available DB schema paths have been set
var eventsetalldb = document.createEvent('Event')
eventsetalldb.initEvent('setalldb', false, false)
bottom.addEventListener('setalldb', function (e) {
    console.error(
        'Event:  setalldb: error: enginedjg does not require DB paths')
}, true)
window.iface.regeventsetalldb( bottom, eventsetalldb )

// DB path did not contain wanted path, rescan
var eventrescan = document.createEvent('Event')
eventrescan.initEvent('rescan', false, false)
bottom.addEventListener('setalldb', function (e) {
        console.error(
            'Event:  eventrescan: error: enginedjg does not require DB paths')
}, true)
window.iface.regeventrescan( bottom, eventrescan )

// Selection of a path has been made
var eventselected = document.createEvent('Event')
eventselected.initEvent('selected', false, false)
bottom.addEventListener('selected', function (e) {
    try {
        fsm.selected()
    }
    catch( error ) {
        console.error(
            'Event:  selected: fsm.selected() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true)
window.iface.regeventselected( bottom, eventselected )

// Selection of a path has been acknowledged
var eventacksubs = document.createEvent('Event')
eventacksubs.initEvent('acksubs', false, false)
bottom.addEventListener('acksubs', function (e) {
    try {
        fsm.acksubs()
    }
    catch( error ) {
        console.error(
            'Event:  acksubs: fsm.acksubs() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true)
window.iface.regeventacksubs( bottom, eventacksubs )

// Requested database schema is now available
var eventackschema = document.createEvent('Event')
eventackschema.initEvent('ackschema', false, false)
bottom.addEventListener('ackschema', function (e) {
    console.error(
        'Event:  ackschema: error: enginedjg does not ask for DB schema')
}, true)
window.iface.regeventackschema( bottom, eventackschema )

// Request to fetch new data from DB
var eventgetnew = document.createEvent('Event')
eventgetnew.initEvent('getnew', false, false)
bottom.addEventListener('getnew', function (e) {
    console.error(
        'Event:  getnew: error: enginedjg does not ask new data from DB')
}, true)
window.iface.regeventgetnew( bottom, eventgetnew )

// New data is coming in
var eventnewdata = document.createEvent('Event')
eventnewdata.initEvent('newdata', false, false)
bottom.addEventListener('newdata', function (e) {
    try {
        if ( (fsm.state === 'waitdata') || (fsm.state === 'showdata') )
            fsm.newdata()
    }
    catch( error ) {
        console.error(
            'Event:  newdata: fsm.newdata() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true)
window.iface.regeventnewdata( bottom, eventnewdata )

// Asynchronous data retrieval from DB has failed
var eventerrdata = document.createEvent('Event')
eventerrdata.initEvent('errdata', false, false)
bottom.addEventListener('errdata', function (e) {
    console.error(
        'Event:  errdata: error: enginedjg has not asked any data from DB')
}, true)
window.iface.regeventerrdata( bottom, eventerrdata )

// Retry Asynchronous data retrieval from DB
var eventretryget = document.createEvent('Event')
eventretryget.initEvent('retryget', false, false)
bottom.addEventListener('retryget', function (e) {
    console.error(
        'Event:  errdata: error: enginedjg is not getting data from DB, no retry')
}, true)
window.iface.regeventretryget( bottom, eventretryget )

// Change of configuration has been requested
var eventchgconf = document.createEvent('Event')
eventchgconf.initEvent('chgconf', false, false)
bottom.addEventListener('chgconf', function (e) {
    try {
        fsm.chgconf()
    }
    catch( error ) {
        console.error(
            'Event:  chgconf: fsm.chgconf() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true)
window.iface.regeventchgconf( bottom, eventchgconf )

// Luminosity
var eventluminsty = document.createEvent('Event')
eventluminsty.initEvent('luminsty', false, false)
bottom.addEventListener('luminsty', function (e) {
    try {
        fsm.luminsty()
    }
    catch( error ) {
        console.error(
            'Event:  luminsty: fsm.luminsty() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true)
window.iface.regeventluminsty( bottom, eventluminsty )

// Keyboard event requires to swap the display format
kbdInit()
var eventswapdisp = document.createEvent('Event')
eventswapdisp.initEvent('swapdisp', false, false)
bottom.addEventListener('swapdisp', function (e) {
    try {
        if ( fsm.state === 'showdata')
            fsm.swapdisp()
    }
    catch( error ) {
        console.error(
            'Event:  swapdisp: fsm.swapdisp() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true)
window.iface.regeventswapdisp( bottom, eventswapdisp )

// The instrument has a persistent configuration object, close gracefully
var eventclosing = document.createEvent('Event')
eventclosing.initEvent('closing', false, false)
bottom.addEventListener('closing', function (e) {
    try {
        fsm.closing()
    }
    catch( error ) {
        console.error(
            'Event:  closing: fsm.closing() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true)
window.iface.regeventclosing( bottom, eventclosing )

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
