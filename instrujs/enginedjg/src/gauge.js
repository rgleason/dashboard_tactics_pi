/* $Id: gauge.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
import JustGage from 'justgage'
/* Note: >1.3.3 deeded in css.js (day / night switch ), update() method:
         see package.json. If still 1.3.3 and no higher available, overrides
         with file ./justgauge133fix241updateie.js before build.
   Watchout: there was a 'const' in the update-method, strange characters in
         a console log also - the WebView backend, IE on Windows does not like!  */

export function createGauge(name, value, decimals, label) {
    return new JustGage({
        id: name,
        value: value,
        decimals: decimals,
        label: label,
        min: 0,
        max: 100,
        pointer: true,
        pointerOptions: {
            toplength: -15,
            bottomlength: 10,
            bottomwidth: 12,
            color: '#8e8e93',
            stroke: '#ffffff',
            stroke_width: 3,
            stroke_linecap: 'round'
        },
        gaugeColor      : '#cdcbcb',
        labelFontColor  : '#262626',
        valueFontColor  : '#101566',
        valueFontFamily : "Courier",
        relativeGaugeSize: true
    })
}
