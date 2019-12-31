/* $Id: index.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import '../sass/style.scss'
import { loadConf, saveConf } from '../../src/persistence'
import getLocInfo from '../../src/location'
import { createGauge } from './gauge'
import { setSkPathFontResizingStyle } from './css'
import confObj from './confObj'

var mylib1= require('exports-loader?mylib1!./iface.js')
var global1 = mylib1.multiply(2, 4)
var bottom = null
console.log('global1: ', global1)


// const sPubConf = require('exports-loader?pubConf!./setConf.js');
import sPubConf from './setConf'
console.log('the sPubConf', sPubConf)

var gauge = [] // from 1...n gauges, here only [0]!

// Persistent configuration
var uid = ''
var conf = confObj()
// Functional and environmental
var locInfo = getLocInfo();

// Run-time
var skpath = ''
var titlepath = ''
var unit = ''
var conversion = 100000

function setval(newval) {
    var conval = newval / conversion
    gauge[0].refresh(conval)
}

// Interface to C++, types needs to be set
export function setconf(newuid, newskpath, inval, inmin, inmax ) {
    console.log('setconf(): ', newuid, newskpath)
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
            console.log(
                'setconf() - warning: empty UID, no existing')
        else
            console.log(
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
                console.log(
                    'setconf() - warning: new configuration?',
                    'No conf for: ', uid )
            }
            else {
                conf = nConf
                console.log('setconf() - For ', uid, ' retrieved: ', conf )
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
    console.log('regPath ', selectedPath )
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

var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'hidden' // webkit
    document.body.scroll = "no" // ie
}

window.addEventListener('load',
    function() {
        console.log('loading EngineDJG')

        locInfo = getLocInfo()

        gauge.push (createGauge('gauge0', 0, 1, '[init]') )

        setSkPathFontResizingStyle()
        
        unloadScrollBars()

        /// TESTING /////
        sPubConf( 'from load' )
        setval(50 * 100000)

        // Create the event, attach it to the 'bottom' id
        bottom = document.getElementById ('bottom' )
        var event = document.createEvent('Event')
        // Define the event name
        event.initEvent('skdatain', true, true);
        // Listen for the event.
        bottom.addEventListener('skdatain', function (e) {
            console.log('bottom: customEvent "skdatain"')
        }, false);
        // Register the event for external triggering
        mylib1.regevent( bottom, event )
        
        setMenu( emptypath )
    }, false)


/* Menu */
/* Original Copyright (c) 2019 by Ryan Morr (https://codepen.io/ryanmorr/pen/JdOvYR)
   Modified for OpenCPN gauge / display usage */

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

