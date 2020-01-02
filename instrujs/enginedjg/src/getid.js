/* $Id: getid.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to ask and retrieve from a client a unique ID for this instance

export function getidAskClient() {
    window.iface.setFlag('bottom', 'getid')
}

export function getidClientAnswer() {
    window.iface.clearFlag()
    return window.iface.getid()
}
