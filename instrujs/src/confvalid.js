/* $Id: confvalid.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to validate a configuration
var alertsenabled = window.instrustat.alerts
var dbglevel = window.instrustat.debuglevel

import { upgradeConfVersion } from './confupgrade'

// Polyfills, for IE back-end (!) on Windows used by WebView
if (!Object.entries)
    Object.entries = function( obj ){
        var ownProps = Object.keys( obj )
        var i = ownProps.length
        var resArray = new Array(i) // preallocate the Array
        while ( i > 0 ) {
            i--
            resArray[ parseInt(i) ] = [ownProps[ parseInt(i) ], obj[ownProps[ parseInt(i) ]]]
        }
        return resArray
    }
if (!Object.keys) Object.keys = function(o) {
   var k=[],p
    for (p in o)
        if (Object.prototype.hasOwnProperty.call(o,p))
            k.push(p)
  return k
}
function areEqualShallowKeys( a, b ) {
    for ( var key in a ) {
        if ( !(key in b) ) {
            return false
        }
    }
    return true
}


function getVersionNumber( ofConf ) {
    var retval = 0
    try {
        if ( dbglevel > 2 )
            console.log('getVersionNumber(): searching for a version number:')
        Object.entries( ofConf ).forEach( function( key, value ) {
        if ( dbglevel > 2 )
            console.log('getVersionNUmber(): key ', key, ' key.length ', key.length, ' value ', value)
        if ( key.length > 1 )
            if ( key[0] === 'version' ) {
                var vers = key[1]
                if ( dbglevel > 2 )
                    console.log('getVersionNUmber(): vers ', vers)
                if ( typeof vers === 'number' )
                    if ( dbglevel > 2 )
                        console.log('getVersionNUmber(): vers is a typeof "number"')
                if ( (key[1] !== null) && (typeof key[1] === 'number') ) {
                    if ( dbglevel > 2 )
                        console.log('getVersionNUmber(): version key and number value found')
                    retval = key[1]
                }
            }
        })
    }
    catch ( error ) {
        if ( dbglevel > 1 )
            console.log(
                'getVersionNUmber(): exception while inspecting the object (entries), error: ', error)
    }
    return retval
}

/* ------------------------------------------------------------------------------------------- */
export function checkConf ( loadedConf, refConf ) {
    if ( loadedConf === null ) {
        if ( dbglevel > 1 ) console.log('checkConf(): inspected object is null, returning null')
        return null
    }
    if ( refConf === null ) {
        if ( dbglevel > 1 ) console.log('checkConf(): reference object is null, returning null')
        return null
    }
    // Inspect the returned object against incompatibility or older version number
    try {
        var refConfKeys    = Object.keys( refConf )
        var loadedConfKeys = Object.keys( loadedConf )
        if ( refConfKeys.length !== loadedConfKeys ) {
            var loadedConfVersion = getVersionNumber( loadedConf )
            if ( loadedConfVersion === 0 ) {
                if ( dbglevel > 1 )
                    console.log('checkConf(): the object has unkwon or zero version number - ignoring!')
                return null
            }
            else if ( loadedConfVersion < refConf.version ) {
                if ( dbglevel > 1 )
                    console.log('checkConf(): Object with lower version number, ', loadedConfVersion,
                                ' compared to current version number, ', refConf.version, ' - upgrading.')
                loadedConf = upgradeConfVersion( loadedConfVersion, loadedConf )
                if ( loadedConf === null ) {
                    if ( dbglevel > 1 )
                        console.log('checkConf(): upgradeConfVersion() fails in upgrade, returns null, ignoring all.')
                    return null
                }
            }
            else if ( loadedConfKeys.length !== refConfKeys.length ) {
                if ( dbglevel > 1 )
                    console.log(
                       'checkConf(): the object has different number of keys, ', loadedConfKeys.length,
                       ' compared to thr reference object\'s number of keys, ', refConfKeys.length, ' - cannot continue.')
                if ( alertsenabled )
                    alert ( window.instrulang.errNofConfKeysDoNotMatch1 + '\n' +
                            window.instrulang.errNofConfKeysDoNotMatch2 + '\n' +
                            window.instrulang.errNofConfKeysDoNotMatch3 + ' ' + refConfKeys.length, + ' ' +
                            window.instrulang.errNofConfKeysDoNotMatch4 + ' ' + loadedConfKeys.length )
                return null
            }
        }
    }
    catch ( error ) {
        if ( dbglevel > 1 )
            console.log('checkConf(): exception while inspecting the object (keys), error: ', error)
            return null
    }

    if ( typeof loadedConf.path === 'undefined' ) {
        if ( dbglevel > 1 )
            console.log('checkConf(): the object has no defined memeber "path".')
        return null
    }

    if ( (loadedConf.path === null) || (loadedConf.path === '') ) {
        if ( dbglevel > 1 )
            console.log('checkConf(): the object has a member "path" but it is empty.')
        return null
    }

    // Make a shallow comparison for the same keys
    try {
        if ( !areEqualShallowKeys( refConf, loadedConf ) ) {
            if ( dbglevel > 1 )
                console.log(
                    'checkConf(): Objects do not have the same keys (EqualyShallow test).',
                    ' Cannot continue.')
            return null
        }
    }
    catch ( error ) {
        if ( dbglevel > 1 )
            console.log(
                'checkConf(): exception while inspecting the object (EqualyShallow - keys), error: ', error)
        return null
    }
    return loadedConf
}
