/* $Id: unloadwebkitiescrollbars.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// The below WebKit / IE compatible function will not pass TS transpiler
export default function unloadWebKitIEScrollBars() {
    document.documentElement.style.overflow = 'hidden' // webkit
    document.body.scroll = 'no' // ie
}
