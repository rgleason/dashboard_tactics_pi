"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.dataQuery = exports.getCollectedDataJSON = exports.setRetrieveSeconds = exports.getRetrieveSeconds = exports.setIdbClientForRetry = exports.setIdbClientStateGotError = exports.setIdbClientStateHasResult = exports.getIdbClientState = exports.initIdbClient = void 0;
/* $Id: idbclient.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
const InfluxDB_1 = require("../influxdb-client/packages/core/src/InfluxDB");
const path_1 = require("./path");
var dbglevel = window.instrustat.debuglevel;
var alerts = window.instrustat.alerts;
var locstate = '';
var jsonCollectedData = [];
var retJsonArray = [];
// This OK only if no simultaneous writer socket _and_ no CORS:
// let url: string = 'http://' + schma.url
// Otherwise we shall access through a CORS proxy:
var url = window.instrustat.corsproxy;
var retrieveSeconds = 300;
function initIdbClient() {
    locstate = 'RDY';
}
exports.initIdbClient = initIdbClient;
function getIdbClientState() {
    return locstate;
}
exports.getIdbClientState = getIdbClientState;
function setIdbClientStateHasResult() {
    locstate = 'RES';
}
exports.setIdbClientStateHasResult = setIdbClientStateHasResult;
function setIdbClientStateGotError() {
    locstate = 'ERR';
}
exports.setIdbClientStateGotError = setIdbClientStateGotError;
function setIdbClientForRetry() {
    jsonCollectedData.length = 0;
    locstate = 'RDY';
}
exports.setIdbClientForRetry = setIdbClientForRetry;
function getRetrieveSeconds() {
    return retrieveSeconds;
}
exports.getRetrieveSeconds = getRetrieveSeconds;
function setRetrieveSeconds(newValue) {
    var nVal = newValue || null;
    if ((nVal === null) || (nVal === 0))
        return;
    retrieveSeconds = nVal;
}
exports.setRetrieveSeconds = setRetrieveSeconds;
function getTimeRangeBackFromNow() {
    let tNowDate = new Date();
    let tNowMs = tNowDate.getTime();
    var tNowISOs = tNowDate.toISOString();
    let tBackMs = tNowMs - (retrieveSeconds * 1000);
    let tBackDate = new Date(tBackMs);
    let tBackISOs = tBackDate.toISOString();
    return {
        start: tBackISOs,
        stop: tNowISOs
    };
}
function getCollectedDataJSON() {
    if (locstate !== 'RES') {
        if (dbglevel > 0)
            console.log('getCollectedDataJSON(): statemachine violation, expected RES, is ', locstate);
        return [];
    }
    retJsonArray = jsonCollectedData;
    let emptyArray = [];
    jsonCollectedData = emptyArray;
    jsonCollectedData.length = 0;
    locstate = 'RDY';
    return retJsonArray;
}
exports.getCollectedDataJSON = getCollectedDataJSON;
function dataQuery() {
    if (dbglevel > 0)
        console.log('dataQuery()');
    if ((dbglevel > 5) && alerts)
        alert('dataQuery() - locstate: ' + locstate);
    if (locstate !== 'RDY') {
        if (dbglevel > 0)
            console.log('dataQuery(): statemachine violation, expected RDY, is ', locstate);
        return;
    }
    let schma = path_1.getPathSchema();
    if (schma.path === '') {
        if (dbglevel > 0)
            console.log('dataQuery(): there is no database schema available for query');
        return;
    }
    let token = schma.token;
    let org = schma.org;
    if ((dbglevel > 6) && alerts)
        alert('dataQuery() - creating queryAPI:\n' +
            'url: ' + url + '\n' +
            'org: ' + org + '\n' +
            'token: ' + token);
    var queryApi = new InfluxDB_1.default({ url, token }).getQueryApi(org);
    var fQry = 'from(bucket:"' + schma.bucket + '")\n';
    let tRge = getTimeRangeBackFromNow();
    fQry += '  |> range(start: ' + tRge.start + ', stop: ' + tRge.stop + ')\n';
    fQry += '  |> filter(fn: (r) => \n';
    fQry += '    r._measurement == "';
    fQry += schma.sMeasurement + '"';
    if (schma.sField1 !== '') {
        fQry += ' and\n';
        fQry += '    r._field == "';
        fQry += schma.sField1 + '"';
    }
    if (schma.sProp1 !== '') {
        fQry += ' and\n';
        fQry += '    r.prop1 == "';
        fQry += schma.sProp1 + '"';
    }
    if (schma.sProp2 !== '') {
        fQry += ' and\n';
        fQry += '    r.prop2 == "';
        fQry += schma.sProp2 + '"';
    }
    if (schma.sProp3 !== '') {
        fQry += ' and\n';
        fQry += '    r.prop3 == "';
        fQry += schma.sProp3 + '"';
    }
    fQry += '\n)';
    if (!(path_1.getPathDbFunc() === '')) {
        fQry += '\n';
        fQry += '  |> ' + path_1.getPathDbFunc() + '\n';
    }
    // Quite handy if cannot find data - attention with the delay, no query while you are looking at the alert!
    // ref https://docs.influxdata.com/flux/v0.50/introduction/getting-started/query-influxdb/#3-filter-your-data
    if (dbglevel > 4)
        console.log('dataQuery(): fQry: ', fQry);
    if ((dbglevel > 5) && alerts)
        alert('dataQuery(): fQry: ' + fQry);
    // performs query and receive line table metadata and rows
    // https://v2.docs.influxdata.com/v2.0/reference/syntax/annotated-csv/
    queryApi.queryRows(fQry, {
        next(row, tableMeta) {
            if (dbglevel > 5)
                console.log('queryApi.queryRows() - row', row);
            const o = tableMeta.toObject(row);
            /*
             JSON stringify the returned object to simplify the type definition
             of the collected array as collection of JSON object strings.
             The consumer of the data shall know what it has ordered...
             */
            let rowdata = JSON.stringify(o, null, 2);
            if (dbglevel > 5)
                console.log(rowdata);
            jsonCollectedData.push(rowdata);
            if (dbglevel > 4)
                console.log('queryApi.queryRows() - jsonCollectedData.length: ', jsonCollectedData.length);
            // console.log( `${o._time} ${o._measurement}.${o._field}=${o._value}` )
        },
        error(error) {
            // note: this function cannot see module variables/functions
            // local state transition via the fsm state transition
            // setIdbClientStateGotError()
            if (dbglevel > 0) {
                console.log('\nDB Query finished ERROR');
            }
            if (dbglevel > 1) {
                console.error(error);
                if (alerts)
                    alert('DB error: ' + error.message);
            }
            try {
                window.iface.seterrdata();
            }
            catch (err) {
                console.error('dataQuery(): error(): iface.seterrdata() failed, error: ', err.message);
            }
        },
        complete() {
            // note: this function cannot see module variables/functions
            // local state transition via the fsm state transition
            // setIdbClientStateHasResult()
            if (dbglevel > 2)
                console.log('\nDB Query finished SUCCESS');
            try {
                window.iface.newdata(0);
            }
            catch (err) {
                console.error('dataQuery(): error(): iface.newdata() failed, error: ', err.message);
            }
        },
    });
}
exports.dataQuery = dataQuery;
