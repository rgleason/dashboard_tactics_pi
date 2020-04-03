/* $Id: siface.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
var ifacedbglevel = window.instrustat.debuglevel

// import sanitizer code port
var escapeHTML = require('./escapeHTML')
var Sanitizer = escapeHTML.default()

var iface = {
    eventsetid    : null,
    elemsetid     : null,
    uid           : '',
    regeventsetid: function ( newelem, newevent ) {
        this.elemsetid = newelem
        this.eventsetid = newevent
    },
    setid: function( newuid ) {
        this.uid = newuid
        if ( (this.eventsetid === null) || (this.elemsetid === null) )
            return
        this.elemsetid.dispatchEvent( this.eventsetid )
    },
    getid: function() {
        if ( (this.uid === null) || (this.uid === '') )
            return ''
        this.clearFlag( this.elemsetid )
        return this.uid
    },
    eventsetall    : null,
    elemsetall     : null,
    allpaths       : [],
    regeventsetall: function ( newelem, newevent ) {
        this.elemsetall = newelem
        this.eventsetall = newevent
    },
    setall: function( alist ) {
        var emptylist = []
        this.allpaths = emptylist
        var varlistitems = alist.split(',')
        for ( var i = 0; i < varlistitems.length; i++ ) {
            this.allpaths.push( varlistitems[ parseInt(i) ] )
        }
        if ( (this.eventsetall === null) || (this.elemsetall === null) )
            return
        this.elemsetall.dispatchEvent( this.eventsetall )
    },
    getall: function() {
        if ( this.allpaths.length === 0 )
            return []
        this.clearFlag( this.elemsetall )
        return this.allpaths
    },
    eventsetalldb : null,
    elemsetalldb  : null,
    allpathsdb    : [],
    regeventsetalldb: function ( newelem, newevent ) {
        this.elemsetalldb = newelem
        this.eventsetalldb = newevent
    },
    setalldb: function( alist ) {
        var emptylist = []
        this.allpathsdb = emptylist
        var varlistitems = alist.split(',')
        for ( var i = 0; i < varlistitems.length; i++ ) {
            this.allpathsdb.push( varlistitems[ parseInt(i) ] )
        }
        if ( (this.eventsetalldb === null) || (this.elemsetalldb === null) )
            return
        this.elemsetalldb.dispatchEvent( this.eventsetalldb )
    },
    getalldb: function() {
        if ( this.allpathsdb.length === 0 )
            return []
        this.clearFlag( this.elemsetalldb )
        return this.allpathsdb
    },
    eventselected    : null,
    elemselected     : null,
    selectedpath     : '',
    regeventselected: function ( newelem, newevent ) {
        this.elemselected = newelem
        this.eventselected = newevent
    },
    setselected: function( newpath ) {
        try {
            if ( (this.eventselected === null) || (this.elemselected === null) )
                return
            this.selectedpath = newpath
            if ( ifacedbglevel > 0 )
                console.log('iface.setselected - selectedpath: ', this.selectedpath)
            this.elemselected.dispatchEvent( this.eventselected )
        }
        catch (error) {
            this.selectedpath = ''
            if ( ifacedbglevel > 1 )
                console.log('iface.setselected - state machine error',
                            error)
            return
        }
    },
    getselected: function() {
        if ( (this.selectedpath === null) || (this.selectedpath === '') )
            return ''
        return this.selectedpath
    },
    eventrescan    : null,
    elemrescan     : null,
    regeventrescan: function ( newelem, newevent ) {
        this.elemrescan = newelem
        this.eventrescan = newevent
    },
    setrescan: function() {
        try {
            if ( (this.eventrescan === null) || (this.elemrescan === null) )
                return
            this.elemrescan.dispatchEvent( this.eventrescan )
        }
        catch (error) {
            if ( ifacedbglevel > 1 )
                console.log('iface.setrescan - state machine error',
                            error)
            return
        }
    },
    eventacksubs    : null,
    elemacksubs     : null,
    acksubspath     : '',
    regeventacksubs: function ( newelem, newevent ) {
        this.elemacksubs = newelem
        this.eventacksubs = newevent
    },
    acksubs: function( forpath ) {
        try {
            if ( (this.eventacksubs === null) || (this.elemacksubs === null) )
                return
            this.acksubspath = forpath
            if ( ifacedbglevel > 0 )
                console.log('iface.setacksubs - acksubspath: ', this.acksubspath)
            this.elemacksubs.dispatchEvent( this.eventacksubs )
        }
        catch (error) {
            this.acksubspath = ''
            if ( ifacedbglevel > 1 )
                console.log('iface.setacksubs - state machine error',
                            error)
            return
        }
    },
    getacksubs: function() {
        if ( (this.acksubspath === null) || (this.acksubspath === '') )
            return ''
        return this.acksubspath
    },
    eventackschema    : null,
    elemackschema     : null,
    dbschema          : '',
    regeventackschema: function ( newelem, newevent ) {
        this.elemackschema = newelem
        this.eventackschema = newevent
    },
    ackschema: function( newschema ) {
        try {
            if ( (this.eventackschema === null) || (this.elemackschema === null) )
                return
            this.dbschema = newschema
            if ( ifacedbglevel > 0 )
                console.log('iface.setackschema - ackschema: ', this.dbschema)
            this.elemackschema.dispatchEvent( this.eventackschema )
        }
        catch (error) {
            this.dbschema = ''
            if ( ifacedbglevel > 1 )
                console.log('iface.setackschema - state machine error',
                            error)
            return
        }
    },
    getdbschema: function() {
        if ( (this.dbschema === null) || (this.dbschema === '') )
            return ''
        return this.dbschema
    },
    eventchgconf    : null,
    elemchgconf     : null,
    chgconfpath     : '',
    regeventchgconf: function ( newelem, newevent ) {
        this.elemchgconf = newelem
        this.eventchgconf = newevent
    },
    setchgconf: function( newpath ) {
        try {
            if ( (this.eventchgconf === null) || (this.elemchgconf === null) )
                return
            this.chgconfpath = newpath
            if ( ifacedbglevel > 0 )
                console.log('iface.setchgconf - chgconfpath: ', this.chgconfpath)
            this.elemchgconf.dispatchEvent( this.eventchgconf )
        }
        catch (error) {
            this.chgconfpath = ''
            if ( ifacedbglevel > 1 )
                console.log('iface.setchgconf - state machine error',
                            error)
            return
        }
    },
    getchgconf: function() {
        if ( (this.chgconfpath === null) || (this.chgconfpath === '') )
            return ''
        return this.chgconfpath
    },
    eventnewdata    : null,
    elemnewdata     : null,
    value           : 0.0,
    regeventnewdata: function ( newelem, newevent ) {
        this.elemnewdata = newelem
        this.eventnewdata = newevent
    },
    newdata: function( newvalue ) {
        this.value = newvalue
        if ( (this.eventnewdata === null) || (this.elemnewdata === null) )
            return
        this.elemnewdata.dispatchEvent( this.eventnewdata )
    },
    getdata: function() {
        if ( this.value === null )
            return 0.0
        return this.value
    },
    eventswapdisp    : null,
    elemswapdisp     : null,
    swapdirection    : 0,
    regeventswapdisp: function ( newelem, newevent ) {
        this.elemswapdisp = newelem
        this.eventswapdisp = newevent
    },
    setswapdisp: function( newswapdirection ) {
        try {
            if ( (this.eventswapdisp === null) || (this.elemswapdisp === null) )
                return
            this.swapdirection = newswapdirection
            if ( ifacedbglevel > 0 )
                console.log(
                    'iface.setswapdisp - swapdirection: ', this.swapdirection)
            this.elemswapdisp.dispatchEvent( this.eventswapdisp )
        }
        catch (error) {
            this.swapdirection = 0
            if ( ifacedbglevel > 1 )
                console.log('iface.setswapdisp - state machine error',
                            error)
            return
        }
    },
    getswapdisp: function() {
        return this.swapdirection
    },
    eventluminsty : null,
    elemluminsty  : null,
    luminsty      : '',
    regeventluminsty: function ( newelem, newevent ) {
        this.elemluminsty = newelem
        this.eventluminsty = newevent
    },
    setluminsty: function( newluminsty ) {
        this.luminsty = newluminsty
        if ( (this.eventluminsty === null) || (this.elemluminsty === null) )
            return
        this.elemluminsty.dispatchEvent( this.eventluminsty )
    },
    getluminsty: function() {
        if ( (this.luminsty === null) || (this.luminsty === '') )
            return ''
        return this.luminsty
    },
    eventclosing    : null,
    elemclosing     : null,
    regeventclosing: function ( newelem, newevent ) {
        this.elemclosing = newelem
        this.eventclosing = newevent
    },
    setclosing: function() {
        try {
            if ( (this.eventclosing === null) || (this.elemclosing === null) )
                return
            if ( ifacedbglevel > 0 )
                console.log('iface.setclosing - closingpath: ', this.closingpath)
            this.elemclosing.dispatchEvent( this.eventclosing )
        }
        catch (error) {
            if ( ifacedbglevel > 1 )
                console.log('iface.setclosing - state machine error',
                            error)
            return
        }
    },
    graphwizdot      : '',
    setgraphwizdot: function( newgraphwizdot ) {
        this.graphwizdot = newgraphwizdot
    },
    getgraphwizdot: function() {
        if ( (this.graphwizdot === null) || (this.graphwizdot === '') )
            return 'not.available'
        return this.graphwizdot
    },
    setFlag: function( elemid, request ) {
        if ( ifacedbglevel > 1 ) console.log(
            'setFlag() elemid: ', elemid, ' request: ', request)
        var el = document.getElementById(elemid)
        var htmlCandidate = 'instrujs:' + request + '!'
        var htmlObj = Sanitizer.createSafeHTML(htmlCandidate)
        el.innerHTML = Sanitizer.unwrapSafeHTML(htmlObj)
        var doc = window.document
        var sel
        var range
        if (window.getSelection && doc.createRange) {
            sel = window.getSelection()
            range = doc.createRange()
            range.selectNodeContents(el)
            sel.removeAllRanges()
            sel.addRange(range)
        }
        else if (doc.body.createTextRange) {
            range = doc.body.createTextRange()
            range.moveToElementText(el)
            range.select()
        }
    },
    clearFlag: function( elemid ) {
        if ( ifacedbglevel > 1 ) console.log(
            'clearFlag(): elemid content before clean: ', elemid.innerHTML)
        if (window.getSelection) {
            window.getSelection().removeAllRanges()
        }
        else if (document.selection) {
            document.selection.empty()
        }
        elemid.innerHTML = ''
    },
    clearFlagById: function( id ) {
        var el = document.getElementById(id)
        this.clearFlag( el )
    }
}
window.iface = iface
