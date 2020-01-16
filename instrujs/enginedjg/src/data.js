/* $Id: data.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to ask and retrieve from a client a unique ID for this instance
import { getPathDefaultsIfNew } from '../../src/conf'
var dbglevel = window.instrustat.debuglevel

export function onWaitdataFinalCheck( that ) {
    var elem = document.getElementById('skPath')
    if ( that.conf != null ) {
        if ( (that.conf.title != null) && (that.conf.title != '' ) )
            elem.innerHTML = that.conf.title
        else if ( (that.conf.path != null) && (that.conf.path != '' ) ) {
            getPathDefaultsIfNew( that )
            if ( (that.conf.title != null) && (that.conf.title != '' ) )
                elem.innerHTML = that.conf.title
            else
                elem.innerHTML = that.conf.path
        }
    }
    else if ( (that.path != null) && (that.path != '' ) ) {
        getPathDefaultsIfNew( that )
        if ( (that.conf.title != null) && (that.conf.title != '' ) )
            elem.innerHTML = that.conf.title
        else
            elem.innerHTML = that.conf.path
    }
    else {
        if ( dbglevel > 1 )
            console.error('onWaitdataFinalCheck(): no path, no conf!')
    }
}

export function showData( that ) {
    that.glastvalue = window.iface.getdata()
    if ( (that.gauge.length > 0) && (that.glastvalue != null) )
        that.gauge[0].refresh( that.glastvalue, that.conf.maxval, that.conf.minval, that.conf.unit )
}

export function clearData( that ) {
    that.glastvalue = 0
    if ( (that.gauge.length > 0) && (that.glastvalue != null) )
        that.gauge[0].refresh( that.glastvalue )
}

export function prepareDataHalt( that ) {
    clearData( that );
}
