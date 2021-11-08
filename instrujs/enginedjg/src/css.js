/* $Id: css.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import { hasProportionalFontSupport } from '../../src/css'

 // Please refer to local ../sass/style.scss

 export function getNewLuminosity( that ) {
     var newluminosity = window.iface.getluminsty()
     if ( (newluminosity === 'day') ||
          (newluminosity === 'dusk') ||
          (newluminosity === 'night') ) {
         that.luminosity  = newluminosity

         var newclass = 'instrument ' + newluminosity
         var elem = document.getElementById('instrument')
         var oldclass = elem.className
         if ( !(newclass === oldclass) )
             elem.className = newclass

         if ( hasProportionalFontSupport() )
             newclass = 'skpath propl ' + newluminosity
         else
             newclass = 'skpath fixed ' + newluminosity
         elem = document.getElementById('skPath')
         oldclass = elem.className
         if ( !(newclass === oldclass) )
             elem.className = newclass

         var elemnum = document.getElementById('numgauge0')
         if ( elemnum !== null ) {
             oldclass = elemnum.className
             if ( hasProportionalFontSupport() )
                 newclass = 'numgauge propl ' + newluminosity
             else
                 newclass = 'numgauge fixed ' + newluminosity
             if ( !(newclass === oldclass) )
             elemnum.className = newclass
         }
         var elemunit = document.getElementById('numgunit0')
         if ( elemunit !== null ) {
             oldclass = elemunit.className
             if ( hasProportionalFontSupport() )
                 newclass = 'numgunit propl ' + newluminosity
             else
                 newclass = 'numgunit fixed ' + newluminosity
             if ( !(newclass === oldclass) )
             elemunit.className = newclass
         }

         document.getElementById('bottom').className = 'bottom ' + that.luminosity

         // Gauge, sorry no SCSS, requires justgage 1.3.4 or greater
         if ( (that.gauge.length > 0) )  {
             if ( newluminosity === 'day') {
                 that.gauge[0].labelFontColor = '#232b99'
                 that.gauge[0].update('labelFontColor', '#232b99')
                 that.gauge[0].valueFontColor = '#232b99'
                 that.gauge[0].update('valueFontColor', '#232b99')
             }
             else if ( newluminosity === 'dusk') {
                 that.gauge[0].labelFontColor = '#e0e0e4'
                 that.gauge[0].update('labelFontColor', '#e0e0e4')
                 that.gauge[0].valueFontColor = '#232b99'
                 that.gauge[0].update('valueFontColor', '#232b99')
             }
             else if ( newluminosity === 'night') {
                 that.gauge[0].labelFontColor = '#6168c2'
                 that.gauge[0].update('labelFontColor', '#6168c2')
                 that.gauge[0].valueFontColor = '#aaaeeb'
                 that.gauge[0].update('valueFontColor', '#aaaeeb')
             }
         }
     }
     return
 }
