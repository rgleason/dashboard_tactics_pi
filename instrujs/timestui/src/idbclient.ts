/* $Id: idbclient.tsx, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
// import "core-js"
// import "regenerator-runtime/runtime.js"

// import {InfluxDB, FluxTableMetaData} from '@influxdata/influxdb-client'
// import {InfluxDB} from '../influxdata/influxdb-client'
import InfluxDB from '../influxdb-client/packages/core/src/InfluxDB'
import FluxTableMetaData from '../influxdb-client/packages/core/src/query/FluxTableMetaData'
import {url, token, org} from './env'

export default function querytest() {
    console.log('querytest')
    var queryApi = new InfluxDB({url, token}).getQueryApi(org)
    var fluxQuery =
      'from(bucket:"my-bucket") |> range(start: 0) |> filter(fn: (r) => r._measurement == "temperature")'

    console.log('*** QUERY ROWS ***')
    // performs query and receive line table metadata and rows
    // https://v2.docs.influxdata.com/v2.0/reference/syntax/annotated-csv/
    queryApi.queryRows(fluxQuery, {
      next(row: string[], tableMeta: FluxTableMetaData) {
        const o = tableMeta.toObject(row)
        // console.log(JSON.stringify(o, null, 2))
        console.log(
          `${o._time} ${o._measurement} in '${o.location}' (${o.example}): ${o._field}=${o._value}`
        )
      },
      error(error: Error) {
        console.error(error)
        console.log('\nFinished ERROR')
      },
      complete() {
        console.log('\nFinished SUCCESS')
      },
    })

    // performs query and receive line results in annotated csv format
    // https://v2.docs.influxdata.com/v2.0/reference/syntax/annotated-csv/
    // queryApi.queryLines(
    //   fluxQuery,
    //   {
    //     error(error: Error) {
    //       console.error(error)
    //       console.log('\nFinished ERROR')
    //     },
    //     next(line: string) {
    //       console.log(line)
    //     },
    //     complete() {
    //       console.log('\nFinished SUCCESS')
    //     },
    //   }
}
