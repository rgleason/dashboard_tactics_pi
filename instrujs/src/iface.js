/* $Id: siface.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

var iface = {
    eventsetid   : null,
    elemsetid    : null,
    uid          : '',
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
            return
        this.clearFlag()
        return this.uid
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
window.iface = iface;

