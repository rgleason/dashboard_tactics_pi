/* $Id: siface.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

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
        this.clearFlag()
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
        var testpaths = [
            'environment.wind.angleApparent',
            'environment.wind.speedApparent',
            'navigation.courseOverGroundTrue',
            'navigation.speedOverGround',
            'navigation.position.latitude',
            'navigation.position.longitude',
            'propulsion.port.oilPressure',
            'propulsion.port.revolutions',
            'propulsion.port.temperature',
            'propulsion.starboard.oilPressure',
            'propulsion.starboard.revolutions',
            'propulsion.starboard.temperature',
            'battery.empty',
        ]
        console.log('this.allpaths ', this.allpaths)
        console.log('testpaths ', testpaths)

        if ( (this.eventsetall == null) || (this.elemsetall == null) )
            return
        this.elemsetall.dispatchEvent( this.eventsetall )
    },
    getall: function() {
        if ( this.allpaths.length == 0 )
            return []
        this.clearFlag()
        return this.allpaths
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
        this.clearFlag()
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
        this.clearFlag()
        return this.luminsty
    },
    setFlag: function( elemid, request ) {
        var el = document.getElementById(elemid)
        el.innerHTML = request
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
    clearFlag: function( ) {
        if (window.getSelection) { 
            window.getSelection().removeAllRanges()
        }
        else if (document.selection) {
            document.selection.empty()
        }
    }
}
window.iface = iface

