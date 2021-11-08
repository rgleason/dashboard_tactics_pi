"use strict";
/* $Id: sifaceops.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
// Provide TypeScript helpers to avoid code copy-paste in work with iface.js
exports.__esModule = true;
exports.createSetClosingEvent = exports.ignoreSetClosingEvent = exports.createSetSwapDispEvent = exports.ignoreSetSwapDispEvent = exports.createSetLuminosityEvent = exports.ignoreSetLuminosityEvent = exports.createSetMarkUnMuteEvent = exports.createSetMarkMuteEvent = exports.ignoreSetMarkUnMuteEvent = exports.ignoreSetMarkMuteEvent = exports.createSetNewMrkDataEvent = exports.createSetMrkDataAckEvent = exports.ignoreSetNewMrkDataEvent = exports.ignoreSetMrkDataAckEvent = exports.createSetSldStopEvent = exports.ignoreSetSldStopEvent = exports.createSetNewSlDataEvent = exports.ignoreSetNewSlDataEvent = exports.createSetDataAckEvent = exports.ignoreSetDataAckEvent = exports.createSetMarkAckEvent = exports.ignoreSetMarkAckEvent = exports.createSetNoUserslEvent = exports.createSetUserslEvent = exports.ignoreSetNoUserslEvent = exports.ignoreSetUserslEvent = exports.createSetChkNoRdyEvent = exports.createSetChkRdyEvent = exports.ignoreSetChkNoRdyEvent = exports.ignoreSetChkRdyEvent = exports.createSetGetNoFeetEvent = exports.createSetGetFeetEvent = exports.ignoreSetGetNoFeetEvent = exports.ignoreSetGetFeetEvent = exports.createSetRetryGetEvent = exports.ignoreSetRetryGetEvent = exports.createSetChgConfEvent = exports.ignoreSetChgConfEvent = exports.createSetErrDataEvent = exports.ignoreSetErrDataEvent = exports.createNewDataEvent = exports.ignoreNewDataEvent = exports.createSetGetNewEvent = exports.ignoreSetGetNewEvent = exports.createAckDbSchemaEvent = exports.ignoreAckDbSchemaEvent = exports.createAckSubscriptionEvent = exports.ignoreAckSubscriptionEvent = exports.createSetSelectedEvent = exports.ignoreSetSelectedEvent = exports.createSetRescanEvent = exports.ignoreSetRescanEvent = exports.createSetAllDbEvent = exports.ignoreSetAllDbEvent = exports.createSetAllPathsEvent = exports.ignoreSetAllPathsEvent = exports.askWaitGetUID = exports.initIfaceOps = void 0;
var fsm;
var bottom;
var dbglevel = window.instrustat.debuglevel;
function initIfaceOps(states, msgElem) {
    console.log('ifaceops initIfaceOps()');
    fsm = states;
    bottom = msgElem;
    if (bottom === null) {
        throw 'ifaceops initIfaceOps() no element: msgElem';
    }
}
exports.initIfaceOps = initIfaceOps;
// --------------------- setid ---------------------------
function askWaitGetUID() {
    var eventsetid = document.createEvent('Event');
    eventsetid.initEvent('setid', false, false);
    bottom.addEventListener('setid', (function (event) {
        if (dbglevel > 1)
            console.log('ifaceops askWaitGetUID() - setid() transition.');
        try {
            fsm.setid();
        }
        catch (error) {
            console.error('ifaceops askWaitGetUID() :  setid: fsm.setid() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
        var pollhascfg;
        (pollhascfg = function () {
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
                        console.error('ifaceops:  pollhascfg(): fsm.hascfg() ' +
                            'transition failed, error: ', error.message, ' current state: ', fsm.state);
                    }
                }
                else {
                    try {
                        fsm.nocfg();
                    }
                    catch (error) {
                        console.error('ifaceops: pollhascfg(): fsm.nocfg() ' +
                            'transition failed, error: ', error.message, ' current state: ', fsm.state);
                    }
                }
            }
            else {
                window.setTimeout(pollhascfg, 100);
            }
        })(); // wait until the ID has been set, or not
    })); // hey non-semicolon-TS-person - this is needed!
    window.iface.regeventsetid(bottom, eventsetid);
}
exports.askWaitGetUID = askWaitGetUID;
/*
 In case the instrument does not have any usage to an event, it still might
 be (accidentally) generated from the client C++ part. Therefore all
 instruments shall deal with all possible events, to avoid eventual crash.
 If there is no statehandler for those events, these event handlers allow
 the instrument to deal with those (eventual) events and ignore them.
 */
// --------------------- setall ---------------------------
function ignoreSetAllPathsEvent() {
    var eventsetall = document.createEvent('Event');
    eventsetall.initEvent('setall', false, false);
    bottom.addEventListener('setall', (function (event) {
        console.error('ifaceops: Event:  setall: error:  all paths not required');
    })); // hey non-semicolon-TS-person - this is needed!
    window.iface.regeventsetall(bottom, eventsetall);
}
exports.ignoreSetAllPathsEvent = ignoreSetAllPathsEvent;
function createSetAllPathsEvent() {
    var eventsetall = document.createEvent('Event');
    eventsetall.initEvent('setall', false, false);
    bottom.addEventListener('setall', (function (event) {
        try {
            fsm.setall();
        }
        catch (error) {
            console.error('ifaceops: Event:  setall: fsm.setall() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventsetall(bottom, eventsetall);
}
exports.createSetAllPathsEvent = createSetAllPathsEvent;
// --------------------- setalldb ---------------------------
function ignoreSetAllDbEvent() {
    var eventsetalldb = document.createEvent('Event');
    eventsetalldb.initEvent('setalldb', false, false);
    bottom.addEventListener('setalldb', (function (event) {
        console.error('ifaceops: Event:  setalldb: error: all db paths not required');
    })); // hey non-semicolon-TS-person - this is needed!
    window.iface.regeventsetalldb(bottom, eventsetalldb);
}
exports.ignoreSetAllDbEvent = ignoreSetAllDbEvent;
function createSetAllDbEvent() {
    var eventsetalldb = document.createEvent('Event');
    eventsetalldb.initEvent('setalldb', false, false);
    bottom.addEventListener('setalldb', (function (event) {
        try {
            fsm.setalldb();
        }
        catch (error) {
            console.error('ifaceops: Event:  setalldb: fsm.setalldb() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventsetalldb(bottom, eventsetalldb);
}
exports.createSetAllDbEvent = createSetAllDbEvent;
// --------------------- setalldb ---------------------------
function ignoreSetRescanEvent() {
    var eventrescan = document.createEvent('Event');
    eventrescan.initEvent('rescan', false, false);
    bottom.addEventListener('rescan', (function (event) {
        console.error('ifaceops: Event: setrescan: error: rescan not required');
    }));
    window.iface.regeventrescan(bottom, eventrescan);
}
exports.ignoreSetRescanEvent = ignoreSetRescanEvent;
function createSetRescanEvent() {
    var eventrescan = document.createEvent('Event');
    eventrescan.initEvent('rescan', false, false);
    bottom.addEventListener('rescan', (function (event) {
        try {
            fsm.rescan();
        }
        catch (error) {
            console.error('ifaceops: Event:  rescan: fsm.rescan() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventrescan(bottom, eventrescan);
}
exports.createSetRescanEvent = createSetRescanEvent;
// --------------------- setselected ---------------------------
function ignoreSetSelectedEvent() {
    var eventselected = document.createEvent('Event');
    eventselected.initEvent('selected', false, false);
    bottom.addEventListener('selected', (function (event) {
        console.error('ifaceops: Event: selected: error: selected path not required');
    }));
    window.iface.regeventselected(bottom, eventselected);
}
exports.ignoreSetSelectedEvent = ignoreSetSelectedEvent;
function createSetSelectedEvent() {
    var eventselected = document.createEvent('Event');
    eventselected.initEvent('selected', false, false);
    bottom.addEventListener('selected', (function (event) {
        try {
            fsm.selected();
        }
        catch (error) {
            console.error('ifaceops: Event:  selected: fsm.selected() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventselected(bottom, eventselected);
}
exports.createSetSelectedEvent = createSetSelectedEvent;
// --------------------- acksubs ---------------------------
function ignoreAckSubscriptionEvent() {
    var eventacksubs = document.createEvent('Event');
    eventacksubs.initEvent('acksubs', false, false);
    bottom.addEventListener('acksubs', (function (event) {
        console.error('ifaceops: Event: acksubs: error: not asking path subscription');
    }));
    window.iface.regeventacksubs(bottom, eventacksubs);
}
exports.ignoreAckSubscriptionEvent = ignoreAckSubscriptionEvent;
function createAckSubscriptionEvent() {
    var eventacksubs = document.createEvent('Event');
    eventacksubs.initEvent('acksubs', false, false);
    bottom.addEventListener('acksubs', (function (event) {
        try {
            fsm.acksubs();
        }
        catch (error) {
            console.error('ifaceops: Event:  acksubs: fsm.acksubs() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventacksubs(bottom, eventacksubs);
}
exports.createAckSubscriptionEvent = createAckSubscriptionEvent;
// --------------------- ackschema ---------------------------
function ignoreAckDbSchemaEvent() {
    var eventackschema = document.createEvent('Event');
    eventackschema.initEvent('ackschema', false, false);
    bottom.addEventListener('ackschema', (function (event) {
        console.error('ifaceops: Event: ackschema: error: not asking DB schema');
    }));
    window.iface.regeventackschema(bottom, eventackschema);
}
exports.ignoreAckDbSchemaEvent = ignoreAckDbSchemaEvent;
function createAckDbSchemaEvent() {
    var eventackschema = document.createEvent('Event');
    eventackschema.initEvent('ackschema', false, false);
    bottom.addEventListener('ackschema', (function (event) {
        try {
            fsm.ackschema();
        }
        catch (error) {
            console.error('ifaceops: Event:  ackschema: fsm.ackschema() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventackschema(bottom, eventackschema);
}
exports.createAckDbSchemaEvent = createAckDbSchemaEvent;
// --------------------- setgetnew ---------------------------
function ignoreSetGetNewEvent() {
    var eventgetnew = document.createEvent('Event');
    eventgetnew.initEvent('getnew', false, false);
    bottom.addEventListener('getnew', (function (event) {
        console.error('ifaceops: Event: getnew: not expected (from outside?)!');
    }));
    window.iface.regeventgetnew(bottom, eventgetnew);
}
exports.ignoreSetGetNewEvent = ignoreSetGetNewEvent;
function createSetGetNewEvent() {
    var eventgetnew = document.createEvent('Event');
    eventgetnew.initEvent('getnew', false, false);
    bottom.addEventListener('getnew', (function (event) {
        try {
            fsm.getnew();
        }
        catch (error) {
            console.error('ifaceops: Event:  getnew: fsm.getnew() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventgetnew(bottom, eventgetnew);
}
exports.createSetGetNewEvent = createSetGetNewEvent;
// --------------------- newdata ---------------------------
function ignoreNewDataEvent() {
    var eventnewdata = document.createEvent('Event');
    eventnewdata.initEvent('newdata', false, false);
    bottom.addEventListener('newdata', (function (event) {
        console.error('ifaceops: Event: newdata: not expected!');
    }));
    window.iface.regeventnewdata(bottom, eventnewdata);
}
exports.ignoreNewDataEvent = ignoreNewDataEvent;
function createNewDataEvent() {
    var eventnewdata = document.createEvent('Event');
    eventnewdata.initEvent('newdata', false, false);
    bottom.addEventListener('newdata', (function (event) {
        try {
            fsm.newdata();
        }
        catch (error) {
            console.error('ifaceops: Event:  newdata: fsm.newdata() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventnewdata(bottom, eventnewdata);
}
exports.createNewDataEvent = createNewDataEvent;
// --------------------- seterrdata ---------------------------
function ignoreSetErrDataEvent() {
    var eventerrdata = document.createEvent('Event');
    eventerrdata.initEvent('errdata', false, false);
    bottom.addEventListener('errdata', (function (event) {
        console.error('ifaceops: Event: errdata: not expected!');
    }));
    window.iface.regeventerrdata(bottom, eventerrdata);
}
exports.ignoreSetErrDataEvent = ignoreSetErrDataEvent;
function createSetErrDataEvent() {
    var eventerrdata = document.createEvent('Event');
    eventerrdata.initEvent('errdata', false, false);
    bottom.addEventListener('errdata', (function (event) {
        try {
            fsm.newdata();
        }
        catch (error) {
            console.error('ifaceops: Event:  errdata: fsm.errdata() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventerrdata(bottom, eventerrdata);
}
exports.createSetErrDataEvent = createSetErrDataEvent;
// --------------------- setchgconf ---------------------------
function ignoreSetChgConfEvent() {
    var eventchgconf = document.createEvent('Event');
    eventchgconf.initEvent('chgconf', false, false);
    bottom.addEventListener('chgconf', (function (event) {
        console.error('ifaceops: Event: chgconf: not expected!');
    }));
    window.iface.regeventchgconf(bottom, eventchgconf);
}
exports.ignoreSetChgConfEvent = ignoreSetChgConfEvent;
function createSetChgConfEvent() {
    var eventchgconf = document.createEvent('Event');
    eventchgconf.initEvent('chgconf', false, false);
    bottom.addEventListener('chgconf', (function (event) {
        try {
            fsm.chgconf();
        }
        catch (error) {
            console.error('ifaceops: Event:  chgconf: fsm.chgconf() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventchgconf(bottom, eventchgconf);
}
exports.createSetChgConfEvent = createSetChgConfEvent;
// --------------------- setretryget ---------------------------
function ignoreSetRetryGetEvent() {
    var eventretryget = document.createEvent('Event');
    eventretryget.initEvent('retryget', false, false);
    bottom.addEventListener('retryget', (function (event) {
        console.error('ifaceops: Event: chgconf: not expected!');
    }));
    window.iface.regeventretryget(bottom, eventretryget);
}
exports.ignoreSetRetryGetEvent = ignoreSetRetryGetEvent;
function createSetRetryGetEvent() {
    var eventretryget = document.createEvent('Event');
    eventretryget.initEvent('retryget', false, false);
    bottom.addEventListener('retryget', (function (event) {
        try {
            fsm.retryget();
        }
        catch (error) {
            console.error('ifaceops: Event: retryget: fsm.retryget() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventretryget(bottom, eventretryget);
}
exports.createSetRetryGetEvent = createSetRetryGetEvent;
// --------------------- setgetfeet ---------------------------
function ignoreSetGetFeetEvent() {
    var eventgetfeet = document.createEvent('Event');
    eventgetfeet.initEvent('getfeet', false, false);
    bottom.addEventListener('getfeet', (function (event) {
        console.error('ifaceops: Event: getfeet: not expected!');
    }));
    window.iface.regeventgetfeet(bottom, eventgetfeet);
}
exports.ignoreSetGetFeetEvent = ignoreSetGetFeetEvent;
function ignoreSetGetNoFeetEvent() {
    var eventnogetfeet = document.createEvent('Event');
    eventnogetfeet.initEvent('nogetfeet', false, false);
    bottom.addEventListener('nogetfeet', (function (event) {
        console.error('ifaceops: Event: nogetfeet: not expected!');
    }));
    window.iface.regeventnogetfeet(bottom, eventnogetfeet);
}
exports.ignoreSetGetNoFeetEvent = ignoreSetGetNoFeetEvent;
function createSetGetFeetEvent() {
    var eventgetfeet = document.createEvent('Event');
    eventgetfeet.initEvent('getfeet', false, false);
    bottom.addEventListener('getfeet', (function (event) {
        try {
            fsm.getfeet();
        }
        catch (error) {
            console.error('ifaceops: Event:  getfeet: fsm.getfeet() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventgetfeet(bottom, eventgetfeet);
}
exports.createSetGetFeetEvent = createSetGetFeetEvent;
function createSetGetNoFeetEvent() {
    var eventnogetfeet = document.createEvent('Event');
    eventnogetfeet.initEvent('nogetfeet', false, false);
    bottom.addEventListener('nogetfeet', (function (event) {
        try {
            fsm.nogetfeet();
        }
        catch (error) {
            console.error('ifaceops: Event:  nogetfeet: fsm.nogetfeet() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventnogetfeet(bottom, eventnogetfeet);
}
exports.createSetGetNoFeetEvent = createSetGetNoFeetEvent;
// --------------------- setchkrdy ---------------------------
function ignoreSetChkRdyEvent() {
    var eventchkrdy = document.createEvent('Event');
    eventchkrdy.initEvent('chkrdy', false, false);
    bottom.addEventListener('chkrdy', (function (event) {
        console.error('ifaceops: Event: chkrdy: not expected!');
    }));
    window.iface.regeventchkrdy(bottom, eventchkrdy);
}
exports.ignoreSetChkRdyEvent = ignoreSetChkRdyEvent;
function ignoreSetChkNoRdyEvent() {
    var eventnochkrdy = document.createEvent('Event');
    eventnochkrdy.initEvent('nochkrdy', false, false);
    bottom.addEventListener('nochkrdy', (function (event) {
        console.error('ifaceops: Event: nochkrdy: not expected!');
    }));
    window.iface.regeventnochkrdy(bottom, eventnochkrdy);
}
exports.ignoreSetChkNoRdyEvent = ignoreSetChkNoRdyEvent;
function createSetChkRdyEvent() {
    var eventchkrdy = document.createEvent('Event');
    eventchkrdy.initEvent('chkrdy', false, false);
    bottom.addEventListener('chkrdy', (function (event) {
        try {
            fsm.chkrdy();
        }
        catch (error) {
            console.error('ifaceops: Event:  chkrdy: fsm.chkrdy() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventchkrdy(bottom, eventchkrdy);
}
exports.createSetChkRdyEvent = createSetChkRdyEvent;
function createSetChkNoRdyEvent() {
    var eventnochkrdy = document.createEvent('Event');
    eventnochkrdy.initEvent('nochkrdy', false, false);
    bottom.addEventListener('nochkrdy', (function (event) {
        try {
            fsm.nochkrdy();
        }
        catch (error) {
            console.error('ifaceops: Event:  nochkrdy: fsm.nochkrdy() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventnochkrdy(bottom, eventnochkrdy);
}
exports.createSetChkNoRdyEvent = createSetChkNoRdyEvent;
// --------------------- setusersl ---------------------------
function ignoreSetUserslEvent() {
    var eventusersl = document.createEvent('Event');
    eventusersl.initEvent('usersl', false, false);
    bottom.addEventListener('usersl', (function (event) {
        console.error('ifaceops: Event: usersl: not expected!');
    }));
    window.iface.regeventusersl(bottom, eventusersl);
}
exports.ignoreSetUserslEvent = ignoreSetUserslEvent;
function ignoreSetNoUserslEvent() {
    var eventnousersl = document.createEvent('Event');
    eventnousersl.initEvent('nousersl', false, false);
    bottom.addEventListener('nousersl', (function (event) {
        console.error('ifaceops: Event: nousersl: not expected!');
    }));
    window.iface.regeventnousersl(bottom, eventnousersl);
}
exports.ignoreSetNoUserslEvent = ignoreSetNoUserslEvent;
function createSetUserslEvent() {
    var eventusersl = document.createEvent('Event');
    eventusersl.initEvent('usersl', false, false);
    bottom.addEventListener('usersl', (function (event) {
        try {
            fsm.usersl();
        }
        catch (error) {
            console.error('ifaceops: Event:  usersl: fsm.usersl() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventusersl(bottom, eventusersl);
}
exports.createSetUserslEvent = createSetUserslEvent;
function createSetNoUserslEvent() {
    var eventnousersl = document.createEvent('Event');
    eventnousersl.initEvent('nousersl', false, false);
    bottom.addEventListener('nousersl', (function (event) {
        try {
            fsm.nousersl();
        }
        catch (error) {
            console.error('ifaceops: Event:  nousersl: fsm.nousersl() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventnousersl(bottom, eventnousersl);
}
exports.createSetNoUserslEvent = createSetNoUserslEvent;
// --------------------- setmarkack ---------------------------
function ignoreSetMarkAckEvent() {
    var eventmarkack = document.createEvent('Event');
    eventmarkack.initEvent('markack', false, false);
    bottom.addEventListener('markack', (function (event) {
        console.error('ifaceops: Event: markack: not expected!');
    }));
    window.iface.regeventmarkack(bottom, eventmarkack);
}
exports.ignoreSetMarkAckEvent = ignoreSetMarkAckEvent;
function createSetMarkAckEvent() {
    var eventmarkack = document.createEvent('Event');
    eventmarkack.initEvent('markack', false, false);
    bottom.addEventListener('markack', (function (event) {
        try {
            fsm.markack();
        }
        catch (error) {
            console.error('ifaceops: Event:  markack: fsm.markack() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventmarkack(bottom, eventmarkack);
}
exports.createSetMarkAckEvent = createSetMarkAckEvent;
// --------------------- setsldataack ---------------------------
function ignoreSetDataAckEvent() {
    var eventsldataack = document.createEvent('Event');
    eventsldataack.initEvent('sldataack', false, false);
    bottom.addEventListener('sldataack', (function (event) {
        console.error('ifaceops: Event: sldataack: not expected!');
    }));
    window.iface.regeventsldataack(bottom, eventsldataack);
}
exports.ignoreSetDataAckEvent = ignoreSetDataAckEvent;
function createSetDataAckEvent() {
    var eventsldataack = document.createEvent('Event');
    eventsldataack.initEvent('sldataack', false, false);
    bottom.addEventListener('sldataack', (function (event) {
        try {
            fsm.sldataack();
        }
        catch (error) {
            console.error('ifaceops: Event:  sldataack: fsm.sldataack() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventsldataack(bottom, eventsldataack);
}
exports.createSetDataAckEvent = createSetDataAckEvent;
// --------------------- newsldata ---------------------------
function ignoreSetNewSlDataEvent() {
    var eventnewsldata = document.createEvent('Event');
    eventnewsldata.initEvent('newsldata', false, false);
    bottom.addEventListener('newsldata', (function (event) {
        console.error('ifaceops: Event: newsldata: not expected!');
    }));
    window.iface.regeventnewsldata(bottom, eventnewsldata);
}
exports.ignoreSetNewSlDataEvent = ignoreSetNewSlDataEvent;
function createSetNewSlDataEvent() {
    var eventnewsldata = document.createEvent('Event');
    eventnewsldata.initEvent('newsldata', false, false);
    bottom.addEventListener('newsldata', (function (event) {
        try {
            fsm.newsldata();
        }
        catch (error) {
            console.error('ifaceops: Event:  newsldata: fsm.newsldata() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventnewsldata(bottom, eventnewsldata);
}
exports.createSetNewSlDataEvent = createSetNewSlDataEvent;
// --------------------- setsldstopack ---------------------------
function ignoreSetSldStopEvent() {
    var eventsldstopack = document.createEvent('Event');
    eventsldstopack.initEvent('sldstopack', false, false);
    bottom.addEventListener('sldstopack', (function (event) {
        console.error('ifaceops: Event: sldstopack: not expected!');
    }));
    window.iface.regeventsldstopack(bottom, eventsldstopack);
}
exports.ignoreSetSldStopEvent = ignoreSetSldStopEvent;
function createSetSldStopEvent() {
    var eventsldstopack = document.createEvent('Event');
    eventsldstopack.initEvent('sldstopack', false, false);
    bottom.addEventListener('sldstopack', (function (event) {
        try {
            fsm.sldstopack();
        }
        catch (error) {
            console.error('ifaceops: Event:  sldstopack: fsm.sldstopack() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventsldstopack(bottom, eventsldstopack);
}
exports.createSetSldStopEvent = createSetSldStopEvent;
// --------------------- setmrkdataack ---------------------------
function ignoreSetMrkDataAckEvent() {
    var eventmrkdataack = document.createEvent('Event');
    eventmrkdataack.initEvent('mrkdataack', false, false);
    bottom.addEventListener('mrkdataack', (function (event) {
        console.error('ifaceops: Event: newmrkdata: not expected!');
    }));
    window.iface.regeventmrkdataack(bottom, eventmrkdataack);
}
exports.ignoreSetMrkDataAckEvent = ignoreSetMrkDataAckEvent;
function ignoreSetNewMrkDataEvent() {
    var eventnewmrkdata = document.createEvent('Event');
    eventnewmrkdata.initEvent('newmrkdata', false, false);
    bottom.addEventListener('newmrkdata', (function (event) {
        console.error('ifaceops: Event: newmrkdata: not expected!');
    }));
    window.iface.regeventnewmrkdata(bottom, eventnewmrkdata);
}
exports.ignoreSetNewMrkDataEvent = ignoreSetNewMrkDataEvent;
function createSetMrkDataAckEvent() {
    var eventmrkdataack = document.createEvent('Event');
    eventmrkdataack.initEvent('mrkdataack', false, false);
    bottom.addEventListener('mrkdataack', (function (event) {
        try {
            fsm.mrkdataack();
        }
        catch (error) {
            console.error('ifaceops: Event:  mrkdataack: fsm.mrkdataack() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventmrkdataack(bottom, eventmrkdataack);
}
exports.createSetMrkDataAckEvent = createSetMrkDataAckEvent;
function createSetNewMrkDataEvent() {
    var eventnewmrkdata = document.createEvent('Event');
    eventnewmrkdata.initEvent('newmrkdata', false, false);
    bottom.addEventListener('newmrkdata', (function (event) {
        try {
            fsm.newmrkdata();
        }
        catch (error) {
            console.error('ifaceops: Event:  newmrkdata: fsm.newmrkdata() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventnewmrkdata(bottom, eventnewmrkdata);
}
exports.createSetNewMrkDataEvent = createSetNewMrkDataEvent;
// ---------------- setmrkmteaack / setmrkumteack ---------------------------
function ignoreSetMarkMuteEvent() {
    var eventmrkmteaack = document.createEvent('Event');
    eventmrkmteaack.initEvent('mrkmteaack', false, false);
    bottom.addEventListener('mrkmteaack', (function (event) {
        console.error('ifaceops: Event: mrkmteaack: not expected!');
    }));
    window.iface.regeventmrkmteaack(bottom, eventmrkmteaack);
}
exports.ignoreSetMarkMuteEvent = ignoreSetMarkMuteEvent;
function ignoreSetMarkUnMuteEvent() {
    var eventmrkumteack = document.createEvent('Event');
    eventmrkumteack.initEvent('mrkumteack', false, false);
    bottom.addEventListener('mrkumteack', (function (event) {
        console.error('ifaceops: Event: mrkumteack: not expected!');
    }));
    window.iface.regeventmrkumteack(bottom, eventmrkumteack);
}
exports.ignoreSetMarkUnMuteEvent = ignoreSetMarkUnMuteEvent;
function createSetMarkMuteEvent() {
    var eventmrkmteaack = document.createEvent('Event');
    eventmrkmteaack.initEvent('mrkmteaack', false, false);
    bottom.addEventListener('mrkmteaack', (function (event) {
        try {
            fsm.mrkmteaack();
        }
        catch (error) {
            console.error('ifaceops: Event:  mrkmteaack: fsm.mrkmteaack() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventmrkmteaack(bottom, eventmrkmteaack);
}
exports.createSetMarkMuteEvent = createSetMarkMuteEvent;
function createSetMarkUnMuteEvent() {
    var eventmrkumteack = document.createEvent('Event');
    eventmrkumteack.initEvent('mrkumteack', false, false);
    bottom.addEventListener('mrkumteack', (function (event) {
        try {
            fsm.mrkumteack();
        }
        catch (error) {
            console.error('ifaceops: Event:  mrkumteack: fsm.mrkumteack() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventmrkumteack(bottom, eventmrkumteack);
}
exports.createSetMarkUnMuteEvent = createSetMarkUnMuteEvent;
// --------------------- setluminsty ---------------------------
function ignoreSetLuminosityEvent() {
    var eventluminsty = document.createEvent('Event');
    eventluminsty.initEvent('luminsty', false, false);
    bottom.addEventListener('luminsty', (function (event) {
        console.error('ifaceops: Event: swapdisp: not expected!');
    }));
    window.iface.regeventluminsty(bottom, eventluminsty);
}
exports.ignoreSetLuminosityEvent = ignoreSetLuminosityEvent;
function createSetLuminosityEvent() {
    var eventluminsty = document.createEvent('Event');
    eventluminsty.initEvent('luminsty', false, false);
    bottom.addEventListener('luminsty', (function (event) {
        try {
            fsm.luminsty();
        }
        catch (error) {
            console.error('ifaceops: Event:  luminsty: fsm.luminsty() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventluminsty(bottom, eventluminsty);
}
exports.createSetLuminosityEvent = createSetLuminosityEvent;
// --------------------- setswapdisp ---------------------------
function ignoreSetSwapDispEvent() {
    var eventswapdisp = document.createEvent('Event');
    eventswapdisp.initEvent('swapdisp', false, false);
    bottom.addEventListener('swapdisp', (function (event) {
        console.error('ifaceops: Event: swapdisp: not expected!');
    }));
    window.iface.regeventswapdisp(bottom, eventswapdisp);
}
exports.ignoreSetSwapDispEvent = ignoreSetSwapDispEvent;
function createSetSwapDispEvent() {
    var eventswapdisp = document.createEvent('Event');
    eventswapdisp.initEvent('swapdisp', false, false);
    bottom.addEventListener('swapdisp', (function (event) {
        try {
            fsm.swapdisp();
        }
        catch (error) {
            console.error('ifaceops: Event: swapdisp: fsm.swapdisp() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventswapdisp(bottom, eventswapdisp);
}
exports.createSetSwapDispEvent = createSetSwapDispEvent;
// --------------------- setclosing ---------------------------
function ignoreSetClosingEvent() {
    var eventclosing = document.createEvent('Event');
    eventclosing.initEvent('closing', false, false);
    bottom.addEventListener('closing', (function (event) {
        console.error('ifaceops: Event: closing: not served here.');
    }));
    window.iface.regeventclosing(bottom, eventclosing);
}
exports.ignoreSetClosingEvent = ignoreSetClosingEvent;
function createSetClosingEvent() {
    var eventclosing = document.createEvent('Event');
    eventclosing.initEvent('closing', false, false);
    bottom.addEventListener('closing', (function (event) {
        try {
            fsm.closing();
        }
        catch (error) {
            console.error('ifaceops: Event: closing: fsm.closing() ' +
                'transition failed, error: ', error.message, ' current state: ', fsm.state);
        }
    }));
    window.iface.regeventclosing(bottom, eventclosing);
}
exports.createSetClosingEvent = createSetClosingEvent;
