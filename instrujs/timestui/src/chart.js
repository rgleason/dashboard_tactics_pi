/* $Id: chart.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/

import tui from '../node_modules/tui-chart/dist/tui-chart-polyfill'

var dbglevel = window.instrustat.debuglevel
var alerts = window.instrustat.alerts
var alertdelay = window.instrustat.alertdelay
var container
var rawData
var options

export function startTimesTuiChart() {

    container = document.getElementById('chart0');

    if ( dbglevel > 1 )
        console.log('startTimesTuiChart() - tui: ', tui)

    rawData = {
        categories: ['18:50:10', '18:50:11', '18:50:12'],
        series: [
            {
                name: 'SiteA',
                data: [110, 120, 130]
            },
            {
                name: 'SiteB',
                data: [140, 150, 160]
            }
        ]
    }
    options = {
        series: {
            shifting: true
        } ,
        usageStatistics: false
    }
    var livechart = tui.lineChart(container, rawData, options);

    livechart.on('load', function () {
        livechart.addData('18:50:13', [170, 180])
        // ['18:50:10', '18:50:11', '18:50:12'] ==> ['18:50:11', '18:50:12', '18:50:13']
        // [110, 120, 130] ==> [120, 130, 170]
        // [140, 150, 160] ==> [150, 160, 180]

        var setTimout = (function() {
            livechart.addData('18:50:14', [190, 200])
            // ['18:50:11', '18:50:12', '18:50:13'] ==> ['18:50:12', '18:50:13', '18:50:14']
            // [120, 130, 170] ==> [130, 170, 190]
            // [150, 160, 180] ==> [160, 180, 200]
        }, 1000)
    });
}
