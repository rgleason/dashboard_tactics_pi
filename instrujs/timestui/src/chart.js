/* $Id: chart.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/

import sanitizer from '../../src/escapeHTML'
var Sanitizer = sanitizer()

import tui from '../node_modules/tui-chart/dist/tui-chart-polyfill'

import { chartData } from './data'
import { hasProportionalFontSupport } from '../../src/css'

var dbglevel = window.instrustat.debuglevel
var alerts = window.instrustat.alerts
var alertdelay = window.instrustat.alertdelay
var container
var chart
var rawData
var options
var theme

export function startTimesTuiChart() {

    container = document.getElementById('chart0')

    // Data shifting logic - see ToastUI's example https://git.io/Jvh92

    if ( dbglevel > 1 )
        console.log('startTimesTuiChart() - tui: ', tui)

    theme = {
        chart : {
            background: 'white',
            opacity: 0
        }
    }

    tui.registerTheme('transpTheme', theme)

    options = {
        theme: 'transpTheme',
        chart: {
            width: 480,
            height: 400
        },
        series: {
            spline: true,
            showDot: true,
            shifting: true,
            zoomable:  false
        },
        xAxis: {
            title: '[s]',
            labelInterval: 3,
            tickInterval: 'auto'
        },
        tooltip: {
            grouped: true,
            align: 'center top',
            template: function(category, items, categoryTimestamp) {
                if ( dbglevel > 2 )
                    console.log('tooltip: category ', category, ' items: ', items)
                var htmlCandidate = '<div id="cTpVal" class="numchart ' +
                    (hasProportionalFontSupport()?'propl':'fixed') + ' ' +
                    window.iface.getluminsty() + '">' +
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
    chart = tui.lineChart(container, chartData, options)
    if ( dbglevel > 1 )
        console.log('startTimesTuiChart() - chart: ', chart)

}

export function showDataTimesTuiChart( nofNewValues ) {
    if ( dbglevel > 4 )
        console.log (
            'showDataTimesTuiChart(): nofNewValues: ', nofNewValues,
            'chart._dynamicDataHelper.addedDataCount: ',
            chart._dynamicDataHelper.addedDataCount )
    var nVal = nofNewValues || null
    if ( (nVal === null) || (nVal == 0) )
        return
    for ( let i = 0; i < nVal; i++ ) {
        chart.addData( chartData.categories[i], [chartData.series[0].data[i]] )
    }
    if ( dbglevel > 4 )
        console.log (
            'showDataTimesTuiChart(): return - ',
            'chart._dynamicDataHelper.addedDataCount: ',
            chart._dynamicDataHelper.addedDataCount )
}
/*
 The issue with the ToastTUI's chart.addData() method here is that is
 asynchronous (actually good, allowing us to do something else).
 But if we continue to fetch even more data, soon there will be buffer
 overrun. It may take several seconds to draw a completely new set of data.
 Database operations are, on the other hand in <100ms range.
 We should be able to wait on the end of the data plot drawing.
 We use the undocumented variable from its _dynamicDataHelper object to
 estimate if it is still busy.
 */
 export function getAddedDataCount() {
     return chart._dynamicDataHelper.addedDataCount
}
