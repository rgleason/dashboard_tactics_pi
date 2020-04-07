/* $Id: path.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to ask and retrieve from a client a unique ID for this instance

var alertsenabled = window.instrustat.alerts
var dbglevel = window.instrustat.debuglevel

var schema

export function initPaths( that ) {
    var emptySchema = {
        path: '',
        url: '',
        org: '',
        token: '',
        bucket: '',
        sMeasurement: '',
        sProp1: '',
        sProp2: '',
        sProp3: '',
        sField1: '',
        sField2: '',
        sField3: ''
    }
    schema = emptySchema
}

export function getPathSchema(){
    return schema
}

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

export function getschemaAcknowledged( that ) {
    window.iface.clearFlagById( 'bottom' )
}

export function gotAckCheckSchema( that ) {
    if ( dbglevel > 0 )
        console.log('path.js gotAckChecSchema()')
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
    alert(window.iface.getdbschema())
    var ackSchema = JSON.parse(window.iface.getdbschema())
    if ( dbglevel > 1 )
        console.log('path.js gotAckCheckPath() - acknowledged schema: ', ackSchema)
    if ( (ackSchema !== null) && (ackSchema !== '') ) {
        if ( ackSchema.path === expectedpath ) {
            if ( dbglevel > 0 )
                console.log('path.js gotAckCheckPath() - acknowledged schema path matches: ', expectedpath)
        }
        else {
            if ( dbglevel > 0 )
                console.log('path.js gotAckCheckPath() - acknowledged schema path mismatch! Requested: ', expectedpath,
                            ', got: ', ackSchema.path)
            if ( alertsenabled )
                alert ( window.instrulang.errSubscriptionAck1 + '\n' +
                        expectedpath + '\n' +
                        window.instrulang.errSubscriptionAck2 + '\n' +
                        ackSchema.path )
        }
    }
    else {
        if ( dbglevel > 0 )
            console.log('path.js gotAckCheckPath() - acknowledged schema is empty or null! Requested: ', expectedpath)
        if ( alertsenabled )
            alert ( window.instrulang.errSubscriptionAck1 + '\n' +
                    expectedpath + '\n' +
                    window.instrulang.errSubscriptionAck2 + '\n' +
                    '' )
    }
    schema = ackSchema
    return
}
