/* $Id: confstat.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in / WebKit based instruments
 * Licensed under MIT - see distribution.
 */
/* >>>> If you plan to modify this file, please make a backup first! <<<< */


/* Overriding the persistent configuration - useful if file:// does not save
   and http:// cannot be used or you want to absolutely override default values per
   each instrument, otherwise do not define anything here. Read the documentation: */

/* >>>> JavaScript syntax error(s) will prevent instruments to start <<<< */

/* If there is an error, take your browser, hit Shift+Ctrl+I for debugger and load
   the instrument HTLM-page (named index.html). Observe messages in the Console. */

const instrustatconf = {
    getObj : function( instruid ) {
        for ( var i = 0; i < this.instruconf.length; i++  ) {
            if ( instruid === this.instruconf[ i ].uid ) {
                return this.instruconf[ i ].conf()
            }
        }
        return null
    },
    instruconf: [
        {
            uid: "3ba69928-d392-4583-8f9a-a323641063d7",
            conf: function () {
                return {
                    skpath: '',
                    title: '',
                    unit: '',
                    decimals: 1,
                    minval: 0,
                    maxval: 100,
                    theme: '',
                    opt1: '',
                    opt2: ''
                }
            }
        }
    ]
}
window.instrustatconf = instrustatconf
