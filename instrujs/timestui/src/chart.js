/* $Id: chart.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/

import sanitizer from '../../src/escapeHTML'
var Sanitizer = sanitizer()

import tui from '../node_modules/tui-chart/dist/tui-chart-polyfill'

import { chartData } from './data'

var dbglevel = window.instrustat.debuglevel
var alerts = window.instrustat.alerts
var alertdelay = window.instrustat.alertdelay
var container
var chart
var rawData
var options

export function startTimesTuiChart() {

    container = document.getElementById('chart0');

    // Data shifting logic - see ToastUI's example https://git.io/Jvh92

    if ( dbglevel > 1 )
        console.log('startTimesTuiChart() - tui: ', tui)

    options = {
        series: {
            spline: true,
            showDot: true,
            shifting: true
        },
        xAxis: {
            title: '[s]',
            labelInterval: 3,
            tickInterval: 'auto'
        },
        yAxis: {
            // min: 0,
            // max: 100,
            // title: 'users'
        },
        tooltip: {
            grouped: true,
            template: function(category, items, categoryTimestamp) {
                if ( dbglevel > 2 )
                    console.log('tooltip: category ', category, ' items: ', items)
                var htmlCandidate = '<div id="cTpVal" class="numchart fixed day">' +
                    category + ' : ' + items[0].value + '</div>'
                var htmlObj = Sanitizer.createSafeHTML(htmlCandidate)
                return Sanitizer.unwrapSafeHTML(htmlObj)
            }
        },
        legend: {
            visible: false
        },
        chartExportMenu: {
            visible: false
        },
        usageStatistics: false
    }
    // var chart = tui.lineChart(container, data, options);
    chart = tui.lineChart(container, chartData, options);
    if ( dbglevel > 1 )
        console.log('startTimesTuiChart() - chart: ', chart)

}

export function showDataTimesTuiChart( nofNewValues ) {
    var nVal = nofNewValues || null
    if ( (nVal === null) || (nVal == 0) )
        return
    for ( let i = 0; i < nVal; i++ ) {
        chart.addData( chartData.categories[i], [chartData.series[0].data[i]] )
    }
}
