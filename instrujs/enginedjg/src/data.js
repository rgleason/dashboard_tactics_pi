/* $Id: data.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to ask and retrieve from a client a unique ID for this instance


export function showData( that ) {
    that.glastvalue = window.iface.getdata()
    if ( (gauge.length > 0) && (that.glastvalue != null) )
        gauge[0].refresh( that.glastvalue)
}

