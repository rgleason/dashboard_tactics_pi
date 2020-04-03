/* $Id: idbclient.tsx, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
import InfluxDB from '../influxdb-client/packages/core/src/InfluxDB'
import FluxTableMetaData from '../influxdb-client/packages/core/src/query/FluxTableMetaData'
import {url, token, org, bucket} from './env'

var dbglevel: number = (window as any).instrustat.debuglevel

var locstate: string = ''
var jsonCollectedData: string[] = []

export function initIdbClient() {
    locstate = 'RDY'
}

export function getIdbClientState() : string {
    return locstate
}

export function setIdbClientForRetry() {
    let emptyArray: string[] = []
    jsonCollectedData = emptyArray
    locstate = 'RDY'
}

export function getCollectedDataJSON():string[] {
    if ( locstate !== 'RES' ) {
        if ( dbglevel > 0 )
            console.log (
                'getCollectedDataJSON(): statemachine violation, expected RES, is ', locstate)
        return []
    }
    let retJsonArray: string[] = jsonCollectedData
    let emptyArray: string[] = []
    jsonCollectedData = emptyArray
    locstate = 'RDY'
    return retJsonArray
}

export function dataQuery() {
    if ( locstate !== 'RDY' ) {
        if ( dbglevel > 0 )
            console.log (
                'dataQuery(): statemachine violation, expected RDY, is ', locstate)
        return
    }
    if ( dbglevel > 0 )
        console.log('querytest')
    var queryApi = new InfluxDB({url, token}).getQueryApi(org)
    var fluxQuery: string = 'from(bucket:"'+ bucket + '")'
    fluxQuery += '|> range(start: -10s)'
    fluxQuery += '|> filter(fn: (r) => r._measurement == "environment")'
    fluxQuery += '|> filter(fn: (r) => r._field == "speedTrueGround")'
    fluxQuery += '|> filter(fn: (r) => r.prop2 == "mwv")'

    if ( dbglevel > 2 )
        console.log('*** QUERY ROWS ***')
    // performs query and receive line table metadata and rows
    // https://v2.docs.influxdata.com/v2.0/reference/syntax/annotated-csv/
    queryApi.queryRows(fluxQuery, {
        next(row: string[], tableMeta: FluxTableMetaData) {
            const o = tableMeta.toObject(row)
            let rowdata:string = JSON.stringify(o, null, 2)
            console.log( rowdata )
            jsonCollectedData.push( rowdata )
            // console.log( `${o._time} ${o._measurement}.${o._field}=${o._value}` )
        },
        error(error: Error) {
            locstate = 'ERR'
            if ( dbglevel > 2 ) {
                console.error(error)
                console.log('\nFinished ERROR')
            }
        },
        complete() {
            locstate = 'RES'
            if ( dbglevel > 3 )
                console.log('\nFinished SUCCESS')
        },
    })
}
