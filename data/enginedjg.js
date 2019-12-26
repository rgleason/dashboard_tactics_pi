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
var uid = '';
var msie = 0;
var skpath = '';
var titlepath = '';
var unit = '';
var conversion = 100000;


function setval(newval) {
    var conval = newval / conversion;
    g.refresh(conval);
}

// Persistence
function saveParam( cname, cvalue, inexdays ) {
    var d = new Date();
    var expires = '';
    var exdays = inexdays || null;
    if ( exdays != null ) {
        d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000));
        var expires = ";expires="+d.toUTCString();
    }
    document.cookie = cname + "=" + cvalue + expires + ";path=/" + uid;
}
function getParam( cname ) {
    var name = cname + "=";
    var ca = document.cookie.split(';');
    for(var i = 0; i < ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
}

// Interface to C++, types needs to be set
function setconf(newuid, newskpath, inval, inmin, inmax ) {
    if ( newuid == null ) {
        console.error('enginedjg.js setconf() - error: null UID');
        return;
    }
    if ( uid == '' )
        console.log('enginedjg.js setconf() - warning: empty UID, no config saving');
    else
        console.log('enginedjg.js setconf() - warning: empty UID, using existing: ', uid);
    if ( (newskpath == null) && ( newuid === '' ) && ( uid == '') ) {
        console.error('enginedjg.js setconf() - error: no uid, no newuid, no newskpath!');
        return;
    }
    if ( ( skpath == '' ) && ( newuid === '') && ( uid == '') ) {
        console.error('enginedjg.js setconf() - error: no new UID, no SK path given, no SK path memorized, no UID.');
        return;
    }
    var nUid = newuid || null;
    if (nUid != null)
        uid = nUid;
    if ( newuid != '' ) {
        skpath = newskpath;
    }
    else {
        if ( uid != '')
        skpath = getParam( 'skpath' );
    }
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
    saveParam( 'skpath', skpath );
}

function regPath(selectedPath) {
    console.log('regPath ', selectedPath );
    setconf( uid, selectedPath, 60 * 100000 );
}

var menu = document.querySelector('.menu');

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
];

var emptypath = [
    'no data available'
];

function setMenu( sortedpath ) {
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
        setMenu( emptypath );
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
    e.preventDefault();
    showMenu();
    document.addEventListener('mousedown', onMouseDown );
}

function onMouseDown(e){
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

