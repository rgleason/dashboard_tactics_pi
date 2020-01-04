/* $Id: getid.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to ask and retrieve from a client a unique ID for this instance
import { loadConf } from '../../src/persistence'
import { createEmptyConf } from './conf'

var dbglevel = window.instrustat.debuglevel

export function getidAskClient() {
    window.iface.setFlag('bottom', 'getid')
}

export function getidClientAnswer( that ) {
    that.uid = window.iface.getid()
    if ( dbglevel > 1 ) console.log('getidClientAnswer(): that.uid : ', that.uid )
}
export function getidConf( that ) {
    var emptyConf = createEmptyConf()
    if ( !(that.uid == '') && !(that.uid == null) )
        that.conf = loadConf( that.uid, that.locInfo.protocol, emptyConf )
    if ( dbglevel > 1 ) console.log('getidClientAnswer(): that.conf : ', that.conf )
}
