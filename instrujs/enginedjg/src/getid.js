/* $Id: getid.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to ask and retrieve from a client a unique ID for this instance

import { loadConf } from '../../src/persistence'

export function getidAskClient() {
    window.iface.setFlag('bottom', 'getid')
}

export function getidClientAnswer( that ) {
    that.uid = window.iface.getid()
    if ( !(that.uid == '') && !(that.uid == null) )
        that.conf = loadConf( that.uid, that.locInfo.protocol )
}
