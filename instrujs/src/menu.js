/* $Id: menu.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor for a state machine to build user menu strucures
import sanitizer from './escapeHTML'
var Sanitizer = sanitizer()

var menu = document.querySelector('.menu')
var waitmsg = {
    allpaths: [window.instrulang.menuPathWaitMsg]
}
var runmsg = {
    allpaths: [window.instrulang.menuPathRunningReconfig]
}
var isOnLoad = false
var isRunTime = false

export function setMenuAllPaths( that, onload, runtime ) {
    isOnLoad = onload || false
    isRunTime = runtime || false
    var menuul = '<ul id="mi1-u-0" class="menu">'
    var submenustart = 0
    var topics = ['','','','','','','','','']
    var htmlObj
    for ( var i = 0; i < that.allpaths.length; i++ ) {
        var pathe = that.allpaths[ parseInt(i) ]
        var pathel = pathe.split('.')
        var j
        for ( j = 0; j < ( pathel.length - 1); j++ )
            if ( pathel[parseInt(j)] !== topics[parseInt(j)] ) {
                topics[parseInt(j)] = pathel[parseInt(j)]
                if ( j === 0 )
                    for ( var z = 1; z < topics.length; z++ )
                        topics[ parseInt(z) ] = ''
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
                menuul += pathel[parseInt(j)]
                menuul += '</span></button>'
                menuul += '<ul id="mi1-u-' + i + '-' + j
                menuul += '" class="menu">'           
            }
        menuul += '<li id="mi1-l-' + i + '-' + j + '" class="menu-item">'
        menuul += '<button id="mif-b-'
        menuul += that.allpaths[parseInt(i)]
        if ( !isRunTime && !isOnLoad ) {
            if ( instrustat.hasPathEntry( pathe ) )
                menuul += '" type="button" class="menu-btn hasdef">'
            else
                menuul += '" type="button" class="menu-btn">'
        }
        else
            menuul += '" type="button" class="menu-btn">'
        menuul += '<span id="mif-s-'
        menuul += that.allpaths[parseInt(i)]
        menuul += '" class="menu-text">'
        menuul += pathel[parseInt(j)]
        menuul += '</span></button></li>'
    }
    menuul += '</li></ul>'
    htmlObj = Sanitizer.createSafeHTML(menuul)
    document.getElementById('pathMenu').innerHTML = Sanitizer.unwrapSafeHTML(htmlObj)
    document.getElementById('pathMenu').overflow = 'hidden'
    menu = document.querySelector('.menu')
    that.menu = menu

    if ( isRunTime )
        document.getElementById('skPath').innerHTML = '&nbsp'
    else if ( isOnLoad ) {
        htmlObj = Sanitizer.createSafeHTML(window.instrulang.loading)
        document.getElementById('skPath').innerHTML =  Sanitizer.unwrapSafeHTML(htmlObj)
    }
    else {
        htmlObj = Sanitizer.createSafeHTML(window.instrulang.rightClickHereToSubscribe)
        document.getElementById('skPath').innerHTML =  Sanitizer.unwrapSafeHTML(htmlObj)
    }
}

var setemptypath = (function() {
    setMenuAllPaths( waitmsg, true, false )
    return
}())

export function setMenuRunTime( that ) {
    setMenuAllPaths( runmsg, false, true )
    return
}

export function setMenuBackToLoading( that ) {
    setMenuAllPaths( waitmsg, true, false )
    return
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

function onMouseDown(e){
    document.removeEventListener('mousedown', onMouseDown)
    e = e.srcElement
    var idsplit
    if ( (e.nodeName) === 'BUTTON' || (e.nodeName === 'SPAN') ) {
        if ( e.id !== '' ) {
            idsplit = e.id.split( '-' )
            if ( idsplit[0] === 'mif' ) {
                if ( !isOnLoad ) {
                    if ( isRunTime )
                        window.iface.setchgconf( 'chgconf' )
                    else
                        window.iface.setselected( idsplit[2] )
                }
                hideMenu()
            }
            else {
                if ( idsplit[0] === 'mi1' )
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
            idsplit = e.id.split( '-' )
            if ( idsplit[0] === 'mi1' )
                document.addEventListener('mousedown', onMouseDown )
            else
                hideMenu()
        }
        else
            hideMenu()
    }
}

function onContextMenu(e){
    e.preventDefault()
    showMenu()
    document.addEventListener('mousedown', onMouseDown )
}

document.addEventListener('contextmenu', onContextMenu, false)
