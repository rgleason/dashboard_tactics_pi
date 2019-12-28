/* $Id: util.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */


/* Persistence */

// LocalStorage (JSON objects) - works (usually) with file:// URIs but can be blocked by policy


function saveObj( cid, cobj ) {
    console.log('saveObj(): ', cid, cobj );
    // let's avoid mess
    var nCid = cid || null;
    if ( (nCid == null) || (nCid == '') ) {
        console.error('util.js saveObj(): no cid');
        return false;
    }
    var nCobj = cobj || null;
    if ( nCobj == null ) {
        console.error('util.js saveObj(): cobj is null');
        return false;
    }
    try {
        localStorage.setItem( cid, JSON.stringify( cobj ) );
        return true;
    }
    catch ( error ) {
        console.error('util.js saveObj(): error: ', error );
        return false;
    }
}
function deleteObj( cid ) {
    console.log('deleteObj(): ', cid);
    // deleting phantom data would be useless
    var nCid = cid || null;
    if ( (nCid == null) || (nCid == '') ) {
        console.error('util.js deleteObj(): no cid');
        return false;
    }
    try {
        localStorage.removeItem( cid );
        return true;
    }
    catch (error) {
        console.error('util.js deleteObj(): error: ', error);
        return false;
    }
}
function getObj( cid ) {
    console.log('getObj(): ', cid);
    // let's avoid chasing phantom data
    var nCid = cid || null;
    if ( (nCid == null) || (nCid == '') ) {
        console.error('util.js getObj(): no cid');
        return null;
    }
    try {
       return JSON.parse( localStorage.getItem( cid ) );
    }
    catch (error) {
        console.error('util.js getObj(): error: ', error);
        return null;
    }
}


// Cookies (do not use w/ file:// URIs, only http:// or https::// )

function saveParam( cname, cid, cvalue, inexdays ) {
    console.log('saveParam(): ', cname, cid, cvalue, inexdays);
    // let's avoid creating useless cookies
    var nCname = cname || null;
    if ( (nCname == null) || (nCname == '') ) {
        console.error('util.js saveParam(): no cname');
        return;
    }
    var nCid = cid || null;
    if ( (nCid == null) || (nCid == '') ) {
        console.error('util.js saveParam(): no cid');
        return;
    }
    var nCvalue = cvalue || null;
    if ( nCvalue == null ) {
        console.error('util.js saveParam(): cvalue is null');
        return;
    }
    var d = new Date();
    var expires = '';
    var exdays = inexdays || null;
    if ( exdays == null )
        exdays = 365; // unused will disappear after 1 year
    d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000));
    var expires = ';expires='+d.toUTCString();
    var cookiestr = cname + '-' + cid + '=' + cvalue + expires + ';path=/';
    console.log('saveParam():', cookiestr);
    document.cookie = cookiestr;
}
function deleteParam( cname, cid ) {
    console.log('deleteParam(): ', cname, cid);
    // deleting phantom cookies would be useless
    var nCname = cname || null;
    if ( (nCname == null) || (nCname == '') ) {
        console.error('util.js deleteParam(): no cname');
        return;
    }
    var nCid = cid || null;
    if ( (nCid == null) || (nCid == '') ) {
        console.error('util.js deleteParam(): no cid');
        return;
    }
    var expires = ';expires=Thu, 01 Jan 1970 00:00:00 UTC';
    var cookiestr = cname + '-' + cid + '=' + expires + ';path=/';
    console.log('deleteParam():', cookiestr);
    document.cookie = cookiestr;
}
function getParam( cname, cid ) {
    console.log('getParam(): ', cname, cid);
    // let's avoid chasing phantom cookies
    var nCname = cname || null;
    if ( (nCname == null) || (nCname == '') ) {
        console.error('util.js getParam(): no cname');
        return;
    }
    var nCid = cid || null;
    if ( (nCid == null) || (nCid == '') ) {
        console.error('util.js getParam(): no cid');
        return;
    }
    var name = cname + '-' + cid + '=';
    var ca = document.cookie.split(';');
    for(var i = 0; i < ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            retval = c.substring(name.length, c.length);
            console.log('getParam(): ', cname=retval);
            return retval;
        }
    }
    console.log('getParam(): ', cname, ' not found.');
    return "";
}
