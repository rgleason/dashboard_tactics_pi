/* $Id: idbclient.tsx, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
import InfluxDB from '../../src/influxdb-client/packages/core/src/InfluxDB'
import FluxTableMetaData from '../../src/influxdb-client/packages/core/src/query/FluxTableMetaData'
// import {url, token, org, bucket} from './env'

import DbSchema from '../../src/dbschema'
import { getPathSchema, getPathDbFunc, getPathDbNum } from './path'

var dbglevel: number = (window as any).instrustat.debuglevel
var alerts: boolean = (window as any).instrustat.alerts

var locstate: string = ''
var jsonCollectedData: string[] = []
var retJsonArray: string[] = []

var retrieveSeconds: number = 300

export function initIdbClient() {
    locstate = 'RDY'
}

export function getIdbClientState() : string {
    return locstate
}

export function setIdbClientStateHasResult() {
    locstate = 'RES'
}

export function setIdbClientStateGotError() {
    locstate = 'ERR'
}

export function setIdbClientForRetry() {
    jsonCollectedData.length = 0
    locstate = 'RDY'
}

export function getRetrieveSeconds(): number {
    return retrieveSeconds
}

export function setRetrieveSeconds( newValue: number ) {
    var nVal = newValue || null
    if ( (nVal === null) || (nVal == 0) )
        return
    retrieveSeconds = nVal
}
/*
 Reason why we do not manage time backwards relative is two folded:
 1) The server time, especially on Hyper-V Docker is not perhaps same as there
 2) We make provisions to allow methods to retrieve slises from the history
 */
interface TimeRange {
    start: string
    stop: string
}
function getTimeRangeBackFromNow(): TimeRange {
    let tNowDate = new Date()
    let tNowMs = tNowDate.getTime()
    var tNowISOs = tNowDate.toISOString()
    let tBackMs = tNowMs - (retrieveSeconds * 1000)
    let tBackDate = new Date(tBackMs)
    let tBackISOs = tBackDate.toISOString()
    return{
        start: tBackISOs,
        stop:  tNowISOs
    }
}

export function getCollectedDataJSON():string[] {
    if ( locstate !== 'RES' ) {
        if ( dbglevel > 0 )
            console.log (
                'getCollectedDataJSON(): statemachine violation, expected RES, is ',
                locstate)
        return []
    }
    retJsonArray = jsonCollectedData
    let emptyArray: string[] = []
    jsonCollectedData = emptyArray
    jsonCollectedData.length = 0
    locstate = 'RDY'
    return retJsonArray
}

export function dataQuery() {
    if ( dbglevel > 0 )
        console.log('dataQuery()')
    if ( (dbglevel > 5) && alerts )
        alert( 'dataQuery() - locstate: ' + locstate )
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

    // This OK only if no simultaneous writer socket _and_ no CORS:
    // let url: string = 'http://' + schma.url
    // Otherwise we shall access through a CORS proxy
    let url:string = (window as any).instrustat.corsproxy

    let token: string = schma.token
    let org: string = schma.org

    var queryApi = new InfluxDB({url, token}).getQueryApi(org)
    var fQry: string = 'from(bucket:"'+ schma.bucket + '")\n'
    let tRge = getTimeRangeBackFromNow()
    fQry += '  |> range(start: ' + tRge.start + ', stop: ' + tRge.stop + ')\n'
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
    if ( !(getPathDbFunc() === '') ) {
        fQry += '\n'
        fQry += '  |> ' + getPathDbFunc() + '\n'
    }
    // Quite handy if cannot find data - attention with the delay, no query while you are looking at the alert!
    // ref https://docs.influxdata.com/flux/v0.50/introduction/getting-started/query-influxdb/#3-filter-your-data
    if ( dbglevel > 4 )
        console.log('dataQuery(): fQry: ', fQry )
    if ( (dbglevel > 5) && alerts )
        alert( 'dataQuery(): fQry: ' + fQry )

    // performs query and receive line table metadata and rows
    // https://v2.docs.influxdata.com/v2.0/reference/syntax/annotated-csv/
    queryApi.queryRows(fQry, {
        next(row: string[], tableMeta: FluxTableMetaData) {
            if ( dbglevel > 5 )
                console.log('queryApi.queryRows() - row', row )
            const o = tableMeta.toObject(row)
            /*
             JSON stringify the returned object to simplify the type definition
             of the collected array as collection of JSON object strings.
             The consumer of the data shall know what it has ordered...
             */
            let rowdata:string = JSON.stringify(o, null, 2)
            if ( dbglevel > 5 )
                console.log( rowdata )

            jsonCollectedData.push( rowdata )
            if ( dbglevel > 4 )
                console.log(
                    'queryApi.queryRows() - jsonCollectedData.length: ',
                    jsonCollectedData.length )
            // console.log( `${o._time} ${o._measurement}.${o._field}=${o._value}` )
        },
        error(error: Error) {
            // note: this function cannot see module variables/functions
            // local state transition via the fsm state transition
            // setIdbClientStateGotError()
            if ( dbglevel > 0 ) {
                console.log('\nDB Query finished ERROR')
            }
            if ( dbglevel > 1 ) {
                console.error(error)
                if ( alerts )
                    alert('DB error' + error.message);
            }
            try {
                (window as any).iface.seterrdata()
            }
            catch( err ) {
                console.error(
                    'dataQuery(): error(): iface.seterrdata() failed, error: ',
                    err.message)
            }
        },
        complete() {
            // note: this function cannot see module variables/functions
            // local state transition via the fsm state transition
            // setIdbClientStateHasResult()
            if ( dbglevel > 2 )
                console.log('\nDB Query finished SUCCESS')
                try {
                    (window as any).iface.newdata(0)
                }
                catch( err ) {
                    console.error(
                        'dataQuery(): error(): iface.newdata() failed, error: ',
                        err.message)
                }
        },
    })
}
