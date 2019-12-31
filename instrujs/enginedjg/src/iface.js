/* $Id: siface.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

var mylib1 = {
    event   : null,
    elem    : null,
    multiply: function (x, y) {
        return x * y
    },
    regevent: function ( newelem, newevent ) {
        this.elem = newelem
        this.event = newevent
    },
    trigger: function() {
        if ( (this.event == null) || (this.elem == null) )
            return
        this.elem.dispatchEvent( this.event )
    }
}
window.mylib1 = mylib1;

