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
var setval = function(newval) {
    var conval = newval / conversion;
    g.refresh(conval);
};
var setconf = function(newskpath, val, min, max ) {
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
};
var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'visible'; // webkit
    document.body.scroll = "no"; // ie
}
window.addEventListener('load', 
                        function() {
                            unloadScrollBars();                     
                            setval(0);
                            setMenu();
                        }, false);

var setMenu = (function() {
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
    let menuul = '<ul class="menu">';
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
                menuul += '<li class="menu-item menu-item-submenu">';
                menuul += '<button type="button" class="menu-btn">';
                menuul += '<span class="menu-text">';
                menuul += pathel[j];
                menuul += '</span></button>';
                menuul += '<ul class="menu">';           
            }
        }
        menuul += '<li class="menu-item">';
        menuul += '<button type="button" class="menu-btn">';
        menuul += '<span class="menu-text">';
        menuul += pathel[j];
        menuul += '</span></button></li>';
    }
    menuul += '</li>';
    document.getElementById('pathMenu').innerHTML = menuul;
    document.getElementById('pathMenu').overflow = 'visible';
})();

var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'hidden'; // webkit
    document.body.scroll = "no"; // ie
}
window.addEventListener('load', 
                        function() {
                            // unloadScrollBars(); 
                        }, false);

//Copyright (c) 2019 by Ryan Morr (https://codepen.io/ryanmorr/pen/JdOvYR)
var menu = document.querySelector('.menu');

function showMenu(x, y){
    menu.style.left = x + 'px';
    menu.style.top = y + 'px';
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
    hideMenu();
    document.removeEventListener('mousedown', onMouseDown);
}

document.addEventListener('contextmenu', onContextMenu, false);

