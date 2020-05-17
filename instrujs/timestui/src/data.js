/* $Id: data.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to ask and retrieve from a client a unique ID for this instance

import sanitizer from '../../src/escapeHTML'
var Sanitizer = sanitizer()

import { rollDisplayToSelection } from './disp'
import { getPathDefaultsIfNew } from '../../src/conf'
import { getRetrieveSeconds, setRetrieveSeconds, setIdbClientForRetry, getCollectedDataJSON } from './idbclient'
import { showDataTimesTuiChart, getSecondsPerPointDraw } from './chart'

var dbglevel = window.instrustat.debuglevel
var alerts = window.instrustat.alerts
var alertdelay = window.instrustat.alertdelay

var alertcondition = false
var alertcounter = 0
var alertthreshold = alertdelay
var suppressShowData = false

var nofGraphPoints = 20 // initData(): max=3600 (anyway, too much!)
var nofValDecimals = 1
var htmlTitle = ''
var htmlTitleValFunc = ''
var htmlTitleValueAsStr = ''
var htmlTitleSymbol = ''
var htmlTitleUnit = ''

var dbData = []
var nofEmptyResults = 0
var limitOfEmptyResults = 10
var nofFrequencyAnalysis = 0
var limitOfFrequencyAnalysis = 20
var contingencyFrequencyAdjustmentSeconds = 10
var sumOfFrequencies = 0
var frequencyAnalysisDone = false
var frequencyStats = {
    lowest : 9999.99,
    avg: 0,
    highest: 0
}
var sumOfOverlapIdx = 0
var overlapStats = {
    lowest : 999999,
    avg: 0,
    highest: 0
}

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
       var s = '0' + num
       return s.substr(s.length-2)
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
        if ( (that.conf.title !== null) && (that.conf.title !== '' ) ) {
            htmlCandidate = that.conf.title
            htmlTitle = htmlCandidate
            if ( (that.conf.dbfunc !== null) && (that.conf.dbfunc !== '') )
                htmlTitleValFunc = window.instrulang.dataFunctionAbbrv
            if ( that.conf.symbol !== null )
                htmlTitleSymbol = that.conf.symbol
            if ( that.conf.unit !== null )
                htmlTitleUnit = that.conf.unit
            if ( that.conf.decimals !== null )
                nofValDecimals = that.conf.decimals
        }
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

function updateTitleWithValue( valueStr ) {
    if (valueStr === null)
        return
    var elemTitle = document.getElementById('skPath')
    var htmlBuild = ''
    var htmlObject = null
    var htmlValue = valueStr
    htmlBuild += htmlTitle + ' ' +
        ((htmlTitleValFunc==='')?'':(htmlTitleValFunc + ' ')) +
        htmlValue + ' ' +
        ((htmlTitleSymbol==='')?'':(htmlTitleSymbol + ' ')) +
        htmlTitleUnit
    if ( htmlBuild !== '' ) {
        htmlObject = Sanitizer.createSafeHTML(htmlBuild)
        elemTitle.innerHTML = Sanitizer.unwrapSafeHTML(htmlObject)
    }
}

/*eslint complexity: ['error', { "max": 35 }]*/
export function showData( that ) {
    if ( dbglevel > 0 )
        console.log('showData()')

    if ( suppressShowData )
        return
    if ( (dbglevel > 5) && alerts )
        alert( 'showData()' )
    /*
     Retrieve the influxdb-client collected data, an array of
     stringified JSON objects, cf. idbclient.ts
     */
    let dbJsonStrArr = getCollectedDataJSON()
    let emptyArray = []
    chartData.categories = emptyArray
    chartData.series[0].data.length = 0 // https://codepen.io/petrim/pen/yLyRQJK
    if ( !(dbJsonStrArr.length) ) {
        nofEmptyResults++
        if ( nofEmptyResults >= limitOfEmptyResults) {
            if ( dbglevel > 0 ) {
                console.error( 'showData(): no data received' )
                if ( (dbglevel > 1) && alerts )
                    alert ( 'showData(): '+
                            window.instrulang.noDataFromDbQry1 + '\n' +
                            window.instrulang.noDataFromDbQry2 + '\n' +
                            window.instrulang.noDataFromDbQry3 + '\n' )
            }
        }
        return
    }
    else
        nofEmptyResults = 0

    // Oldest timestamp idx=0 which is good for timeseries graph, keep
    for ( let i in dbJsonStrArr ) {
        if ( dbJsonStrArr[parseInt(i)].length > 0 ) {
            let iobj = JSON.parse( dbJsonStrArr[parseInt(i)] )
            dbData.push( iobj )
        }
    }
    // The members of the object may vary according the DB DbSchema
    // but we must have at least the time and value fields
    if ( dbglevel > 0 ) {
        for ( let i = 0; (i < dbData.length); i++ ){
            if ( !('_time' in dbData[parseInt(i)]) ) {
                console.error( 'showData(): no _time field in dbData[',i,']' )
                if ( (dbglevel > 1) && alerts )
                    alert ( 'showData(): '+ window.instrulang.dataFromDbNoTime )
                return
            }
            if ( !('_value' in dbData[parseInt(i)]) ) {
                console.error( 'showData(): no _value field in dbData[',i,']' )
                if ( (dbglevel > 1) && alerts )
                    alert ( 'showData(): '+ window.instrulang.dataFromDbNoValue )
                return
        }
        }
    }
    if ( dbglevel > 4 )
        console.log (
            'showData(): dbData.lenght: ', dbData.length,
            ' chartData.series[0].data.length ', chartData.series[0].data.length)

    // We will show only max. number of points, reject the excess history
    // or make undersampling if the sampling frequency is too high
    lastDataTimestamp.idx = -1
    for ( let i=(nofGraphPoints>dbData.length)?0:(dbData.length-nofGraphPoints);
           (i < dbData.length); i++ ){
        var getSeriesValue = (function (iobj, nofDec) {
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
                retvalue = iobj._value.toFixed( nofDec )
            }
            return {
                name: retname,
                data: retvalue,
                stmp: retstamp
            }
        }) // function seriesData()
        var seriesData = getSeriesValue( dbData[parseInt(i)], nofValDecimals )
        if ( dbglevel > 4 )
            console.log (
                'showData(): ',
                ' seriesData.name ', seriesData.name,
                ' seriesData.data ', seriesData.data,
                ' seriesData.stmp ', seriesData.stmp )
        if ( (dbglevel > 5) && alerts )
            alert ( 'showData():'+ '\n' +
                    'seriesData.name ' + seriesData.name + '\n' +
                    'seriesData.data ' + seriesData.data + '\n' +
                    'seriesData.stmp ' + seriesData.stmp )
        if ( seriesData.stmp > lastDataTimestamp.stmp ) {
            lastDataTimestamp.stmp = seriesData.stmp
            lastDataTimestamp.idx = i
            chartData.categories.push( seriesData.name )
            chartData.series[0].data.push( seriesData.data )
        }
    } // for received data from start to max. points
    chartData.series[0].name = that.path

    // Let's collect some statistics and adjust samplng if necessary
    if ( !frequencyAnalysisDone ) {

        let nofSamples = dbData.length
        let startStamp = Date.parse( dbData[0]._time )
        let endStamp   = Date.parse( dbData[(nofSamples - 1)]._time )
        let sampleFrequency = ( nofSamples / (endStamp -startStamp) * 1000 )

        if ( nofFrequencyAnalysis < limitOfFrequencyAnalysis ) {

            sumOfFrequencies += sampleFrequency
            if ( sampleFrequency < frequencyStats.lowest )
                frequencyStats.lowest = sampleFrequency
            if ( sampleFrequency > frequencyStats.highest )
                frequencyStats.highest = sampleFrequency
            let genuineNewIdx = lastDataTimestamp.idx + 1
            sumOfOverlapIdx += genuineNewIdx
            if ( genuineNewIdx < overlapStats.lowest )
                overlapStats.lowest = genuineNewIdx
            if ( genuineNewIdx > overlapStats.highest )
                overlapStats.highest = genuineNewIdx
            nofFrequencyAnalysis++
        }
        else {
            frequencyStats.avg = ( sumOfFrequencies -
                                   frequencyStats.lowest -
                                   frequencyStats.highest ) /
                                 ( nofFrequencyAnalysis - 2 )
            overlapStats.avg   = ( sumOfOverlapIdx -
                                   overlapStats.lowest -
                                   overlapStats.highest ) /
                                 ( nofFrequencyAnalysis - 2 )
            if ( dbglevel > 4 )
                console.log
                ( 'showData(): latest sampleFrequency: ', sampleFrequency,
                  'latest nofSamples: ', nofSamples,
                  'getRetrieveSeconds(): ', getRetrieveSeconds(),
                  'latest startStamp: ', startStamp,
                  'latest endStamp: ', endStamp,
                  'frequencyStats.lowest: ', frequencyStats.lowest,
                  'frequencyStats.highest: ', frequencyStats.highest,
                  'frequencyStats.avg: ', frequencyStats.avg,
                  'overlapStats.lowest: ', overlapStats.lowest,
                  'overlapStats.highest: ', overlapStats.highest,
                  'overlapStats.avg: ', overlapStats.avg
               )
            if ( (dbglevel > 5) && alerts )
                alert
                ( 'showData(): latest sampleFrequency: ' + sampleFrequency + '\n' +
                  'latest nofSamples: ' + nofSamples + '\n' +
                  'getRetrieveSeconds(): ' + getRetrieveSeconds() + '\n' +
                  'latest startStamp: ' + startStamp + '\n' +
                  'latest endStamp: ' + endStamp + '\n' +
                  'frequencyStats.lowest: ' + frequencyStats.lowest + '\n' +
                  'frequencyStats.highest: ' + frequencyStats.highest + '\n' +
                  'frequencyStats.avg: ' + frequencyStats.avg + '\n' +
                  'overlapStats.lowest: ' + overlapStats.lowest + '\n' +
                  'overlapStats.highest: ' + overlapStats.highest + '\n' +
                  'overlapStats.avg: ' + overlapStats.avg
                )
            // Calculate a new, representative history window to retrieve

            if ( (overlapStats.avg > 0) && (frequencyStats.avg !== 0) ) {
                let newRetrieveSeconds = Math.round( (nofGraphPoints +
                    contingencyFrequencyAdjustmentSeconds) / frequencyStats.avg )
                if ( dbglevel > 4 )
                    console.log ( 'showData() newRetrieveSeconds: ',
                                  newRetrieveSeconds )
                setRetrieveSeconds( newRetrieveSeconds)
            } // then useless data, reduce timespan

            frequencyAnalysisDone = true
        }
    }

    let numberOfNewPoints = chartData.series[0].data.length

    if ( numberOfNewPoints > 0 )
        updateTitleWithValue( chartData.series[0].data[numberOfNewPoints-1])

    showDataTimesTuiChart( numberOfNewPoints ) // call even if 0, useful for dbg

    return
}

export function clearData( that ) {
    let emptyArray = []
    dbData = emptyArray
    chartData.categories = emptyArray
    chartData.series[0].data.length = 0 // https://codepen.io/petrim/pen/yLyRQJK
    return
}

export function noData( that ) {
    setIdbClientForRetry()
}

export function prepareDataHalt( that ) {
    clearData( that )
}
