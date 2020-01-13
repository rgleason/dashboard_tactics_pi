/* $Id: index.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import { packagename, version } from '../../src/version'
console.log('enginedjg ', packagename(), ' ', version())
var dbglevel = window.instrustat.debuglevel

import '../sass/style.scss'
// import { loadConf, saveConf } from '../../src/persistence'
import { createStateMachine } from './statemachine'
import { setSkPathFontResizingStyle } from './css'
import visualize from '../../src/state-machine-visualize'

// we could access it with window.iface but this is needed once to get it in...
var iface = require('exports-loader?iface!../../src/iface.js')

// State Machine Service
if ( dbglevel > 0 ) console.log('index.js - creating the finite state machine')
var fsm = createStateMachine();
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

// Create the transitional events (the IE way, sorry!) for clieant messages
var bottom = document.getElementById ('bottom' )

// UID and configuration file
var eventsetid = document.createEvent('Event')
eventsetid.initEvent('setid', false, false);
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
    var pollhascfg = function () {
        if ( dbglevel > 1 ) console.log('pollhascfg() - waiting on "hasid" state')
        if ( fsm.is('hasid') ) {
            var hascfg = true
            if ( fsm.conf == null )
                hascfg = false
            else if ( (fsm.conf.path == null) || (fsm.conf.path === '') )
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
            setTimeout(pllhascfg, 100);
        }
    }(); // do selection of the next action in the routing once ID has been set, or not
}, true);
window.iface.regeventsetid( bottom, eventsetid )

// All available paths have been set
var eventsetall = document.createEvent('Event')
eventsetall.initEvent('setall', false, false);
bottom.addEventListener('setall', function (e) {
    try {
        fsm.setall()
    }
    catch( error ) {
        console.error(
            'Event:  setall: fsm.setall() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true);
window.iface.regeventsetall( bottom, eventsetall )

// Selection of a path has been made
var eventselected = document.createEvent('Event')
eventselected.initEvent('selected', false, false);
bottom.addEventListener('selected', function (e) {
    try {
        fsm.selected()
    }
    catch( error ) {
        console.error(
            'Event:  selected: fsm.selected() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true);
window.iface.regeventselected( bottom, eventselected )

// Selection of a path has been acknowledged
var eventacksubs = document.createEvent('Event')
eventacksubs.initEvent('acksubs', false, false);
bottom.addEventListener('acksubs', function (e) {
    try {
        fsm.acksubs()
    }
    catch( error ) {
        console.error(
            'Event:  acksubs: fsm.acksubs() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true);
window.iface.regeventacksubs( bottom, eventacksubs )

// New data is coming in
var eventnewdata = document.createEvent('Event')
eventnewdata.initEvent('newdata', false, false);
bottom.addEventListener('newdata', function (e) {
    try {
        fsm.newdata()
    }
    catch( error ) {
        console.error(
            'Event:  newdata: fsm.newdata() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true);
window.iface.regeventnewdata( bottom, eventnewdata 
                            )
// Change of configuration has been requested
var eventchgconf = document.createEvent('Event')
eventchgconf.initEvent('chgconf', false, false);
bottom.addEventListener('chgconf', function (e) {
    try {
        fsm.chgconf()
    }
    catch( error ) {
        console.error(
            'Event:  chgconf: fsm.chgconf() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true);
window.iface.regeventchgconf( bottom, eventchgconf )

// Luminosity
var eventluminsty = document.createEvent('Event')
eventluminsty.initEvent('luminsty', false, false);
bottom.addEventListener('luminsty', function (e) {
    try {
        fsm.luminsty()
    }
    catch( error ) {
        console.error(
            'Event:  luminsty: fsm.luminsty() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true);
window.iface.regeventluminsty( bottom, eventluminsty )

// The instrument has a persistent configuraiton object
var eventclosing = document.createEvent('Event')
eventclosing.initEvent('closing', false, false);
bottom.addEventListener('closing', function (e) {
    try {
        fsm.closing()
    }
    catch( error ) {
        console.error(
            'Event:  closing: fsm.closing() transition failed, error: ', error,
            ' current state: ', fsm.state)
    }
}, true);
window.iface.regeventclosing( bottom, eventclosing )

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
        setTimeout(pollinitga, 100);
    }
}

pollinitga(); // do _everything_ in the routing once condition met


 /* *********

function setval(newval) {
    var conval = newval / conversion
    gauge[0].refresh(conval)
}

// Interface to C++, types needs to be set
export function setconf(newuid, newskpath, inval, inmin, inmax ) {
    if ( dbglevel > 0 ) console.log('setconf(): ', newuid, newskpath)
    var nUid = newuid || null
    if (nUid != null)
        uid = nUid
    else {
        console.error(
            'setconf() - error: null UID')
        return
    }
    if ( nUid == '') {
        if ( uid == '' )
            if ( dbglevel > 0 ) console.log(
                'setconf() - warning: empty UID, no existing')
        else
            if ( dbglevel > 0 ) console.log(
                'setconf() - warning: empty UID, using existing: ', uid)
    }
    var nSkPath = newskpath || null
    if ( (nSkPath == null) && ( uid == '') ) {
        console.error(
            'setconf() - error: no uid (no newuid), no newskpath!')
        return
    }
    if ( ( skpath == '' ) && ( uid == '') ) {
        console.error(
            'setconf() - error: no new UID,',
            'no SK path memorized, no UID.')
        return
    }
    if ( ( nSkPath != null) && (nSkPath != '') ) {
        skpath = newskpath
    }
    else {
        if ( uid != '') {
            var nConf = loadConf( uid, conf, locInfo.path )
            if ( nConf == null ) {
                if ( dbglevel > 0 ) console.log(
                    'setconf() - warning: new configuration?',
                    'No conf for: ', uid )
            }
            else {
                conf = nConf
                if ( dbglevel > 0 ) console.log('setconf() - For ', uid, ' retrieved: ', conf )
            }
            skpath = conf.skpath
        }
    }
    var arrxsk = skpath.split(".")
    titlepath = "<p>"
    for ( var i = 0; i < (arrxsk.length-1); i++ ) {
        titlepath += arrxsk[i] + "."
    }
    titlepath += "<b>" + arrxsk[arrxsk.length-1] + "</b>"
    document.getElementById('skPath').innerHTML = titlepath
    unit = "[bar]"
    var nMin = inmin || null
    if (nMin == null)
        nMin = 0
    var nMax = inmax || null
    if (nMax == null)
        nMax = 70
    var newval = inval || null
    if (newval == null)
        newval = 700000
    var conval = newval / conversion
    gauge[0].refresh(conval, nMax, nMin, unit )
    if ( uid != '' ) {
        conf.skpath = skpath
        if ( !saveConf( uid, conf ) ) {
            console.error(
                'setconf() - failed to save for ', uid,
                ' configuration: ', conf )
        }
    }
    return
}

function regPath(selectedPath) {
    if ( dbglevel > 0 ) console.log('regPath ', selectedPath )
    setconf( uid, selectedPath, 60 * 100000 )
}

var menu = document.querySelector('.menu')

var alldatapaths = [
    'environment.wind.angleApparent',
    'environment.wind.speedApparent',
    'navigation.courseOverGroundTrue',
    'navigation.speedOverGround',
    'navigation.position.latitude',
    'navigation.position.longitude',
    'propulsion.port.oilPressure',
    'propulsion.port.revolutions',
    'propulsion.port.temperature',
    'propulsion.starboard.oilPressure',
    'propulsion.starboard.revolutions',
    'propulsion.starboard.temperature',
    'battery.empty',
]

var emptypath = [
    'no data available'
]

function setMenu( sortedpath ) {
    var menuul = '<ul id="mi1-u-0" class="menu">'
    var topics = ['','','','','','','','','']
    var submenustart = 0
    for ( var i = 0; i < sortedpath.length; i++ ) {
        var pathel = sortedpath[ i ].split('.')
        for ( var j = 0; j < ( pathel.length - 1); j++ ) {
            if ( pathel[j] != topics[j] ) {
                topics[j] = pathel[j]
                while ( submenustart > j ){
                    menuul += '</ul>'
                    menuul += '</li>'
                    submenustart--         
                }
                submenustart++
                menuul += '<li id="mi1-l-' + i + '-' + j
                menuul += '" class="menu-item menu-item-submenu">'
                menuul += '<button id="mi1-b-' + i + '-' + j
                menuul += '" type="button" class="menu-btn">'
                menuul += '<span id="mi1-s-' + i + '-' + j
                menuul += '" class="menu-text">'
                menuul += pathel[j]
                menuul += '</span></button>'
                menuul += '<ul id="mi1-u-' + i + '-' + j
                menuul += '" class="menu">'           
            }
        }
        menuul += '<li id="mi1-l-' + i + '-' + j + '" class="menu-item">'
        menuul += '<button id="mif-b-'
        menuul += sortedpath[i]
        menuul += '" type="button" class="menu-btn">'
        menuul += '<span id="mif-s-'
        menuul += sortedpath[i]
        menuul += '" class="menu-text">'
        menuul += pathel[j]
        menuul += '</span></button></li>'
    }
    menuul += '</li></ul>'
    document.getElementById('pathMenu').innerHTML = menuul
    document.getElementById('pathMenu').overflow = 'hidden'
    menu = document.querySelector('.menu')
}

****** */

var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'hidden' // webkit
    document.body.scroll = "no" // ie
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


/* Menu */
/* Original Copyright (c) 2019 by Ryan Morr (https://codepen.io/ryanmorr/pen/JdOvYR)
   Modified for OpenCPN gauge / display usage */

/* ****

function showMenu(){
    menu.classList.add('menu-show')
}

function hideMenu(){
    menu.classList.remove('menu-show')
}

function onContextMenu(e){
    e.preventDefault()
    showMenu()
    document.addEventListener('mousedown', onMouseDown )
}

function onMouseDown(e){
    document.removeEventListener('mousedown', onMouseDown)
    e= e.srcElement
    if ( (e.nodeName) === 'BUTTON' || (e.nodeName === 'SPAN') ) {
        if ( e.id !== '' ) {
            var ids = e.id.split( '-' )
            if ( ids[0] == 'mif' ) {
                regPath(ids[2])
                hideMenu()
            }
            else {
                if ( ids[0] == 'mi1' )
                        document.addEventListener('mousedown', onMouseDown )
                else
                    hideMenu()
            }
        }
        else {
            hideMenu()
        }
    }
    else {
        if ( e.id !== '' ) {
            var ids = e.id.split( '-' )
            if ( ids[0] == 'mi1' )
                document.addEventListener('mousedown', onMouseDown )
            else
                hideMenu()
        }
        else
            hideMenu()
    }
}

document.addEventListener('contextmenu', onContextMenu, false)
 **** */
