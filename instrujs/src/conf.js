/* $Id: conf.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to load, find and store configuration
var alertsenabled = window.instrustat.alerts
var dbglevel = window.instrustat.debuglevel
import { loadConf, saveConf } from './persistence'

var hasPersistentOrStaticConf = false;

// Polyfills, for IE back-end (!) on Windows used by WebView
if (!Object.entries)
    Object.entries = function( obj ){
        var ownProps = Object.keys( obj ),
            i = ownProps.length,
            resArray = new Array(i); // preallocate the Array
        while (i--)
            resArray[i] = [ownProps[i], obj[ownProps[i]]]
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
    return true;
}

export function createEmptyConf() {
    return {
        version    : 1,
        path       : '',
        title      : '',
        unit       : '',
        decimals   : 1,
        minval     : 0,
        loalert    : 0,
        hialert    : 0,
        maxval     : 100,
        multiplier : 1,
        divider    : 1,
        offset     : 0
    }
}
 
export function getConf( that ) {
    if ( dbglevel > 1 )
        console.log('getConf()')
    if ( (that.uid == '') || (that.uid == null) ) {
        if ( dbglevel > 1 ) console.log('getConf(): no that.uid.')
        return
    }
    var emptyConf = createEmptyConf()
    var loadedConf = loadConf( that.uid, that.locInfo.protocol )
    that.conf = checkConf( loadedConf, emptyConf )
    if ( that.conf == null )
        that.conf = emptyConf
    return
}

export function getPathDefaultsIfNew ( that ) {
    if ( dbglevel > 1 ) console.log(
        'getPathDefaultsIfNew()')
    if ( hasPersistentOrStaticConf ) {
        if ( dbglevel > 1 ) console.log(
            'getPathDefaultsIfNew(): the object has already a persistent configuration, will not override, path: ', that.path)
        return
    }
    var emptyConf = createEmptyConf()
    var defConfObj
    try {
        defConfObj = window.instrustat.skpathlookup( that.path )
    }
    catch ( error ) {
        if ( dbglevel > 0 ) console.error(
            'getPathDefaultsIfNew(): exception occurred when parsing common.js, looking for ',
            that.path, ', got error: ', error)
        if ( alertsenabled )
            alert( window.instrulang.errCommonJs + '\n' + that.path + '\n' + error )
        return
    }
    if ( defConfObj == null ) {
        if ( dbglevel > 1 ) console.log(
            'getPathDefaultsIfNew(): could not get default object with the selected path, ', that.path)
        return
    }
    if ( dbglevel > 1 ) console.log(
        'getPathDefaultsIfNew(): got following static object for path (', that.path, ') : ', defConfObj)
    that.conf = checkConf( defConfObj, emptyConf )
    if ( that.conf == null )
        that.conf = emptyConf
    saveConf( that.uid, that.conf )
    return
}

export function prepareConfHalt ( that ) {
    if ( dbglevel > 0 ) console.log('prepareConfHalt()')
    if ( that.conf == null ) {
        if ( dbglevel > 1 ) console.log('checkConf(): configuration object is null, nothing to do.')
        return
    }
    if ( (that.uid == null) || (that.uid == '') ) {
        if ( dbglevel > 1 ) console.log('checkConf(): there is no valid UID, nothing to do.')
        return
    }
    saveConf( that.uid, that.conf )
}

function checkConf ( loadedConf, refConf ) {
    if ( loadedConf == null ) {
        if ( dbglevel > 1 ) console.log('checkConf(): inspected object is null, returning null')
        return null
    }
    if ( refConf == null ) {
        if ( dbglevel > 1 ) console.log('checkConf(): reference object is null, returning null')
        return null
    }
    // Inspect the returned object against incompatibility or older version number
    try {
        var refConfKeys    = Object.keys( refConf )
        var loadedConfKeys = Object.keys( loadedConf )
        if ( refConfKeys.length != loadedConfKeys ) {
            var loadedConfVersion = 0
            try {
                if ( dbglevel > 2 )
                    console.log('checkConf(): searching for a version number:')
                Object.entries( loadedConf ).forEach( function( key, value ) {
                    if ( dbglevel > 2 )
                        console.log('checkConf(): key ', key, ' key.length ', key.length, ' value ', value)
                    if ( key.length > 1 ) {
                        if ( key[0] == 'version' ) {
                            var vers = key[1]
                            if ( dbglevel > 2 )
                                console.log('checkConf(): vers ', vers)
                            if ( typeof vers == 'number' )
                                if ( dbglevel > 2 )
                                    console.log('checkConf(): vers is a typeof "number"')
                            if ( (key[1] != null) && (typeof key[1] == 'number') ) {
                                if ( dbglevel > 2 )
                                console.log('checkConf(): version key and number value found')
                                loadedConfVersion = key[1]
                            }
                        }
                    }
                })
            }
            catch ( error ) {
                if ( dbglevel > 1 )
                    console.log(
                        'checkConf(): exception while inspecting the object (entries), error: ', error)
            }
            if ( loadedConfVersion == 0 ) {
                if ( dbglevel > 1 )
                    console.log('checkConf(): the object has unkwon or zero version number - ignoring!')
                return nujll
            }
            else if ( loadedConfVersion < refConf.version ) {
                if ( dbglevel > 1 )
                    console.log('checkConf(): Object with lower version number, ', loadedConfVersion,
                                ' compared to current version number, ', refConf.version, ' - upgrading.')
                loadedConf = upgradeConfVersion( loadedConfVersion, loadedConf )
                if ( loadedConf == null ) {
                    if ( dbglevel > 1 )
                        console.log('checkConf(): upgradeConfVersion() fails in upgrade, returns null, ignoring all.')
                    return null
                }
            }
            else {
                if ( loadedConfKeys.length != refConfKeys.length ) {
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
    }
    catch ( error ) {
        if ( dbglevel > 1 )
            console.log('checkConf(): exception while inspecting the object (keys), error: ', error)
            return null
    }

    try {
        if ( (loadedConf.path == null) || (loadedConf.path == '') ) {
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

function upgradeConfVersion( loadecConfVersion, loadedConf ) {
    return null
}
