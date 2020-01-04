/* $Id: path.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to ask and retrieve from a client a unique ID for this instance


export function getallAskClient() {
    window.iface.setFlag('bottom', 'getall')
}

export function getallClientAnswer( that ) {
    that.allpaths= window.iface.getall()
}

export function getpathAskClient( that ) {
    if ( that.path != '' )
        window.iface.setFlag('bottom', that.path )
}

