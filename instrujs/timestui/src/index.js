"use strict";
/* $Id: index.tsx, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
exports.__esModule = true;
var version_1 = require("../../src/version");
console.log('timestui ', version_1.packagename(), ' ', version_1.version());
var dbglevel = window.instrustat.debuglevel;
require("../../src/iface.js");
require("../sass/style.scss");
var kbd_1 = require("../../src/kbd");
var statemachine_1 = require("./statemachine");
var state_machine_visualize_1 = require("../../src/state-machine-visualize");
var unloadwebkitiescrollbars_1 = require("./unloadwebkitiescrollbars");
var idbclient_1 = require("./idbclient");
console.log('making a DB querytest()');
idbclient_1["default"]();
console.log('out of DB querytest()');
// we access it with window.iface but this is needed once, to get it in...
// var iface = require('exports-loader?iface!../../src/iface.js')
// var iface = (window as any).iface
// State Machine Service
if (dbglevel > 0)
    console.log('index.js - creating the finite state machine');
var fsm = statemachine_1.createStateMachine();
if (dbglevel > 0)
    console.log('fsm created - state: ', fsm.state);
try {
    var dot = state_machine_visualize_1["default"](fsm);
    window.iface.setgraphwizdot(dot);
    if (dbglevel > 0)
        console.log('index.js: state machine GraphWiz presentation available through iface.js');
}
catch (error) {
    console.error('index.js: state machine visualize, error: ', error);
}
try {
    fsm.init();
}
catch (error) {
    console.error('index.js: fsm.init() transition failed, error: ', error);
}
// Create the transitional events (the IE way, sorry!) for clieant messages
var bottom = document.getElementById('bottom');
if (!bottom) {
    throw 'timestui: init: no element: bottom';
}
// UID and configuration file
var eventsetid = document.createEvent('Event');
eventsetid.initEvent('setid', false, false);
bottom.addEventListener('setid', (function (event) {
    if (dbglevel > 1)
        console.log('pollhascfg() - attempt to setid() transition.');
    try {
        fsm.setid();
    }
    catch (error) {
        console.error('Event:  setid: fsm.setid() transition failed, error: ', error, ' current state: ', fsm.state);
    }
    var pollhascfg = (function () {
        if (dbglevel > 1)
            console.log('pollhascfg() - waiting on "hasid" state');
        if (fsm.is('hasid')) {
            var hascfg = true;
            if (fsm.conf === null)
                hascfg = false;
            else if ((fsm.conf.path === null) || (fsm.conf.path === ''))
                hascfg = false;
            if (hascfg) {
                try {
                    fsm.hascfg();
                }
                catch (error) {
                    console.error('index.js:  pollhascfg(): fsm.hascfg() transition failed, error: ', error, ' current state: ', fsm.state);
                }
            }
            else {
                try {
                    fsm.nocfg();
                }
                catch (error) {
                    console.error('index.js: pollhascfg(): fsm.nocfg() transition failed, error: ', error, ' current state: ', fsm.state);
                }
            }
        }
        else {
            var n;
            n = window.setTimeout(function () { return pollhascfg; }, 100);
        }
    }()); // do selection of the next action in the routing once ID has been set, or not
})); // hey non-semicolon-TS-person - this is needed!
window.iface.regeventsetid(bottom, eventsetid);
// All available paths have been set
var eventsetall = document.createEvent('Event');
eventsetall.initEvent('setall', false, false);
bottom.addEventListener('setall', (function (event) {
    console.error('Event:  setall: error: timestui does not require all paths');
})); // hey non-semicolon-TS-person - this is needed!
window.iface.regeventsetall(bottom, eventsetall);
// All available DB schema paths have been set
var eventsetalldb = document.createEvent('Event');
eventsetalldb.initEvent('setalldb', false, false);
bottom.addEventListener('setalldb', (function (event) {
    try {
        fsm.setalldb();
    }
    catch (error) {
        console.error('Event:  setall: fsm.setalldb() transition failed, error: ', error, ' current state: ', fsm.state);
    }
}));
window.iface.regeventsetalldb(bottom, eventsetalldb);
// Selection of a path has been made
var eventselected = document.createEvent('Event');
eventselected.initEvent('selected', false, false);
bottom.addEventListener('selected', (function (event) {
    try {
        fsm.selected();
    }
    catch (error) {
        console.error('Event:  selected: fsm.selected() transition failed, error: ', error, ' current state: ', fsm.state);
    }
}));
window.iface.regeventselected(bottom, eventselected);
// Selection of a path requires reload
var eventrescan = document.createEvent('Event');
eventrescan.initEvent('rescan', false, false);
bottom.addEventListener('rescan', (function (event) {
    try {
        fsm.rescan();
    }
    catch (error) {
        console.error('Event:  rescan: fsm.rescan() transition failed, error: ', error, ' current state: ', fsm.state);
    }
}));
window.iface.regeventrescan(bottom, eventrescan);
// Selection of a path has been acknowledged
var eventacksubs = document.createEvent('Event');
eventacksubs.initEvent('acksubs', false, false);
bottom.addEventListener('acksubs', (function (event) {
    console.error('Event:  ackschema: error: timestui does not ask for path subscription');
}));
window.iface.regeventacksubs(bottom, eventacksubs);
// Requested database schema is now available
var eventackschema = document.createEvent('Event');
eventackschema.initEvent('ackschema', false, false);
bottom.addEventListener('ackschema', (function (event) {
    try {
        fsm.ackschema();
    }
    catch (error) {
        console.error('Event:  acksubs: fsm.ackschema() transition failed, error: ', error, ' current state: ', fsm.state);
    }
}));
window.iface.regeventackschema(bottom, eventackschema);
// New data is coming in
var eventnewdata = document.createEvent('Event');
eventnewdata.initEvent('newdata', false, false);
bottom.addEventListener('newdata', (function (event) {
    console.error('Event:  newdata: error: timestui does not ask data from a path subscription');
}));
window.iface.regeventnewdata(bottom, eventnewdata);
// Change of configuration has been requested
var eventchgconf = document.createEvent('Event');
eventchgconf.initEvent('chgconf', false, false);
bottom.addEventListener('chgconf', (function (event) {
    try {
        fsm.chgconf();
    }
    catch (error) {
        console.error('Event:  chgconf: fsm.chgconf() transition failed, error: ', error, ' current state: ', fsm.state);
    }
}));
window.iface.regeventchgconf(bottom, eventchgconf);
// Luminosity
var eventluminsty = document.createEvent('Event');
eventluminsty.initEvent('luminsty', false, false);
bottom.addEventListener('luminsty', (function (event) {
    try {
        fsm.luminsty();
    }
    catch (error) {
        console.error('Event:  luminsty: fsm.luminsty() transition failed, error: ', error, ' current state: ', fsm.state);
    }
}));
window.iface.regeventluminsty(bottom, eventluminsty);
// Keyboard event requires to swap the display format
kbd_1.kbdInit();
var eventswapdisp = document.createEvent('Event');
eventswapdisp.initEvent('swapdisp', false, false);
bottom.addEventListener('swapdisp', (function (event) {
    try {
        if (fsm.state === 'showdata')
            fsm.swapdisp();
    }
    catch (error) {
        console.error('Event:  swapdisp: fsm.swapdisp() transition failed, error: ', error, ' current state: ', fsm.state);
    }
}));
window.iface.regeventswapdisp(bottom, eventswapdisp);
// The instrument has a persistent configuration object, close gracefully
var eventclosing = document.createEvent('Event');
eventclosing.initEvent('closing', false, false);
bottom.addEventListener('closing', (function (event) {
    try {
        fsm.closing();
    }
    catch (error) {
        console.error('Event:  closing: fsm.closing() transition failed, error: ', error, ' current state: ', fsm.state);
    }
}));
window.iface.regeventclosing(bottom, eventclosing);
/* Since now no other events apart the window load(), we need to await here until
   it has been executed, before continuing to truy event driven operation */
function pollinitga() {
    if (dbglevel > 0)
        console.log('pollinitga() - waiting for initga, now: ', fsm.state);
    if (fsm.is('initga')) {
        try {
            fsm.initok();
        }
        catch (error) {
            console.error('index.js:  fsm.initok() transition failed, error: ', error, ' current state: ', fsm.state);
        }
    }
    else {
        setTimeout(pollinitga, 100);
    }
}
pollinitga(); // do _everything_ in the routing once condition met
/* ------------------------------------------ */
var unloadScrollBars = function () {
    unloadwebkitiescrollbars_1["default"]();
};
window.addEventListener('load', function () {
    if (dbglevel > 0)
        console.log('index.js - state: ', fsm.state);
    unloadScrollBars();
    // Loading state done
    try {
        fsm.loaded();
    }
    catch (error) {
        console.error('loading: fsm.loaded() transition failed, error: ', error);
    }
}, false);
/* ------------------------------------------ */
