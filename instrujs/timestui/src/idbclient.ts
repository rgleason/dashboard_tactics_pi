/* $Id: idbclient.tsx, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
import InfluxDB from '../../src/influxdb-client/packages/core/src/InfluxDB'
import FluxTableMetaData from '../../src/influxdb-client/packages/core/src/query/FluxTableMetaData'
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

    // alert ('dataQuery()')

    let schma: DbSchema = getPathSchema()

    // alert (schma)

    if ( schma.path === '' ) {
        if ( dbglevel > 0 )
            console.log (
                'dataQuery(): there is no database schema available for query')
        return
    }

    // This OK only if no simultaneous writer socket _and_ no CORS:
    // let url: string = 'http://' + schma.url
    // Otherwise we shall access through a CORS proxy
    let url:string = (window as any).instrustat.corsproxy

    let token: string = schma.token
    let org: string = schma.org

    var queryApi = new InfluxDB({url, token}).getQueryApi(org)
    var fQry: string = 'from(bucket:"'+ schma.bucket + '")\n'
    fQry += '  |> range(start: -300s)\n'
    fQry += '  |> filter(fn: (r) => \n'
    fQry += '    r._measurement == "'
    fQry += schma.sMeasurement + '"'
    if ( schma.sField1 !== '' ) {
        fQry += ' and\n'
        fQry += '    r._field == "'
        fQry += schma.sField1 + '"'
    }
    if ( schma.sProp1 !== '' ) {
        fQry += ' and\n'
        fQry += '    r.prop1 == "'
        fQry += schma.sProp1 + '"'
    }
    if ( schma.sProp2 !== '' ) {
        fQry += ' and\n'
        fQry += '    r.prop2 == "'
        fQry += schma.sProp2 + '"'
    }
    if ( schma.sProp3 !== '' ) {
        fQry += ' and\n'
        fQry += '    r.prop3 == "'
        fQry += schma.sProp3 + '"'
    }
    fQry += '\n)'

    // Quite handy if cannot find data - attention with the delay, no query while you are looking alert!
    // ref https://docs.influxdata.com/flux/v0.50/introduction/getting-started/query-influxdb/#3-filter-your-data
    // alert( fQry )

    if ( dbglevel > 2 )
        console.log('*** QUERY ROWS ***');
    // performs query and receive line table metadata and rows
    // https://v2.docs.influxdata.com/v2.0/reference/syntax/annotated-csv/
    queryApi.queryRows(fQry, {
        next(row: string[], tableMeta: FluxTableMetaData) {
            const o = tableMeta.toObject(row)
            /*
             JSON stringify the returned object to simplify the type definition
             of the collected array as collection of JSON object strings.
             The consumer of the data shall know what it has ordered...
             */
            let rowdata:string = JSON.stringify(o, null, 2)
            if ( dbglevel > 0 )
                console.log( rowdata )

            jsonCollectedData.push( rowdata )
            // console.log( `${o._time} ${o._measurement}.${o._field}=${o._value}` )
        },
        error(error: Error) {
            locstate = 'ERR';

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
