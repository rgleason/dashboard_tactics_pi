"use strict";
/* $Id: index.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
Object.defineProperty(exports, "__esModule", { value: true });
const version_1 = require("../../src/version");
console.log('timestui ', version_1.packagename(), ' ', version_1.version());
var dbglevel = window.instrustat.debuglevel;
require("../../src/iface.js");
// High level  operations available on iface.js
const ifaceops_1 = require("../../src/ifaceops");
require("../sass/style.scss");
const kbd_1 = require("../../src/kbd");
const unloadwebkitiescrollbars_1 = require("./unloadwebkitiescrollbars");
const chart_1 = require("./chart");
// InfluxDB client module
const idbclient_1 = require("./idbclient");
idbclient_1.initIdbClient();
// State Machine Services
const statemachine_1 = require("./statemachine");
const state_machine_visualize_1 = require("../../src/state-machine-visualize");
if (dbglevel > 0)
    console.log('index.ts - creating the finite state machine');
var fsm = statemachine_1.createStateMachine();
if (dbglevel > 0)
    console.log('fsm created - state: ', fsm.state);
try {
    var dot = state_machine_visualize_1.default(fsm);
    window.iface.setgraphwizdot(dot);
    if (dbglevel > 0)
        console.log('index.ts: state machine GraphWiz presentation available through iface.js');
}
catch (error) {
    console.error('index.ts: state machine visualize, error: ', error.message);
}
try {
    fsm.init();
}
catch (error) {
    console.error('index.ts: fsm.init() transition failed, error: ', error.message);
}
// Create the transitional events (the IE way, sorry!) for client messages
var bottom = document.getElementById('bottom');
if (bottom === null) {
    throw 'timestui: init: no element: bottom';
}
// Initiate and set the event handling on events available on iface.js
ifaceops_1.initIfaceOps(fsm, bottom);
ifaceops_1.askWaitGetUID();
ifaceops_1.ignoreSetAllPathsEvent();
ifaceops_1.createSetAllDbEvent();
ifaceops_1.createSetRescanEvent();
ifaceops_1.createSetSelectedEvent();
ifaceops_1.ignoreAckSubscriptionEvent();
ifaceops_1.createAckDbSchemaEvent();
ifaceops_1.ignoreSetGetNewEvent(); // see pollshowdata()
// New data is available - keep up the data streaming by transitional states
var eventnewdata = document.createEvent('Event');
eventnewdata.initEvent('newdata', false, false);
bottom.addEventListener('newdata', ((event) => {
    try {
        fsm.newdata();
    }
    catch (error) {
        console.error('Event: newdata: fsm.newdata() transition failed, error: ', error.message, ' current state: ', fsm.state);
    }
    /*
      Showdata is a transitional state - it has one entry transition
      1/ newdata - from waitdata - DB query has succeeded
      Two possible exits
      2/ getnew - new data query cycle
      3/ chgconf - mouse event, if occurs to ask for a config change
      Since FSM cannot launch its own event we need to serve getnew transition
    */
    var pollshowdata;
    var prevNewDataCnt = 0;
    var nofRemainedSame = 0;
    (pollshowdata = function () {
        var newDataCnt = chart_1.getAddedDataCount();
        var painting = true;
        if (newDataCnt !== prevNewDataCnt) {
            prevNewDataCnt = newDataCnt;
        }
        else {
            nofRemainedSame++;
            if (nofRemainedSame >= 1) // (450ms+del)=~500ms when chart follows
                painting = false;
        }
        if (dbglevel > 4)
            console.log('pollshowdata() - waiting for showdata, now: ', fsm.state, ' painting ', painting, ' (', newDataCnt, ',', prevNewDataCnt, ',', nofRemainedSame, ')');
        if (fsm.is('showdata') && !painting) {
            try {
                fsm.getnew();
            }
            catch (error) {
                console.error('index.ts:  fsm.newget() transition failed, error: ', error.message, ' current state: ', fsm.state);
            }
        }
        else {
            window.setTimeout(pollshowdata, 450);
        }
    })(); // make the transition from newdata to getnew by polling the state
}));
window.iface.regeventnewdata(bottom, eventnewdata);
// Asynchronous data retrieval from DB has failed
var eventerrdata = document.createEvent('Event');
eventerrdata.initEvent('errdata', false, false);
bottom.addEventListener('errdata', ((event) => {
    try {
        fsm.errdata();
    }
    catch (error) {
        console.error('Event: errdata: fsm.errdata() transition failed, error: ', error.message, ' current state: ', fsm.state);
    }
    /*
      Nodata is a transitional state - it has one entry transition
      1/ errdata - from waitdata - DB query has failed
      Two possible exits
      2/ retryget - new data query cycle launched despite error
      3/ chgconf - mouse event, if occurs to ask for a config change
      Since FSM cannot launch its own event we need to serve retryget transition
    */
    var pollerrdata;
    (pollerrdata = function () {
        if (dbglevel > 0)
            console.log('pollerrdata() - waiting for nodata, now: ', fsm.state);
        if (fsm.is('nodata')) {
            try {
                fsm.retryget();
            }
            catch (error) {
                console.error('index.ts:  fsm.retryget() transition failed, error: ', error.message, ' current state: ', fsm.state);
            }
        }
        else {
            window.setTimeout(pollerrdata, 100);
        }
    })(); // make the transition from errdata to retryget by polling the state
}));
window.iface.regeventerrdata(bottom, eventerrdata);
// Retry Asynchronous data retrieval from DB
ifaceops_1.createSetRetryGetEvent();
ifaceops_1.createSetChgConfEvent();
// Common housekeeping requests
ifaceops_1.createSetLuminosityEvent();
// Keyboard event requires to swap the display format
kbd_1.kbdInit();
// Not served (yet)
ifaceops_1.ignoreSetSwapDispEvent();
// The instrument has a persistent configuration object, close gracefully
ifaceops_1.createSetClosingEvent();
// Let's register to events not used here - better have a handler, though
ifaceops_1.ignoreSetGetFeetEvent();
ifaceops_1.ignoreSetGetNoFeetEvent();
ifaceops_1.ignoreSetChkRdyEvent();
ifaceops_1.ignoreSetChkNoRdyEvent();
ifaceops_1.ignoreSetUserslEvent();
ifaceops_1.ignoreSetNoUserslEvent();
ifaceops_1.ignoreSetMarkAckEvent();
ifaceops_1.ignoreSetDataAckEvent();
ifaceops_1.ignoreSetNewSlDataEvent();
ifaceops_1.ignoreSetSldStopEvent();
ifaceops_1.ignoreSetMrkDataAckEvent();
ifaceops_1.ignoreSetNewMrkDataEvent();
ifaceops_1.ignoreSetMarkMuteEvent();
ifaceops_1.ignoreSetMarkUnMuteEvent();
/* Since now no other events apart the window load(), we need to await here until
   it has been executed, before continuing to try event driven operation */
var reloadDelay = 2;
var pollinitga;
(pollinitga = function () {
    if (dbglevel > 0)
        console.log('pollinitga() - waiting for initga, now: ', fsm.state);
    if (fsm.is('initga')) {
        if (reloadDelay === 0) {
            try {
                fsm.initok();
            }
            catch (error) {
                console.error('index.ts:  fsm.initok() transition failed, error: ', error.message, ' current state: ', fsm.state);
            }
        }
        else {
            reloadDelay--;
            if (dbglevel > 1)
                console.log('pollinitga() - initga+reloadDelay: ', reloadDelay);
            setTimeout(pollinitga, 500);
        }
    }
    else {
        setTimeout(pollinitga, 100);
    }
})(); // do _everything_ in the routing once condition met
/* ------------------------------------------ */
var unloadScrollBars = function () {
    unloadwebkitiescrollbars_1.default();
};
window.addEventListener('load', function () {
    if (dbglevel > 0)
        console.log('index.ts - state: ', fsm.state);
    unloadScrollBars();
    // Loading state done
    try {
        fsm.loaded();
    }
    catch (error) {
        console.error('loading: fsm.loaded() transition failed, error: ', error.message);
    }
}, false);
/* ------------------------------------------ */
