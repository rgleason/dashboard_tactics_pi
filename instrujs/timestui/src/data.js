/* $Id: data.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to ask and retrieve from a client a unique ID for this instance

import sanitizer from '../../src/escapeHTML'
var Sanitizer = sanitizer()

import { rollDisplayToSelection } from './disp'
import { getPathDefaultsIfNew } from '../../src/conf'
import { getRetrieveSeconds, setIdbClientForRetry, getCollectedDataJSON } from './idbclient'
import { showDataTimesTuiChart } from './chart'

var dbglevel = window.instrustat.debuglevel
var alerts = window.instrustat.alerts
var alertdelay = window.instrustat.alertdelay

var alertcondition = false
var alertcounter = 0
var alertthreshold = alertdelay
var suppressShowData = false

var nofGraphPoints = 20 // initData(): max=3600 (anyway, too much!)
var nofValDecimals = 1

var dbData = []
var firstRetrieval = true
var sampleFrequency = 0

export var chartData = {
    categories: [],
    series: [
        {
            name: 'Loading',
            data: []
        }
    ]
}
var lastDataTimestamp = {
    stmp: 0,
    idx: 0
}

function strMinSecSinceEpoch( cntSec ) {
    let d = new Date( 0 )
    let msSinceBigBang = d.setUTCSeconds( cntSec )
    let mSinceBigBang = d.getMinutes()
    let sSinceBigBang = d.getSeconds()
    var leadZeroStr = function (num) {
       var s = "0" + num;
       return s.substr(s.length-2);
    }
    let mStr = leadZeroStr(mSinceBigBang)
    let sStr = leadZeroStr(sSinceBigBang)
    let fStr = '00:' + mStr + ':' + sStr
    return fStr
}

export function initData( that ) {

    // X-axis (category) with '00:MM:SS' time since EPOCH
    for ( let i = 0; i <  nofGraphPoints; i++ ) {
        let fStr = strMinSecSinceEpoch( i )
        chartData.categories.push( fStr )
        chartData.series[0].data.push( 0 )
    }
}

export function onWaitdataFinalCheck( that ) {
    var elem = document.getElementById('skPath')
    var htmlObj = null
    var htmlCandidate = null
    var getSetDefTitle = (function () {
        getPathDefaultsIfNew( that )
        if ( (that.conf.title !== null) && (that.conf.title !== '' ) )
            htmlCandidate = that.conf.title
        else
            htmlCandidate = that.conf.path
    })
    if ( that.conf !== null ) {
        if ( (that.conf.title !== null) && (that.conf.title !== '' ) )
            htmlCandidate = that.conf.title
        else if ( (that.conf.path !== null) && (that.conf.path !== '' ) )
            getSetDefTitle()
    }
    else if ( (that.path !== null) && (that.path !== '' ) )
        getSetDefTitle()
    else if ( dbglevel > 1 )
        console.error('onWaitdataFinalCheck(): no path, no conf!')

    if ( htmlCandidate !== null ) {
        htmlObj = Sanitizer.createSafeHTML(htmlCandidate)
        elem.innerHTML = Sanitizer.unwrapSafeHTML(htmlObj)
    }
}

export function waitData( that ) {
    return
}

export function showData( that ) {
    if ( dbglevel > 0 )
        console.log('dataQuery()')

    if ( suppressShowData )
        return
    alert( 'showData()' )
    /*
     Retrieve the influxdb-client collected data, an array of
     stringified JSON objects, cf. idbclient.ts
     */
    let dbJsonStrArr = getCollectedDataJSON()
    chartData.categories.length = 0
    chartData.series[0].data.length = 0 // https://codepen.io/petrim/pen/yLyRQJK
    if ( !(dbJsonStrArr.length) ) {
        if ( dbglevel > 0 ) {
            console.error( 'showData(): no data received' )
            if ( (dbglevel > 1) && alerts )
                alert ( 'showData(): '+
                        window.instrulang.noDataFromDbQry1 + '\n' +
                        window.instrulang.noDataFromDbQry2 + '\n' +
                        window.instrulang.noDataFromDbQry3 + '\n' )
        }
        return
    }
    for ( let i in dbJsonStrArr ) {
        let iobj = JSON.parse( dbJsonStrArr[i] )
        dbData.push( iobj )
    }
    // The members of the object may vary according the DB DbSchema
    // but we must have at least the time and value fields
    if ( dbglevel > 0 ) {
        if ( !('_time' in dbData[0]) ) {
            console.error( 'showData(): no _time field in dbData[0]' )
            if ( (dbglevel > 1) && alerts )
                alert ( 'showData(): '+ window.instrulang.dataFromDbNoTime )
            return
        }
        if ( !('_value' in dbData[0]) ) {
            console.error( 'showData(): no _value field in dbData[0]' )
            if ( (dbglevel > 1) && alerts )
                alert ( 'showData(): '+ window.instrulang.dataFromDbNoValue )
            return
        }
    }
    if ( firstRetrieval ) {
        firstRetrieval = false
        let nofSamples = dbData.length
        let startStamp = Date.parse( dbData[0]._time )
        let endStamp   = Date.parse( dbData[(nofSamples - 1)]._time )
        let sampleFrequency = ( nofSamples / (endStamp -startStamp) * 1000 )
        if ( dbglevel > 3 )
            console.log ( 'showData(): sampleFrequency: ', sampleFrequency,
                          'nofSamples: ', nofSamples,
                          'getRetrieveSeconds(): ', getRetrieveSeconds(),
                          'startStamp: ', startStamp,
                          'endStamp: ', endStamp )
        if ( (dbglevel > 4) && alerts )
            alert ( 'sampleFrequency: ' + sampleFrequency + '\n' +
                    'nofSamples: ' + nofSamples + '\n' +
                    'getRetrieveSeconds(): ' + getRetrieveSeconds() + '\n' +
                    'startStamp: ' + startStamp + '\n' +
                    'endStamp: ' + endStamp )
    }
    // We will show only max. number of points, reject the excess history
    // or make undersampling if the sampling frequency is too high
    lastDataTimestamp.idx = -1
    for ( let i = 0; ( (i < nofGraphPoints) && (i < dbData.length) ); i++ ){
        var getSeriesValue = (function (iobj) {
            // The timestamp format is https://tools.ietf.org/html/rfc3339 (5.8)
            // Date.parse() converts it OK to milliseconds
            var retname = strMinSecSinceEpoch( i )
            var retvalue = 0
            var retstamp = i * 1000
            let s1 = iobj._time.split('T')
            if ( s1.length < 2 ) {
                if ( dbglevel > 0 )
                    console.error (
                        'showData(): unknown _time field in dbData[', i,
                        ']: ', iobj._time )
                if ( (dbglevel > 1) && alerts )
                    alert ( 'showData(): '+ window.instrulang.dataFromDbBadTime
                            + '\n' + iobj._time)
            } // then an issue, not a RFC339 or even ISO-8601 <date>T<time>
            else {
                let s2 = s1[1].split('Z')
                let s3 = s2[0].split('.') // HH:MM:SS.ms
                retname = s3[0]           // HH:MM:SS
                retstamp = Date.parse( iobj._time )
            }
            if ( isNaN(iobj._value) ) {
                if ( dbglevel > 0 )
                    console.error (
                        'showData(): unknown _value field in dbData[', i,
                        ']: ', iobj._value )
                if ( (dbglevel > 1) && alerts )
                    alert ( 'showData(): '+ window.instrulang.dataFromDbBadValue
                            + '\n' + iobj._value)
            } // then no data value
            else {
                retvalue = iobj._value.toFixed( nofValDecimals )
            }
            return {
                name: retname,
                data: retvalue,
                stmp: retstamp
            }
        }) // function seriesData()
        var seriesData = getSeriesValue( dbData[i] )
        if ( dbglevel > 4 )
            console.log ( 'showData(): seriesData: ', seriesData)
        if ( (dbglevel > 5) && alerts )
            alert ( 'showData():'+ '\n' +
                    'seriesData.name ' + seriesData.name + '\n' +
                    'seriesData.data ' + seriesData.data + '\n' +
                    'seriesData.stmp ' + seriesData.stmp )
        if ( seriesData.stmp > lastDataTimestamp.stmp ) {
            lastDataTimestamp.stmp = seriesData.stmp
            lastDataTimestamp.idx = i
        }
        chartData.categories.push( seriesData.name )
        chartData.series[0].data.push( seriesData.data )
    } // for received data from start to max. points
    chartData.series[0].name = that.path

    showDataTimesTuiChart( ( lastDataTimestamp.idx + 1) )

}

export function clearData( that ) {

    // that.glastvalue = 0
    // if ( that.gauge.length > 0 ) {
    //     that.gauge[0].symbol = ''
    //     that.gauge[0].refresh( 0, 100, 0, '' )
    // }
    // else {
    //     document.getElementById('numgauge0').innerHTML = '&nbsp;'
    //     document.getElementById('numgunit0').innerHTML = '&nbsp;'
    // }
    // suppressShowData = true // otherwise the rolling dial will check for value
    // rollDisplayToSelection( that )
    // suppressShowData = false

}

export function noData( that ) {
    setIdbClientForRetry()
    window.iface.setretyget()
}

export function prepareDataHalt( that ) {
    clearData( that )
}
