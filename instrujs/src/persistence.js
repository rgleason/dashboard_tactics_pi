/* $Id: persistence.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
/*
  LocalStorage (JSON objects) - works (usually) with file:// URIs on WebKitbut can be
  blocked by policy. On MSIE it is cookies which work on WebView backend. Likewise,
  the cookies are blocked usually by a policy. So the http:// should be suggested
  to the end user if nothing works on his/her WebKit/MSIE.
*/

var alertsenabled = window.instrustat.alerts
var dbglevel = window.instrustat.debuglevel
var bSelfTest = false
var bLocalStorage = false
var bCookies = false
var bStatic = false

export function loadConf( cid, locProtocol, emptyConf ) {

    if ( dbglevel > 0 ) console.log('persistence.js loadConf() ', cid, locProtocol)

    // Priority for static confiruation even if we do not encourage for it
    try {
        var statConf = window.instrustatconf.getObj( cid )
        if ( statConf != null ) {
            bStatic = true
            return statConf
        }
        if ( dbglevel > 1 )
            console.log('persistence.js loadConf(): no static config found')
    }
    catch ( error ) {
        if ( dbglevel > 0 )
            console.error(
                'persistence.js loadConf() - instrustatconf.getObj() error: ',
                error)
        if ( alertsenabed )
            alert('ERROR in common.js!\n' + error )
        return null
    } // then tis exception is worthwhile to tell the user
    
    // Using local storage is indeed better, but is any available from backend?
    if ( !bSelfTest ) {
        SelfTest( locProtocol )
    }
    
    if ( !bLocalStorage && !bCookies ) {
        if ( dbglevel > 0 ) console.error(
            'persistence.js: loadConf(): no local storage support available,')
        if ( dbglevel > 1 ) console.log(
            'probably a back-end (IE or WebKit) policy prevents the usage ',
            'for the used protocol: ', locProtocol)
        if ( alertsenabled )
            alert('Saving of settings not available!\n' +
                  'System policy prevents local storage\n' +
                  'The protocol (ini/conf) for instrument is "' + locProtocol +
                  '//"\n' +
                  'Try another protocol or static configuration.\n')
        return null
    }
    if ( bLocalStorage )
        return getObj( cid )
    // cookies are available as our final hope!
    if ( emptyConf == null ) {
        if ( dbglevel > 0 ) console.error(
            'persistence.js: loadConf(): no emptyConf structure given')
        return null
    } // then we do not have a model to fetch the elements for
    var loadObj = emptyConf
    Object.keys( emptyConf ).forEach( function( key, index ) {
        var lparam = getParam ( key, cid )
        if ( lparam != null )
            loadObj[key] = lparam
    })
    return loadObj
}

export function saveConf( cid, confObj ) {
    if ( bStatic )
        return; // nothing to save
    if ( !bSelfTest ) {
        SelfTest( locProtocol )
    }
    if ( !bLocalStorage && !bCookies ) {
        if ( dbglevel > 0 )
            console.error('persistence.js saveConf(): no storage available')
        return false
    }
    if ( bLocalStorage )
        return saveObj( cid )
    // cookies are available!
    Object.keys(loadObj).forEach(function(key,index) {
        if ( !saveParam( key, cid, confObj[key]) )
            return false
    })
    return true
}

function saveObj( cid, cobj ) {
    if ( dbglevel > 0 ) console.log('saveObj(): ', cid, cobj )
    // let's avoid mess
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js saveObj(): no cid')
        return false
    }
    var nCobj = cobj || null
    if ( nCobj == null ) {
        if ( dbglevel > 0 ) console.error('persistence.js saveObj(): cobj is null')
        return false
    }
    try {
        localStorage.setItem( cid, JSON.stringify( cobj ) )
        return true
    }
    catch ( error ) {
        if ( dbglevel > 0 ) console.error('persistence.js saveObj(): error: ', error )
        return false
    }
}

function getObj( cid ) {
    if ( dbglevel > 0 ) console.log('getObj(): ', cid)
    // let's avoid chasing phantom data
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js getObj(): no cid')
        return null
    }
    try {
       return JSON.parse( localStorage.getItem( cid ) )
    }
    catch (error) {
        if ( dbglevel > 0 ) console.error('persistence.js getObj(): error: ', error)
        return null
    }
}

function deleteObj( cid ) {
    if ( dbglevel > 0 ) console.log('deleteObj(): ', cid)
    // deleting phantom data would be useless
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js deleteObj(): no cid')
        return false
    }
    try {
        localStorage.removeItem( cid )
        return true
    }
    catch (error) {
        if ( dbglevel > 0 ) console.error('persistence.js deleteObj(): error: ', error)
        return false
    }
}


// Cookies (do not use w/ file:// URIs, only http:// or https::// )

function saveParam( cname, cid, cvalue, inexdays ) {
    if ( dbglevel > 0 ) console.log('saveParam(): ', cname, cid, cvalue, inexdays)
    // let's avoid creating useless cookies
    var nCname = cname || null
    if ( (nCname == null) || (nCname == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js saveParam(): no cname')
        return false
    }
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js saveParam(): no cid')
        return false
    }
    var nCvalue = cvalue || null
    if ( nCvalue == null ) {
        if ( dbglevel > 0 ) console.error('persistence.js saveParam(): cvalue is null')
        return false
    }
    var d = new Date()
    var expires = ''
    var exdays = inexdays || null
    if ( exdays == null )
        exdays = 365 // unused will disappear after 1 year
    d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000))
    var expires = ';expires='+d.toUTCString()
    var cookiestr = cid + '-' + cname + '=' + cvalue + expires + ';path=/'
    if ( dbglevel > 0 ) console.log('saveParam():', cookiestr)
    try {
        document.cookie = cookiestr
        return true
    }
    catch( error ) {
        if ( dbglevel > 0 ) console.error('persistence.js saveParam(): cvalue is document.cookie exception: ', error)
        return false
    }
}

function getParam( cname, cid ) {
    if ( dbglevel > 0 ) console.log('getParam(): ', cname, cid)
    // let's avoid chasing phantom cookies
    var nCname = cname || null
    if ( (nCname == null) || (nCname == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js getParam(): no cname')
        return null
    }
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js getParam(): no cid')
        return null
    }
    var allpiparit = document.cookie
    if ( dbglevel > 2 ) console.log('persistence.js getParam(): document.cookie: "',
                                    allpiparit, '"')
    var piparit = allpiparit.split(';')
    var firstelemname = cid + '-' + cname
    var elemname = ' ' + firstelemname
    for ( var i = 0; i < piparit.length; i++ ) {
        var pipari = piparit[ i ].split('=')
        if ( pipari.length == 2 ) {
            if ( dbglevel > 2 ) console.log(
                'persistence.js getParam(): pipari[0] :"', pipari[0],
                '" pipari[1]:"', pipari[1], '"')
            var comp = elemname
            if ( i == 0 )
                comp = firstelemname
            if ( pipari[0] === comp ) {
                if ( dbglevel > 0 ) console.log(
                    'getParam(): returning:"', pipari[1])
                return pipari[1]
            }
        }
        else if ( dbglevel > 2 ) {
            console.log(
                'persistence.js getParam(): non-formed piparit[', i, '] :"',
                piparit[i])
        }
    }
    if ( dbglevel > 0 ) console.log('getParam(): ', name, ' not found.')
    return null
}

function deleteParam( cname, cid ) {
    if ( dbglevel > 0 ) console.log('deleteParam(): ', cname, cid)
    // deleting phantom cookies would be useless
    var nCname = cname || null
    if ( (nCname == null) || (nCname == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js deleteParam(): no cname')
        return false
    }
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        if ( dbglevel > 0 ) console.error('persistence.js deleteParam(): no cid')
        return false
    }
    var expires = ';expires=Thu, 01 Jan 1970 00:00:00 UTC'
    var cookiestr = cid + '-' + cname + '=' + expires + ';path=/'
    if ( dbglevel > 1 ) console.log('deleteParam():', cookiestr)
    try {
        document.cookie = cookiestr
        return true
    }
    catch( error ) {
        if ( dbglevel > 0 ) console.error('persistence.js deleteParam(): cvalue is document.cookie exception: ', error)
        return false
    }
}

function SelfTest( locProtocol ) {

    if ( bSelfTest )
        return true
    bSelfTest = true
    if( dbglevel > 0 ) console.log('perstince.js SelfTest()')

    var protocol = locProtocol

    try {
        var sKey  = 'test.persistence.js'
        var sTest = 'test'
        localStorage.setItem( sKey, sTest )
        var sComp = localStorage.getItem( sKey )
        localStorage.removeItem( sKey )
        if ( sTest === sComp ) {
            bLocalStorage = true
            if ( dbglevel > 0 ) console.log(
                'localStorage() support is available, protocol: ', protocol)
        }
        else
            if ( dbglevel > 0 ) console.log(
                'localStorage() support but verification failed, protocol: ',
                protocol)
    }
    catch( error ) {
        if ( dbglevel > 0 ) console.log(
            'localStorage() support not available, protocol: ', protocol)
    }

    var cNameTest = 'test-cookie-by-persistence-js'
    var cCidTest  = String( Math.floor(Math.random() * Math.floor(999999)) )
    var cValTest  = 'test'
    try {
        if ( saveParam( cNameTest, cCidTest, cValTest ) ) {
            var cValComp = getParam( cNameTest, cCidTest )
            if ( cValComp === cValTest ) {
                if ( dbglevel > 0 ) console.log(
                    'document.cookie support is available, protocol: ', protocol)
                bCookies = true
                if ( !deleteParam( cNameTest, cCidTest ) )
                    if ( dbglevel > 0 ) console.log(
                        'document.cookie: warning: cannot delete the test cookie: ',
                        cNameTest, '-', cCidTest)
            }
            else {
                if ( dbglevel > 0 ) console.log(
                    'document.cookie support not available (cannot read back)',
                    ', protocol: ', protocol)
            }
        }
        else {
            if ( dbglevel > 0 ) console.log(
                'document.cookie support not available (cannot save), protocol: ',
                protocol)
        }
    }
    catch( error) {
        if ( dbglevel > 0 ) console.log(
            'document.cookie support not available, protocol: ',
            protocol, ' error: ', error)
    }
    return
}
