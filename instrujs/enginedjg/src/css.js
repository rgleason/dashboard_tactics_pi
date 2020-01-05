/* $Id: css.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import '../sass/style.scss'

var skpathispropl = false

export function setSkPathFontResizingStyle() {
    var hasproplsupport = false
    var hadexception = false
    var ua = window.navigator.userAgent
    var msie = ua.indexOf("MSIE ")
    try { 
        if ( CSS.supports ) {
            if ( CSS.supports("font-size",
                              getComputedStyle(
                                  document.documentElement).getPropertyValue(
                                      '--skpproplw')) ) {
                console.log('Has viewport proportional font size support')
                hasproplsupport = true;
            }
            else{
                console.log('No viewport proportional font size support, use media properties')
            }
        }
    }
    catch( error ) {
        hadexception = true
        console.log('No CSS.supports() - got exception, use media properties - unless this is a MSIE: use propl, anyway!')
    }
    if ( hasproplsupport || ( hadexception && (msie > 0) ) ) {
        skpathispropl = true
        document.getElementById("skPath").className = "skpath propl day"
    }
    else
        document.getElementById("skPath").className = "skpath fixed day"
    return
}

export function getNewLuminosity( that ) {
    var newluminosity = window.iface.luminsty
    if ( (newluminosity === 'day') ||
         (newluminosity === 'dusk') ||
         (newluminosity === 'night') ) {
        that.luminosity  = newluminosity

        var newclass = 'instrument ' + newluminosity
        var elem = document.getElementById('instrument')
        var oldclass = elem.className
        if ( !(newclass === oldclass) )
            elem.className = newclass

        if ( skpathispropl )
            newclass = 'skpath propl ' + newluminosity
        else
            newclass = 'skpath fixed ' + newluminosity
        elem = document.getElementById('skPath')
        oldclass = elem.className
        if ( !(newclass === oldclass) )
            elem.className = newclass
    }
    return
}
