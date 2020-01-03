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
var bSelfTest = false
var bLocalStorage = false
var bCookies = false

export function loadConf( cid, locProtocol ) {
    if ( !bSelfTest ) {
        SelfTest( locProtocol )
    }
    if ( !bLocalStorage && !bCookies ) {
        console.error('persistence.js loadConf(): no storage available')
        return null
    }
    if ( bLocalStorage )
        return getObj( cid )
    // cookies are available!
    var loadObj = confObj
    Object.keys(loadObj).forEach(function(key,index) {
        var lparam = getParam ( key, cid )
        if ( lparam != null )
            loadObj[key] = lparam
    })
    return loadObj
}

export function saveConf( cid, confObj ) {
    if ( !bSelfTest ) {
        SelfTest( locProtocol )
    }
    if ( !bLocalStorage && !bCookies ) {
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
    console.log('saveObj(): ', cid, cobj )
    // let's avoid mess
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        console.error('persistence.js saveObj(): no cid')
        return false
    }
    var nCobj = cobj || null
    if ( nCobj == null ) {
        console.error('persistence.js saveObj(): cobj is null')
        return false
    }
    try {
        localStorage.setItem( cid, JSON.stringify( cobj ) )
        return true
    }
    catch ( error ) {
        console.error('persistence.js saveObj(): error: ', error )
        return false
    }
}
function getObj( cid ) {
    console.log('getObj(): ', cid)
    // let's avoid chasing phantom data
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        console.error('persistence.js getObj(): no cid')
        return null
    }
    try {
       return JSON.parse( localStorage.getItem( cid ) )
    }
    catch (error) {
        console.error('persistence.js getObj(): error: ', error)
        return null
    }
}
function deleteObj( cid ) {
    console.log('deleteObj(): ', cid)
    // deleting phantom data would be useless
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        console.error('persistence.js deleteObj(): no cid')
        return false
    }
    try {
        localStorage.removeItem( cid )
        return true
    }
    catch (error) {
        console.error('persistence.js deleteObj(): error: ', error)
        return false
    }
}


// Cookies (do not use w/ file:// URIs, only http:// or https::// )

function saveParam( cname, cid, cvalue, inexdays ) {
    console.log('saveParam(): ', cname, cid, cvalue, inexdays)
    // let's avoid creating useless cookies
    var nCname = cname || null
    if ( (nCname == null) || (nCname == '') ) {
        console.error('persistence.js saveParam(): no cname')
        return false
    }
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        console.error('persistence.js saveParam(): no cid')
        return false
    }
    var nCvalue = cvalue || null
    if ( nCvalue == null ) {
        console.error('persistence.js saveParam(): cvalue is null')
        return false
    }
    var d = new Date()
    var expires = ''
    var exdays = inexdays || null
    if ( exdays == null )
        exdays = 365 // unused will disappear after 1 year
    d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000))
    var expires = ';expires='+d.toUTCString()
    var cookiestr = cname + '-' + cid + '=' + cvalue + expires + ';path=/'
    console.log('saveParam():', cookiestr)
    try {
        document.cookie = cookiestr
        return true
    }
    catch( error ) {
        console.error('persistence.js saveParam(): cvalue is document.cookie expception: ', error)
        return false
    }
}
function getParam( cname, cid ) {
    console.log('getParam(): ', cname, cid)
    // let's avoid chasing phantom cookies
    var nCname = cname || null
    if ( (nCname == null) || (nCname == '') ) {
        console.error('persistence.js getParam(): no cname')
        return null
    }
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        console.error('persistence.js getParam(): no cid')
        return null
    }
    var name = cname + '-' + cid + '='
    var ca = document.cookie.split(';')
    for(var i = 0; i < ca.length; i++) {
        var c = ca[i]
        while (c.charAt(0) == ' ') {
            c = c.substring(1)
        }
        if (c.indexOf(name) == 0) {
            retval = c.substring(name.length, c.length)
            console.log('getParam(): ', cname=retval)
            return retval
        }
    }
    console.log('getParam(): ', cname, ' not found.')
    return null
}
function deleteParam( cname, cid ) {
    console.log('deleteParam(): ', cname, cid)
    // deleting phantom cookies would be useless
    var nCname = cname || null
    if ( (nCname == null) || (nCname == '') ) {
        console.error('persistence.js deleteParam(): no cname')
        return false
    }
    var nCid = cid || null
    if ( (nCid == null) || (nCid == '') ) {
        console.error('persistence.js deleteParam(): no cid')
        return false
    }
    var expires = ';expires=Thu, 01 Jan 1970 00:00:00 UTC'
    var cookiestr = cname + '-' + cid + '=' + expires + ';path=/'
    console.log('deleteParam():', cookiestr)
    try {
        document.cookie = cookiestr
        return true
    }
    catch( error ) {
        console.error('persistence.js deleteParam(): cvalue is document.cookie expception: ', error)
        return false
    }
}

function SelfTest( locProtocol ) {

    if ( bSelfTest )
        return true

    var protocol = locProtocol
    
    try {
        var sKey  = 'test.persistence.js'
        var sTest = 'test'
        localStorage.setItem( sKey, sTest )
        var sComp = localStorage.getItem( sKey )
        localStorage.removeItem( sKey )
        if ( sTest === sComp ) {
            bLocalStorage = true
            console.log(
                'localStorage() support is available, protocol: ', protocol)
        }
        else
            console.log(
                'localStorage() support but verification failed, protocol: ',
                protocol)
    }
    catch( error ) {
        console.log(
            'localStorage() support not available, protocol: ', protocol)
    }
    var cNameTest = 'test-cookie-by-persistence-js'
    var cCidTest  = String( Math.floor(Math.random() * Math.floor(999999)) )
    var cValTest  = 'test'
    try {
        if ( saveParam( cNameTest, cCidTest, cValTest ) ) {
            var cValComp = getParam( cNameTest, cCidTest )
            if ( cValComp === cValTest ) {
                console.log(
                    'document.cookie support is available, protocol: ', protocol)
                bCookies = true
                if ( !deleteParam( cNameTest, cCidTest ) )
                    console.log(
                        'document.cookie: warning: cannot delete the test cookie: ',
                        cNameTest, '-', cCidTest)
            }
            else {
                console.log(
                    'document.cookie support not available (cannot read back)',
                    ', protocol: ', protocol)
            }
        }
        else {
            console.log(
                'document.cookie support not available (cannot save), protocol: ',
                protocol)
        }
    }
    catch( error) {
        console.log(
            'document.cookie support not available, protocol: ',
            protocol, ' error: ', error)
    }
    if ( !bLocalStorage && !bCookies ) {
        console.error(
            'persistence.js: no local storage support available,',
            'probably a back-end (IE or WebKit) policy prevents the usage ',
            'for the used protocol: ', protocol)
        if ( locProtocol === 'file:' )
            alert('Saving of settings not available!\n' +
                  'System policy prevents local storage\n' +
                  'for the protocol "file://"\n' +
                  'Provide instrument files by protocol "http://"')
        else
            alert('Saving of settings not available!\n' +
                  'System policy prevents local storage.' +
                  'This is often the case for "file:://" protocol.')
    }
    return
}
