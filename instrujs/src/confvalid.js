/* $Id: confvalid.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to validate a configuration
var alertsenabled = window.instrustat.alerts
var dbglevel = window.instrustat.debuglevel

import {upgradeConfVersion} from './confupgrade'

function getVersionNumber( ofConf ) {
    var retval = 0
    try {
        if ( dbglevel > 2 )
            console.log('getVersionNumber(): searching for a version number:')
        Object.entries( loadedConf ).forEach( function( key, value ) {
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
                if ( (key[1] != null) && (typeof key[1] === 'number') ) {
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
        if ( refConfKeys.length != loadedConfKeys ) {
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
            else if ( loadedConfKeys.length != refConfKeys.length ) {
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

    try {
        if ( (loadedConf.path === null) || (loadedConf.path === '') ) {
            if ( dbglevel > 1 )
                console.log('checkConf(): the object does not have a path-key or it is empty.',
                            ' Cannot continue.')
            return null
        }
    }
    catch ( error ) {
        if ( dbglevel > 1 )
            console.log('checkConf(): exception while inspecting the object (path), error: ', error)
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
