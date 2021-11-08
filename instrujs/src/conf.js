/* $Id: conf.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

//An actor to load, find and store configuration
var alertsenabled = window.instrustat.alerts
var dbglevel = window.instrustat.debuglevel
import { loadConf, saveConf } from './persistence'
import  {checkConf } from './confvalid'

export function createEmptyConf() {
    return {
        version    : 1,
        path       : '',
        title      : '',
        symbol     : '',
        unit       : '',
        display    : 'dial',
        decimals   : 1,
        minval     : 0,
        loalert    : 0,
        hialert    : 0,
        maxval     : 100,
        multiplier : 1,
        divider    : 1,
        offset     : 0,
        dbfunc     : '',
        dbnum      : 0,
        wrnmsg     : false
    }
}

export function getConf( that ) {
    if ( dbglevel > 1 )
        console.log('getConf()')
    if ( (that.uid === '') || (that.uid === null) ) {
        if ( dbglevel > 1 ) console.log('getConf(): no that.uid.')
        return
    }
    var emptyConf = createEmptyConf()
    var loadedConf = loadConf( that.uid, that.locInfo.protocol )
    that.conf = checkConf( loadedConf, emptyConf )
    if ( that.conf === null )
        that.conf = emptyConf
    return
}

export function getPathDefaultsIfNew ( that ) {
    if ( dbglevel > 1 ) console.log(
        'getPathDefaultsIfNew() - that.path: ', that.path)
    var emptyConf = createEmptyConf()
    var defConfObj
    try {
        defConfObj = window.instrustat.skpathlookup( that.path )
        defConfObj.path = that.path // override eventual wildcards
    }
    catch ( error ) {
        if ( dbglevel > 0 ) console.error(
            'getPathDefaultsIfNew(): exception occurred when parsing common.js, looking for ',
            that.path, ', got error: ', error)
        if ( alertsenabled )
            alert( window.instrulang.errCommonJs + '\n' + that.path + '\n' + error )
        return
    }
    if ( defConfObj === null ) {
        if ( dbglevel > 1 ) console.log(
            'getPathDefaultsIfNew(): could not get default object with the selected path, ', that.path)
        return
    }
    if ( dbglevel > 1 ) console.log(
        'getPathDefaultsIfNew(): got following static object for path (', that.path, ') : ', defConfObj)
    that.conf = checkConf( defConfObj, emptyConf )
    if ( that.conf === null )
        that.conf = emptyConf
    saveConf( that.uid, that.conf )
    return
}

export function memorizeSettings ( that ) {
    if ( that.conf !== null )
        saveConf( that.uid, that.conf )
}

export function clearConf ( that ) {
    that.conf = null
    that.perspath = false
    that.path = ''
}

export function prepareConfHalt ( that ) {
    if ( dbglevel > 0 ) console.log('prepareConfHalt()')
    if ( that.conf === null ) {
        if ( dbglevel > 1 ) console.log('checkConf(): configuration object is null, nothing to do.')
        return
    }
    if ( (that.uid === null) || (that.uid === '') ) {
        if ( dbglevel > 1 ) console.log('checkConf(): there is no valid UID, nothing to do.')
        return
    }
    saveConf( that.uid, that.conf )
}
