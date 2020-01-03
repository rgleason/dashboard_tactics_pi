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

        // Gauge
        if ( newluminosity == 'day') {
            that.gauge[0].labelFontColor = '#262626'
            that.gauge[0].update('labelFontColor', '#262626')
            that.gauge[0].valueFontColor = '#101566'
            that.gauge[0].update('valueFontColor', '#101566')
        }
        else if ( newluminosity == 'dusk') {
            that.gauge[0].labelFontColor = '#8389e0'
            that.gauge[0].update('labelFontColor', '#8389e0')
            that.gauge[0].valueFontColor = '#232b99'
            that.gauge[0].update('valueFontColor', '#232b99')
        }
        else if ( newluminosity == 'night') {
            that.gauge[0].labelFontColor = '#d3d5f5'
            that.gauge[0].update('labelFontColor', 'd3d5f5')
            that.gauge[0].valueFontColor = '#aaaeeb'
            that.gauge[0].update('valueFontColor', 'aaaeeb')
        }
    }
    return
}

