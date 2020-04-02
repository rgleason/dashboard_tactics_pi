/* $Id: path.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to ask and retrieve from a client a unique ID for this instance

var alertsenabled = window.instrustat.alerts
var dbglevel = window.instrustat.debuglevel

export function getalldbAskClient() {
    window.iface.setFlag('bottom', 'getalldb')
}

export function getalldbClientAnswer( that ) {
    that.allpaths= window.iface.getalldb()
}

export function getpathAskClient( that ) {
    if ( that.perspath ) {
        if ( that.conf === null ) {
            if ( dbglevel > 0 )
                console.log('path.js getpathAskClient() - static path suggested but conf object is null!')
            return
        }
        if ( (that.conf.path === null) || (that.conf.path === '') ) {
            if ( dbglevel > 0 )
                console.log('path.js getpathAskClient() - static path suggested but conf.path is empty or null!')
            return
        }
        window.iface.setFlag( 'bottom', that.conf.path )
    }
    else {
        that.path = window.iface.getselected()
        if ( that.path !== '' )
            window.iface.setFlag( 'bottom', that.path )
    }
}

export function getpathAcknowledged( that ) {
    window.iface.clearFlagById( 'bottom' )
}

export function gotAckCheckPath( that ) {
    if ( dbglevel > 0 )
        console.log('path.js gotAckCheckPath()')
    var expectedpath = that.path
    if ( that.perspath ) {
        if ( that.conf === null ) {
            if ( dbglevel > 0 )
                console.log('path.js gotAckCheckPath() - static path suggested but conf object is null!')
        }
        if ( (that.conf.path === null) || (that.conf.path === '') ) {
            if ( dbglevel > 0 )
                console.log('path.js gotAckCheckPath() - static path suggested but conf.path is empty or null!')
        }
        expectedpath = that.conf.path
    }
    var acknowledgedpath = window.iface.getacksubs()
    if ( dbglevel > 1 )
        console.log('path.js gotAckCheckPath() - acknowledgedpath: ', acknowledgedpath)
    if ( (acknowledgedpath !== null) && (acknowledgedpath !== '') ) {
        if ( acknowledgedpath === expectedpath ) {
            if ( dbglevel > 0 )
                console.log('path.js gotAckCheckPath() - acknowledgedpath matches: ', expectedpath)
        }
        else {
            if ( dbglevel > 0 )
                console.log('path.js gotAckCheckPath() - acknowledgedpath mismatch! Requested: ', expectedpath,
                            ', got: ', acknowledgedpath)
            if ( alertsenabled )
                alert ( window.instrulang.errSubscriptionAck1 + '\n' +
                        expectedpath + '\n' +
                        window.instrulang.errSubscriptionAck2 + '\n' +
                        acknowledgedpath )
        }
    }
    else {
        if ( dbglevel > 0 )
            console.log('path.js gotAckCheckPath() - acknowledgedpath is empty or null! Requested: ', expectedpath)
        if ( alertsenabled )
            alert ( window.instrulang.errSubscriptionAck1 + '\n' +
                    expectedpath + '\n' +
                    window.instrulang.errSubscriptionAck2 + '\n' +
                    '' )
    }
    return
}
