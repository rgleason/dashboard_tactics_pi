/* $Id: siface.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
var ifacedbglevel = window.instrustat.debuglevel

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
        if ( (this.eventsetid == null) || (this.elemsetid == null) )
            return
        this.elemsetid.dispatchEvent( this.eventsetid )
    },
    getid: function() {
        if ( (this.uid == null) || (this.uid === '') )
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
            this.allpaths.push( varlistitems[ i ] )
        }
        if ( (this.eventsetall == null) || (this.elemsetall == null) )
            return
        this.elemsetall.dispatchEvent( this.eventsetall )
    },
    getall: function() {
        if ( this.allpaths.length == 0 )
            return []
        this.clearFlag( this.elemsetall )
        return this.allpaths
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
            if ( (this.eventselected == null) || (this.elemselected == null) )
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
        if ( (this.selectedpath == null) || (this.selectedpath === '') )
            return ''
        return this.selectedpath
    },
    acksubs: function( ) {
        if ( (this.eventselected != null) && (this.elemselected != null) )
            this.clearFlag( this.elemselected )
        return
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
        if ( (this.eventnewdata == null) || (this.elemnewdata == null) )
            return
        this.elemnewdata.dispatchEvent( this.eventnewdata )
    },
    getdata: function() {
        if ( this.value == null )
            return 0.0
        return this.value
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
        if ( (this.eventluminsty == null) || (this.elemluminsty == null) )
            return
        this.elemluminsty.dispatchEvent( this.eventluminsty )
    },
    getluminsty: function() {
        if ( (this.luminsty == null) || (this.luminsty === '') )
            return
        return this.luminsty
    },
    setFlag: function( elemid, request ) {
        if ( ifacedbglevel > 1 ) console.log(
            'setFlag() elemid: ', elemid, ' request: ', request)
        var el = document.getElementById(elemid)
        el.innerHTML = 'instrujs:' + request + '!'
        var doc = window.document, sel, range;
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
    clearFlag: function( elemid  ) {
        if ( ifacedbglevel > 1 ) console.log(
            'clearFlag(): elemid content: ', elemid.innerHTML)
        if (window.getSelection) { 
            window.getSelection().removeAllRanges()
        }
        else if (document.selection) {
            document.selection.empty()
        }
        elemid.innerHTML = ''
    }
}
window.iface = iface

