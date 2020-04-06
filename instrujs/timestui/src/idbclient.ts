/* $Id: idbclient.tsx, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
import InfluxDB from '../influxdb-client/packages/core/src/InfluxDB'
import FluxTableMetaData from '../influxdb-client/packages/core/src/query/FluxTableMetaData'
// import {url, token, org, bucket} from './env'

import DbSchema from '../../src/dbschema'
import { getPathSchema } from './path'

var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts

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
    if ( dbglevel > 0 )
        console.log('dataQuery()')
    if ( locstate !== 'RDY' ) {
        if ( dbglevel > 0 )
            console.log (
                'dataQuery(): statemachine violation, expected RDY, is ', locstate)
        return
    }
    let schma: DbSchema = getPathSchema()
    if ( schma.path === '' ) {
        if ( dbglevel > 0 )
            console.log (
                'dataQuery(): there is no database schema available for query')
        return
    }
    let url: string = schma.url
    let token: string = schma.token
    let org: string = schma.org

    alert (url + token + org)

    var queryApi = new InfluxDB({url, token}).getQueryApi(org)
    var fluxQuery: string = 'from(bucket:"'+ schma.bucket + '")'
    fluxQuery += '|> range(start: -10s)'
    fluxQuery += '|> filter(fn: (r) => r._measurement == "'
    fluxQuery + schma.sMeasurement + '")'
    if ( schma.sField1 !== '' ) {
        fluxQuery += '|> filter(fn: (r) => r._field == "'
        fluxQuery += schma.sField1 + '")'
    }
    if ( schma.sProp1 !== '' ) {
        fluxQuery += '|> filter(fn: (r) => r._prop1 == "'
        fluxQuery += schma.sProp1 + '")'
    }
    if ( schma.sProp2 !== '' ) {
        fluxQuery += '|> filter(fn: (r) => r._prop2 == "'
        fluxQuery += schma.sProp2 + '")'
    }
    if ( schma.sProp3 !== '' ) {
        fluxQuery += '|> filter(fn: (r) => r._prop3 == "'
        fluxQuery += schma.sProp3 + '")'
    }

    alert( fluxQuery )

    if ( dbglevel > 2 )
        console.log('*** QUERY ROWS ***');
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
            locstate = 'ERR';
            alert('DB error' + error.message);
            (window as any).iface.seterrdata()
            if ( dbglevel > 0 ) {
                console.log('\nDB Query finished ERROR')
            }
            if ( dbglevel > 1 ) {
                console.error(error)
                if ( alerts )
                    alert('DB error' + error.message);
            }
        },
        complete() {
            locstate = 'RES';
            (window as any).iface.newdata(0)
            if ( dbglevel > 2 )
                console.log('\nDB Query finished SUCCESS')
        },
    })
}
