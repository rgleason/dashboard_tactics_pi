/* $Id: chart.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/

import sanitizer from '../../src/escapeHTML'
var Sanitizer = sanitizer()

import tui from '../node_modules/tui-chart/dist/tui-chart-polyfill'

var dbglevel = window.instrustat.debuglevel
var alerts = window.instrustat.alerts
var alertdelay = window.instrustat.alertdelay
var container
var rawData
var options

export function startTimesTuiChart() {

    container = document.getElementById('chart0');

    // Data shifting logic - see ToastUI's example https://git.io/Jvh92

    if ( dbglevel > 1 )
        console.log('startTimesTuiChart() - tui: ', tui)

    function getRandom(start, end) {
        return start + (Math.floor(Math.random() * (end - start + 1)));
    }

    function zeroFill(number) {
        var filledNumber;

        if (number < 10) {
            filledNumber = '0' + number;
        } else {
            filledNumber = number;
        }

        return filledNumber;
    }
    function adjustTime(time, addTime) {
        addTime = addTime || 60;
        if (time < 0) {
            time += addTime;
        }
        return time;
    }

    function makeDate(hour, minute, second) {
        return zeroFill(adjustTime(hour, 24)) + ':' + zeroFill(adjustTime(minute)) + ':' + zeroFill(adjustTime(second));
    }

    function range(start, stop, step) {
        var arr = [];
        var flag;

        if (typeof stop === 'undefined') {
            stop = start || 0;
            start = 0;
        }

        step = step || 1;
        flag = step < 0 ? -1 : 1;
        stop *= flag;

        for (; start * flag < stop; start += step) {
            arr.push(start);
        }

        return arr;
    }
    var legends = ['n/a', 'n/a2'];
    var seriesData = range(1).map(function (value, index) {
        var name = legends[index]
        var data = range(20).map(function () {
            return getRandom(150, 200)
        });
        return {
            name: name,
            data: data
        };
    });
    var baseNow = new Date()
    var startSecond = baseNow.getSeconds() - seriesData[0].data.length - 1
    var categories = seriesData[0].data.map(function (value, index) {
        var hour = baseNow.getHours()
        var minute = baseNow.getMinutes()
        var second = startSecond + index
        if (second < 0) {
            minute -= 1
        }
        if (minute < 0) {
            hour -= 1
        }
        return makeDate(hour, minute, (startSecond + index));
    });

    var data = {
        categories: categories,
        series: seriesData
    };

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
            title: 'users'
        },
        tooltip: {
            grouped: true,
            template: function(category, items, categoryTimestamp) {
                if ( dbglevel > 2 )
                    console.log('tooltip: category ', category, ' items: ', items)
                var htmlCandidate = '<div id="cTpVal" class="numchart fixed day">' +
                    category + ' : ' + items[0].value + '</div>'
                // var head = '<div>' + category + '</div>'
                // var body = '<div">' + items.value + ':' + items.legend + '</div>'
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
    var chart = tui.lineChart(container, data, options);
    if ( dbglevel > 1 )
        console.log('startTimesTuiChart() - chart: ', chart)

    chart.on('load', function () {
        if ( dbglevel > 2 )
            console.log('startTimesTuiChart() - load()')
        var index = categories.length
        setInterval(function () {
            console.log('startTimesTuiChart() - setInterval()')
            var now = new Date()
            var category = makeDate(now.getHours(), now.getMinutes(), now.getSeconds())
            var values = [getRandom(150, 200), getRandom(150, 200)]

            chart.addData(category, values)
            index += 1
        }, 5000);
    });

}
