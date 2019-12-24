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
var skpath = "";
var titlepath = "";
var unit = "";
var conversion = 100000;

function setval(newval) {
    var conval = newval / conversion;
    g.refresh(conval);
}

function setconf(newskpath, val, min, max ) {
    skpath = newskpath;
    let arr = newskpath.split(".");
    titlepath = "<p>";
    for ( i = 0; i < (arr.length-1); i++ )
        titlepath += arr[i] + ".";
    titlepath += "<b>" + arr[arr.length-1] + "</b>";
    document.getElementById('skPath').innerHTML = titlepath;
    unit = "[bar]"
    let nMin = min || null;
    if (nMin == null)
        nMin = 0;
    let nMax = max || null;
    if (nMax == null)
        nMax = 6
    g.refresh(val, nMax, nMin, unit );
}

function regPath(selectedPath) {
    console.log('regPath ', selectedPath );
    setconf( selectedPath, 0 );
}

var setMenu = (function setMenu() {
    let sortedpath = [
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
    let menuul = '<ul id="mi1-u-0" class="menu">';
    let topics = ['','','','','','','','',''];
    let submenustart = 0;
    for ( i = 0; i < sortedpath.length; i++ ) {
        let pathel = sortedpath[ i ].split('.');
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
    document.getElementById('pathMenu').overflow = 'visible';
})();

window.addEventListener('load', 
                        function() {
//                            unloadScrollBars();                     
                            setval(0);
  //                          setMenu();
                        }, false);

var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'hidden'; // webkit
    document.body.scroll = "no"; // ie
}

// Original Copyright (c) 2019 by Ryan Morr (https://codepen.io/ryanmorr/pen/JdOvYR)
// Modified for OpenCPN gauge / display usage
var menu = document.querySelector('.menu');

function showMenu(x, y){
   /*
     Override the default position in CSS to be the one of the cursors.
     However, the canvas is usually very small a situation may occure
     where not all menu items are entirely visible. Adjust rather CSS.
   */
    //    menu.style.left = x + 'px';
    //    menu.style.top = y + 'px';
    menu.classList.add('menu-show');
}

function hideMenu(){
    menu.classList.remove('menu-show');
}

function onContextMenu(e){
    e.preventDefault();
    showMenu(e.pageX, e.pageY);
    document.addEventListener('mousedown', onMouseDown, false);
}

function onMouseDown(e){
    document.removeEventListener('mousedown', onMouseDown);
    e= e.srcElement;
    if ( (e.nodeName) === 'BUTTON' || (e.nodeName === 'SPAN') ) {
        if ( e.id !== '' ) {
            let ids = e.id.split( '-' );
            if ( ids[0] == 'mif' ) {
                regPath(ids[2]);
                hideMenu();
            }
            else {
                if ( ids[0] == 'mi1' )
                    document.addEventListener('mousedown', onMouseDown, false);
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
            let ids = e.id.split( '-' );
            if ( ids[0] == 'mi1' )
                document.addEventListener('mousedown', onMouseDown, false);
            else
                hideMenu();
        }
        else
            hideMenu();
    }
}

document.addEventListener('contextmenu', onContextMenu, false);

