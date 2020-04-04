/* $Id: css.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/* Requires that the JS/TS has a local ../sass/style.scss containing:
.skpath.propl.day   { color: instrujs.$skpathcolorday; }
.skpath.propl.dusk  { color: instrujs.$skpathcolordusk; }
.skpath.propl.night { color: instrujs.$skpathcolornight; }
.skpath.fixed.day   { color: instrujs.$skpathcolorday; }
.skpath.fixed.dusk  { color: instrujs.$skpathcolordusk; }
.skpath.fixed.night { color: instrujs.$skpathcolornight; }
 */

var skpathispropl = false

export function setSkPathFontResizingStyle() {
    var hasproplsupport = false
    var hadexception = false
    var ua = window.navigator.userAgent
    var msie = ua.indexOf('MSIE ')
    try {
        if ( CSS.supports ) {
            if ( CSS.supports('font-size',
                              getComputedStyle(
                                  document.documentElement).getPropertyValue(
                                      '--skpproplw')) ) {
                console.log('Has viewport proportional font size support')
                hasproplsupport = true
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
        document.getElementById('skPath').className = 'skpath propl day'
    }
    else
        document.getElementById('skPath').className = 'skpath fixed day'
    return
}

export function hasProportionalFontSupport() {
    return skpathispropl
}
