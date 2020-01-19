/* $Id: menu.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor for a state machine to build user menu strucures

var menu = document.querySelector('.menu')
var waitmsg = {
    allpaths: [window.instrulang.menuPathWaitMsg]
}
var runmsg = {
    allpaths: [window.instrulang.menuPathRunningReconfig]
}
var isOnLoad = false
var isRunTime = false

var setemptypath = function() {
    setMenuAllPaths( waitmsg, true, false )
}()

export function setMenuRunTime( that ) {
    setMenuAllPaths( runmsg, false, true )
}

export function setMenuBackToLoading( that ) {
    setMenuAllPaths( waitmsg, true, false )
}

export function setMenuAllPaths( that, onload, runtime ) {
    isOnLoad = onload || false
    isRunTime = runtime || false
    var menuul = '<ul id="mi1-u-0" class="menu">'
    var submenustart = 0
    var topics = ['','','','','','','','','']
    for ( var i = 0; i < that.allpaths.length; i++ ) {
        var pathel = that.allpaths[ i ].split('.')
        for ( var j = 0; j < ( pathel.length - 1); j++ ) {
            if ( pathel[j] != topics[j] ) {
                topics[j] = pathel[j]
                if ( j == 0 )
                    for ( var z = 1; z < topics.length; z++ )
                        topics[ z ] = ''
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
        menuul += that.allpaths[i]
        menuul += '" type="button" class="menu-btn">'
        menuul += '<span id="mif-s-'
        menuul += that.allpaths[i]
        menuul += '" class="menu-text">'
        menuul += pathel[j]
        menuul += '</span></button></li>'
    }
    menuul += '</li></ul>'
    document.getElementById('pathMenu').innerHTML = menuul
    document.getElementById('pathMenu').overflow = 'hidden'
    menu = document.querySelector('.menu')
    that.menu = menu
    if ( !(isOnLoad || isRunTime) )
        document.getElementById('skPath').innerHTML =
        window.instrulang.rightClickHereToSubscribe
    if ( isOnLoad )
        document.getElementById('skPath').innerHTML =
        window.instrulang.loading
    if ( isRunTime )
        document.getElementById('skPath').innerHTML =
        '&nbsp'
}

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
                if ( !isOnLoad ) {
                    if ( isRunTime )
                        window.iface.setchgconf( 'chgconf' )
                    else
                        window.iface.setselected( ids[2] )
                }
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
