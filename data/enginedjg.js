/* $Id: enginedjg.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
var g = new JustGage({
    id: "gauge",
    value: 0,
    decimals: 1,
    label: "[init]",
    min: 0,
    max: 100,
    pointer: true,
    pointerOptions: {
        toplength: -15,
        bottomlength: 10,
        bottomwidth: 12,
        color: '#8e8e93',
        stroke: '#ffffff',
        stroke_width: 3,
        stroke_linecap: 'round'
    },
    labelFontColor: "black",
    valueFontColor: "blue",
    valueFontFamily: "Courier",
    relativeGaugeSize: true
});
var msie = 0;
var skpath = '';
var titlepath = '';
var unit = '';
var conversion = 100000;


function setval(newval) {
    var conval = newval / conversion;
    g.refresh(conval);
}

// Interface to C++, types needs to be set
function setconf(newskpath, inval, inmin, inmax ) {
    if ( (newskpath == null) || ( newskpath === '' ) ) {
        console.log('enginedjg.js setconf() - warning: null or invalid SK path');
        return;
    }
    skpath = newskpath + '';
    var arrxsk = skpath.split(".");
    titlepath = "<p>";
    for ( i = 0; i < (arrxsk.length-1); i++ ) {
        titlepath += arrxsk[i] + ".";
    }
    titlepath += "<b>" + arrxsk[arrxsk.length-1] + "</b>";
    document.getElementById('skPath').innerHTML = titlepath;
    unit = "[bar]";
    var nMin = inmin || null;
    if (nMin == null)
        nMin = 0;
    var nMax = inmax || null;
    if (nMax == null)
        nMax = 70;
    var newval = inval || null;
    if (newval == null)
        newval = 700000;
    var conval = newval / conversion;
    g.refresh(conval, nMax, nMin, unit );
}

function regPath(selectedPath) {
    console.log('regPath ', selectedPath );
    setconf( selectedPath, 60 * 100000 );
}

var menu = document.querySelector('.menu');

function setMenu() {
    var sortedpath = [
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
    ];
    var menuul = '<ul id="mi1-u-0" class="menu">';
    var topics = ['','','','','','','','',''];
    var submenustart = 0;
    for ( i = 0; i < sortedpath.length; i++ ) {
        var pathel = sortedpath[ i ].split('.');
        for ( j = 0; j < ( pathel.length - 1); j++ ) {
            if ( pathel[j] != topics[j] ) {
                topics[j] = pathel[j];
                while ( submenustart > j ){
                    menuul += '</ul>';
                    menuul += '</li>';
                    submenustart--;         
                }
                submenustart++;
                menuul += '<li id="mi1-l-' + i + '-' + j;
                menuul += '" class="menu-item menu-item-submenu">';
                menuul += '<button id="mi1-b-' + i + '-' + j;
                menuul += '" type="button" class="menu-btn">';
                menuul += '<span id="mi1-s-' + i + '-' + j;
                menuul += '" class="menu-text">';
                menuul += pathel[j];
                menuul += '</span></button>';
                menuul += '<ul id="mi1-u-' + i + '-' + j;
                menuul += '" class="menu">';           
            }
        }
        menuul += '<li id="mi1-l-' + i + '-' + j + '" class="menu-item">';
        menuul += '<button id="mif-b-';
        menuul += sortedpath[i];
        menuul += '" type="button" class="menu-btn">';
        menuul += '<span id="mif-s-';
        menuul += sortedpath[i];
        menuul += '" class="menu-text">';
        menuul += pathel[j];
        menuul += '</span></button></li>';
    }
    menuul += '</li></ul>';
    document.getElementById('pathMenu').innerHTML = menuul;
    document.getElementById('pathMenu').overflow = 'hidden';
    menu = document.querySelector('.menu');
}

var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'hidden'; // webkit
    document.body.scroll = "no"; // ie
}

window.addEventListener('load',
    function() {
        console.log('loading enginedjg.js');  
        try { 
            if ( CSS.supports ) {
                if ( CSS.supports("font-size","5vw") ) {
                    console.log('Viewport proportinal support');
                    document.getElementById("skPath").className += " propl";
                }
                else{
                    console.log('No viewport proportinal support, fixed size');
                    document.getElementById("skPath").className += " fixed";
                }
            }
        }
        catch( error ) {
            console.log('No CSS.supports()');
            var ua = window.navigator.userAgent;
            msie = ua.indexOf("MSIE ");
            if ( msie > 0 ) {
                console.log('MSIE - WebView forcing to IE8+ presumed, viewport proportional');
                document.getElementById("skPath").className += " propl";
            }
            else {
                console.log('cannot determine viewport proportinal support, fixed size');
                document.getElementById("skPath").className += " fixed";
            }
        }
        unloadScrollBars();
        setval(50 * 100000);
        setMenu();
    }, false);

/* Menu */
/* Original Copyright (c) 2019 by Ryan Morr (https://codepen.io/ryanmorr/pen/JdOvYR)
   Modified for OpenCPN gauge / display usage */

function showMenu(){
    menu.classList.add('menu-show');
}

function hideMenu(){
    menu.classList.remove('menu-show');
}

function onContextMenu(e){
    console.log('onContextMenu()');
    e.preventDefault();
    showMenu();
    document.addEventListener('mousedown', onMouseDown );
}

function onMouseDown(e){
    console.log('onMouseDown()');
    document.removeEventListener('mousedown', onMouseDown);
    e= e.srcElement;
    if ( (e.nodeName) === 'BUTTON' || (e.nodeName === 'SPAN') ) {
        if ( e.id !== '' ) {
            var ids = e.id.split( '-' );
            if ( ids[0] == 'mif' ) {
                regPath(ids[2]);
                hideMenu();
            }
            else {
                if ( ids[0] == 'mi1' )
                        document.addEventListener('mousedown', onMouseDown );
                else
                    hideMenu();
            }
        }
        else {
            hideMenu();
        }
    }
    else {
        if ( e.id !== '' ) {
            var ids = e.id.split( '-' );
            if ( ids[0] == 'mi1' )
                document.addEventListener('mousedown', onMouseDown );
            else
                hideMenu();
        }
        else
            hideMenu();
    }
}

document.addEventListener('contextmenu', onContextMenu, false);

