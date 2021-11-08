/* $Id: sifaceops.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
// Provide TypeScript helpers to avoid code copy-paste in work with iface.js

/*eslint camelcase: ['error', {'properties': 'never'}]*/
/*eslint new-cap: ["error", { "newIsCap": false }]*/

// set 'instrumentFiniteStateMachine' of instrument's tsconfig.json 'paths'
import {StateMachine} from 'instrumentFiniteStateMachine'

var fsm: StateMachine
var bottom: HTMLElement
var dbglevel: number = (window as any).instrustat.debuglevel

export function initIfaceOps( states: StateMachine, msgElem: HTMLElement ) {
    console.log('ifaceops initIfaceOps()')
    fsm = states
    bottom = msgElem
    if (bottom === null) {
        throw 'ifaceops initIfaceOps() no element: msgElem'
    }
}

// --------------------- setid ---------------------------

export function askWaitGetUID( ) {
    var eventsetid: Event = document.createEvent('Event')
    eventsetid.initEvent('setid', false, false)
    bottom.addEventListener('setid', ((event: CustomEvent) => {
        if ( dbglevel > 1 )
            console.log('ifaceops askWaitGetUID() - setid() transition.')
        try {
            fsm.setid()
        }
        catch( error ) {
            console.error(
                'ifaceops askWaitGetUID() :  setid: fsm.setid() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
        var pollhascfg: () => void
        (pollhascfg= function () {
            if ( dbglevel > 1 )
                console.log('pollhascfg() - waiting on "hasid" state')
            if ( fsm.is('hasid') ) {
                var hascfg = true
                if ( fsm.conf === null )
                    hascfg = false
                else if ( (fsm.conf.path === null) || (fsm.conf.path === '') )
                    hascfg = false
                if ( hascfg ) {
                    try {
                        fsm.hascfg()
                    }
                    catch( error ) {
                        console.error(
                            'ifaceops:  pollhascfg(): fsm.hascfg() ' +
                            'transition failed, error: ',
                            error.message, ' current state: ', fsm.state)
                    }
                }
                else {
                    try {
                        fsm.nocfg()
                    }
                    catch( error ) {
                        console.error(
                            'ifaceops: pollhascfg(): fsm.nocfg() ' +
                            'transition failed, error: ',
                            error.message, ' current state: ', fsm.state)
                    }
                }
            }
            else {
                window.setTimeout(pollhascfg, 100)
            }
        })() // wait until the ID has been set, or not
    }) as EventListener);  // hey non-semicolon-TS-person - this is needed!
    (window as any).iface.regeventsetid( bottom, eventsetid )
}
/*
 In case the instrument does not have any usage to an event, it still might
 be (accidentally) generated from the client C++ part. Therefore all
 instruments shall deal with all possible events, to avoid eventual crash.
 If there is no statehandler for those events, these event handlers allow
 the instrument to deal with those (eventual) events and ignore them.
 */

// --------------------- setall ---------------------------

export function ignoreSetAllPathsEvent() {
    var eventsetall: Event = document.createEvent('Event')
    eventsetall.initEvent('setall', false, false)
    bottom.addEventListener('setall', ((event: Event) => {
        console.error(
            'ifaceops: Event:  setall: error:  all paths not required')
    }) as EventListener);  // hey non-semicolon-TS-person - this is needed!
    (window as any).iface.regeventsetall( bottom, eventsetall )
}

export function createSetAllPathsEvent() {
    var eventsetall: Event = document.createEvent('Event')
    eventsetall.initEvent('setall', false, false)
    bottom.addEventListener('setall', ((event: Event) => {
        try {
            fsm.setall()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  setall: fsm.setall() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventsetall( bottom, eventsetall )
}

// --------------------- setalldb ---------------------------

export function ignoreSetAllDbEvent() {
    var eventsetalldb: Event = document.createEvent('Event')
    eventsetalldb.initEvent('setalldb', false, false)
    bottom.addEventListener('setalldb', ((event: Event) => {
        console.error(
            'ifaceops: Event:  setalldb: error: all db paths not required')
    }) as EventListener);  // hey non-semicolon-TS-person - this is needed!
    (window as any).iface.regeventsetalldb( bottom, eventsetalldb )
}

export function createSetAllDbEvent() {
    var eventsetalldb: Event = document.createEvent('Event')
    eventsetalldb.initEvent('setalldb', false, false)
    bottom.addEventListener('setalldb', ((event: Event) => {
        try {
            fsm.setalldb()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  setalldb: fsm.setalldb() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventsetalldb( bottom, eventsetalldb )
}

// --------------------- setalldb ---------------------------

export function ignoreSetRescanEvent() {
    var eventrescan: Event = document.createEvent('Event')
    eventrescan.initEvent('rescan', false, false)
    bottom.addEventListener('rescan', ((event: Event) => {
        console.error(
            'ifaceops: Event: setrescan: error: rescan not required')
    }) as EventListener);
    (window as any).iface.regeventrescan( bottom, eventrescan )
}

export function createSetRescanEvent() {
    var eventrescan: Event = document.createEvent('Event')
    eventrescan.initEvent('rescan', false, false)
    bottom.addEventListener('rescan', ((event: Event) => {
        try {
            fsm.rescan()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  rescan: fsm.rescan() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventrescan( bottom, eventrescan )
}

// --------------------- setselected ---------------------------

export function ignoreSetSelectedEvent() {
    var eventselected: Event = document.createEvent('Event')
    eventselected.initEvent('selected', false, false)
    bottom.addEventListener('selected', ((event: Event) => {
        console.error(
            'ifaceops: Event: selected: error: selected path not required')
    }) as EventListener);
    (window as any).iface.regeventselected( bottom, eventselected )
}

export function createSetSelectedEvent() {
    var eventselected: Event = document.createEvent('Event')
    eventselected.initEvent('selected', false, false)
    bottom.addEventListener('selected', ((event: Event) => {
        try {
            fsm.selected()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  selected: fsm.selected() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventselected( bottom, eventselected )
}

// --------------------- acksubs ---------------------------

export function ignoreAckSubscriptionEvent() {
    var eventacksubs: Event = document.createEvent('Event')
    eventacksubs.initEvent('acksubs', false, false)
    bottom.addEventListener('acksubs', ((event: Event) => {
        console.error(
            'ifaceops: Event: acksubs: error: not asking path subscription')
    }) as EventListener);
    (window as any).iface.regeventacksubs( bottom, eventacksubs )
}

export function createAckSubscriptionEvent() {
    var eventacksubs: Event = document.createEvent('Event')
    eventacksubs.initEvent('acksubs', false, false)
    bottom.addEventListener('acksubs', ((event: Event) => {
        try {
            fsm.acksubs()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  acksubs: fsm.acksubs() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventacksubs( bottom, eventacksubs )
}

// --------------------- ackschema ---------------------------

export function ignoreAckDbSchemaEvent() {
    var eventackschema: Event = document.createEvent('Event')
    eventackschema.initEvent('ackschema', false, false)
    bottom.addEventListener('ackschema', ((event: Event) => {
        console.error(
            'ifaceops: Event: ackschema: error: not asking DB schema')
    }) as EventListener);
    (window as any).iface.regeventackschema( bottom, eventackschema )
}

export function createAckDbSchemaEvent() {
    var eventackschema: Event = document.createEvent('Event')
    eventackschema.initEvent('ackschema', false, false)
    bottom.addEventListener('ackschema', ((event: Event) => {
        try {
            fsm.ackschema()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  ackschema: fsm.ackschema() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventackschema( bottom, eventackschema )
}

// --------------------- setgetnew ---------------------------

export function ignoreSetGetNewEvent() {
    var eventgetnew: Event = document.createEvent('Event')
    eventgetnew.initEvent('getnew', false, false)
    bottom.addEventListener('getnew', ((event: Event) => {
        console.error(
                'ifaceops: Event: getnew: not expected (from outside?)!')
    }) as EventListener);
    (window as any).iface.regeventgetnew( bottom, eventgetnew )
}

export function createSetGetNewEvent() {
    var eventgetnew: Event = document.createEvent('Event')
    eventgetnew.initEvent('getnew', false, false)
    bottom.addEventListener('getnew', ((event: Event) => {
        try {
            fsm.getnew()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  getnew: fsm.getnew() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventgetnew( bottom, eventgetnew )
}

// --------------------- newdata ---------------------------

export function ignoreNewDataEvent() {
    var eventnewdata: Event = document.createEvent('Event')
    eventnewdata.initEvent('newdata', false, false)
    bottom.addEventListener('newdata', ((event: Event) => {
        console.error(
                'ifaceops: Event: newdata: not expected!')
    }) as EventListener);
    (window as any).iface.regeventnewdata( bottom, eventnewdata )
}

export function createNewDataEvent() {
    var eventnewdata: Event = document.createEvent('Event')
    eventnewdata.initEvent('newdata', false, false)
    bottom.addEventListener('newdata', ((event: Event) => {
        try {
            fsm.newdata()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  newdata: fsm.newdata() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventnewdata( bottom, eventnewdata )
}

// --------------------- seterrdata ---------------------------

export function ignoreSetErrDataEvent() {
    var eventerrdata: Event = document.createEvent('Event')
    eventerrdata.initEvent('errdata', false, false)
    bottom.addEventListener('errdata', ((event: Event) => {
        console.error(
                'ifaceops: Event: errdata: not expected!')
    }) as EventListener);
    (window as any).iface.regeventerrdata( bottom, eventerrdata )
}

export function createSetErrDataEvent() {
    var eventerrdata: Event = document.createEvent('Event')
    eventerrdata.initEvent('errdata', false, false)
    bottom.addEventListener('errdata', ((event: Event) => {
        try {
            fsm.newdata()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  errdata: fsm.errdata() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventerrdata( bottom, eventerrdata )
}

// --------------------- setchgconf ---------------------------

export function ignoreSetChgConfEvent() {
    var eventchgconf: Event = document.createEvent('Event')
    eventchgconf.initEvent('chgconf', false, false)
    bottom.addEventListener('chgconf', ((event: Event) => {
        console.error(
                'ifaceops: Event: chgconf: not expected!')
    }) as EventListener);
    (window as any).iface.regeventchgconf( bottom, eventchgconf )
}

export function createSetChgConfEvent() {
    var eventchgconf: Event = document.createEvent('Event')
    eventchgconf.initEvent('chgconf', false, false)
    bottom.addEventListener('chgconf', ((event: Event) => {
        try {
            fsm.chgconf()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  chgconf: fsm.chgconf() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventchgconf( bottom, eventchgconf )
}

// --------------------- setretryget ---------------------------

export function ignoreSetRetryGetEvent() {
    var eventretryget: Event = document.createEvent('Event')
    eventretryget.initEvent('retryget', false, false)
    bottom.addEventListener('retryget', ((event: Event) => {
        console.error(
                'ifaceops: Event: chgconf: not expected!')
    }) as EventListener);
    (window as any).iface.regeventretryget( bottom, eventretryget )
}

export function createSetRetryGetEvent() {
    var eventretryget: Event = document.createEvent('Event')
    eventretryget.initEvent('retryget', false, false)
    bottom.addEventListener('retryget', ((event: Event) => {
        try {
            fsm.retryget()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event: retryget: fsm.retryget() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventretryget( bottom, eventretryget )
}

// --------------------- setgetfeet ---------------------------

export function ignoreSetGetFeetEvent() {
    var eventgetfeet: Event = document.createEvent('Event')
    eventgetfeet.initEvent('getfeet', false, false)
    bottom.addEventListener('getfeet', ((event: Event) => {
        console.error(
                'ifaceops: Event: getfeet: not expected!')
    }) as EventListener);
    (window as any).iface.regeventgetfeet( bottom, eventgetfeet )
}

export function ignoreSetGetNoFeetEvent() {
    var eventnogetfeet: Event = document.createEvent('Event')
    eventnogetfeet.initEvent('nogetfeet', false, false)
    bottom.addEventListener('nogetfeet', ((event: Event) => {
        console.error(
                'ifaceops: Event: nogetfeet: not expected!')
    }) as EventListener);
    (window as any).iface.regeventnogetfeet( bottom, eventnogetfeet )
}

export function createSetGetFeetEvent() {
    var eventgetfeet: Event = document.createEvent('Event')
    eventgetfeet.initEvent('getfeet', false, false)
    bottom.addEventListener('getfeet', ((event: Event) => {
        try {
            fsm.getfeet()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  getfeet: fsm.getfeet() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventgetfeet( bottom, eventgetfeet )
}

export function createSetGetNoFeetEvent() {
    var eventnogetfeet: Event = document.createEvent('Event')
    eventnogetfeet.initEvent('nogetfeet', false, false)
    bottom.addEventListener('nogetfeet', ((event: Event) => {
        try {
            fsm.nogetfeet()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  nogetfeet: fsm.nogetfeet() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventnogetfeet( bottom, eventnogetfeet )
}

// --------------------- setchkrdy ---------------------------

export function ignoreSetChkRdyEvent() {
    var eventchkrdy: Event = document.createEvent('Event')
    eventchkrdy.initEvent('chkrdy', false, false)
    bottom.addEventListener('chkrdy', ((event: Event) => {
        console.error(
                'ifaceops: Event: chkrdy: not expected!')
    }) as EventListener);
    (window as any).iface.regeventchkrdy( bottom, eventchkrdy )
}

export function ignoreSetChkNoRdyEvent() {
    var eventnochkrdy: Event = document.createEvent('Event')
    eventnochkrdy.initEvent('nochkrdy', false, false)
    bottom.addEventListener('nochkrdy', ((event: Event) => {
        console.error(
                'ifaceops: Event: nochkrdy: not expected!')
    }) as EventListener);
    (window as any).iface.regeventnochkrdy( bottom, eventnochkrdy )
}

export function createSetChkRdyEvent() {
    var eventchkrdy: Event = document.createEvent('Event')
    eventchkrdy.initEvent('chkrdy', false, false)
    bottom.addEventListener('chkrdy', ((event: Event) => {
        try {
            fsm.chkrdy()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  chkrdy: fsm.chkrdy() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventchkrdy( bottom, eventchkrdy )
}

export function createSetChkNoRdyEvent() {
    var eventnochkrdy: Event = document.createEvent('Event')
    eventnochkrdy.initEvent('nochkrdy', false, false)
    bottom.addEventListener('nochkrdy', ((event: Event) => {
        try {
            fsm.nochkrdy()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  nochkrdy: fsm.nochkrdy() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventnochkrdy( bottom, eventnochkrdy )
}

// --------------------- setusersl ---------------------------

export function ignoreSetUserslEvent() {
    var eventusersl: Event = document.createEvent('Event')
    eventusersl.initEvent('usersl', false, false)
    bottom.addEventListener('usersl', ((event: Event) => {
        console.error(
                'ifaceops: Event: usersl: not expected!')
    }) as EventListener);
    (window as any).iface.regeventusersl( bottom, eventusersl )
}

export function ignoreSetNoUserslEvent() {
    var eventnousersl: Event = document.createEvent('Event')
    eventnousersl.initEvent('nousersl', false, false)
    bottom.addEventListener('nousersl', ((event: Event) => {
        console.error(
                'ifaceops: Event: nousersl: not expected!')
    }) as EventListener);
    (window as any).iface.regeventnousersl( bottom, eventnousersl )
}

export function createSetUserslEvent() {
    var eventusersl: Event = document.createEvent('Event')
    eventusersl.initEvent('usersl', false, false)
    bottom.addEventListener('usersl', ((event: Event) => {
        try {
            fsm.usersl()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  usersl: fsm.usersl() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventusersl( bottom, eventusersl )
}

export function createSetNoUserslEvent() {
    var eventnousersl: Event = document.createEvent('Event')
    eventnousersl.initEvent('nousersl', false, false)
    bottom.addEventListener('nousersl', ((event: Event) => {
        try {
            fsm.nousersl()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  nousersl: fsm.nousersl() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventnousersl( bottom, eventnousersl )
}

// --------------------- setmarkack ---------------------------

export function ignoreSetMarkAckEvent() {
    var eventmarkack: Event = document.createEvent('Event')
    eventmarkack.initEvent('markack', false, false)
    bottom.addEventListener('markack', ((event: Event) => {
        console.error(
                'ifaceops: Event: markack: not expected!')
    }) as EventListener);
    (window as any).iface.regeventmarkack( bottom, eventmarkack )
}

export function createSetMarkAckEvent() {
    var eventmarkack: Event = document.createEvent('Event')
    eventmarkack.initEvent('markack', false, false)
    bottom.addEventListener('markack', ((event: Event) => {
        try {
            fsm.markack()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  markack: fsm.markack() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventmarkack( bottom, eventmarkack )
}

// --------------------- setsldataack ---------------------------

export function ignoreSetDataAckEvent() {
    var eventsldataack: Event = document.createEvent('Event')
    eventsldataack.initEvent('sldataack', false, false)
    bottom.addEventListener('sldataack', ((event: Event) => {
        console.error(
                'ifaceops: Event: sldataack: not expected!')
    }) as EventListener);
    (window as any).iface.regeventsldataack( bottom, eventsldataack )
}

export function createSetDataAckEvent() {
    var eventsldataack: Event = document.createEvent('Event')
    eventsldataack.initEvent('sldataack', false, false)
    bottom.addEventListener('sldataack', ((event: Event) => {
        try {
            fsm.sldataack()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  sldataack: fsm.sldataack() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventsldataack( bottom, eventsldataack )
}

// --------------------- newsldata ---------------------------

export function ignoreSetNewSlDataEvent() {
    var eventnewsldata: Event = document.createEvent('Event')
    eventnewsldata.initEvent('newsldata', false, false)
    bottom.addEventListener('newsldata', ((event: Event) => {
        console.error(
                'ifaceops: Event: newsldata: not expected!')
    }) as EventListener);
    (window as any).iface.regeventnewsldata( bottom, eventnewsldata )
}

export function createSetNewSlDataEvent() {
    var eventnewsldata: Event = document.createEvent('Event')
    eventnewsldata.initEvent('newsldata', false, false)
    bottom.addEventListener('newsldata', ((event: Event) => {
        try {
            fsm.newsldata()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  newsldata: fsm.newsldata() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventnewsldata( bottom, eventnewsldata )
}

// --------------------- setsldstopack ---------------------------

export function ignoreSetSldStopEvent() {
    var eventsldstopack: Event = document.createEvent('Event')
    eventsldstopack.initEvent('sldstopack', false, false)
    bottom.addEventListener('sldstopack', ((event: Event) => {
        console.error(
                'ifaceops: Event: sldstopack: not expected!')
    }) as EventListener);
    (window as any).iface.regeventsldstopack( bottom, eventsldstopack )
}

export function createSetSldStopEvent() {
    var eventsldstopack: Event = document.createEvent('Event')
    eventsldstopack.initEvent('sldstopack', false, false)
    bottom.addEventListener('sldstopack', ((event: Event) => {
        try {
            fsm.sldstopack()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  sldstopack: fsm.sldstopack() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventsldstopack( bottom, eventsldstopack )
}

// --------------------- setmrkdataack ---------------------------

export function ignoreSetMrkDataAckEvent() {
    var eventmrkdataack: Event = document.createEvent('Event')
    eventmrkdataack.initEvent('mrkdataack', false, false)
    bottom.addEventListener('mrkdataack', ((event: Event) => {
        console.error(
                'ifaceops: Event: newmrkdata: not expected!')
    }) as EventListener);
    (window as any).iface.regeventmrkdataack( bottom, eventmrkdataack )
}

export function ignoreSetNewMrkDataEvent() {
    var eventnewmrkdata: Event = document.createEvent('Event')
    eventnewmrkdata.initEvent('newmrkdata', false, false)
    bottom.addEventListener('newmrkdata', ((event: Event) => {
        console.error(
                'ifaceops: Event: newmrkdata: not expected!')
    }) as EventListener);
    (window as any).iface.regeventnewmrkdata( bottom, eventnewmrkdata )
}

export function createSetMrkDataAckEvent() {
    var eventmrkdataack: Event = document.createEvent('Event')
    eventmrkdataack.initEvent('mrkdataack', false, false)
    bottom.addEventListener('mrkdataack', ((event: Event) => {
        try {
            fsm.mrkdataack()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  mrkdataack: fsm.mrkdataack() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventmrkdataack( bottom, eventmrkdataack )
}

export function createSetNewMrkDataEvent() {
    var eventnewmrkdata: Event = document.createEvent('Event')
    eventnewmrkdata.initEvent('newmrkdata', false, false)
    bottom.addEventListener('newmrkdata', ((event: Event) => {
        try {
            fsm.newmrkdata()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  newmrkdata: fsm.newmrkdata() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventnewmrkdata( bottom, eventnewmrkdata )
}

// ---------------- setmrkmteaack / setmrkumteack ---------------------------

export function ignoreSetMarkMuteEvent() {
    var eventmrkmteaack: Event = document.createEvent('Event')
    eventmrkmteaack.initEvent('mrkmteaack', false, false)
    bottom.addEventListener('mrkmteaack', ((event: Event) => {
        console.error(
                'ifaceops: Event: mrkmteaack: not expected!')
    }) as EventListener);
    (window as any).iface.regeventmrkmteaack( bottom, eventmrkmteaack )
}

export function ignoreSetMarkUnMuteEvent() {
    var eventmrkumteack: Event = document.createEvent('Event')
    eventmrkumteack.initEvent('mrkumteack', false, false)
    bottom.addEventListener('mrkumteack', ((event: Event) => {
        console.error(
                'ifaceops: Event: mrkumteack: not expected!')
    }) as EventListener);
    (window as any).iface.regeventmrkumteack( bottom, eventmrkumteack )
}

export function createSetMarkMuteEvent() {
    var eventmrkmteaack: Event = document.createEvent('Event')
    eventmrkmteaack.initEvent('mrkmteaack', false, false)
    bottom.addEventListener('mrkmteaack', ((event: Event) => {
        try {
            fsm.mrkmteaack()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  mrkmteaack: fsm.mrkmteaack() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventmrkmteaack( bottom, eventmrkmteaack )
}

export function createSetMarkUnMuteEvent() {
    var eventmrkumteack: Event = document.createEvent('Event')
    eventmrkumteack.initEvent('mrkumteack', false, false)
    bottom.addEventListener('mrkumteack', ((event: Event) => {
        try {
            fsm.mrkumteack()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  mrkumteack: fsm.mrkumteack() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventmrkumteack( bottom, eventmrkumteack )
}

// --------------------- setluminsty ---------------------------

export function ignoreSetLuminosityEvent() {
    var eventluminsty: Event = document.createEvent('Event')
    eventluminsty.initEvent('luminsty', false, false)
    bottom.addEventListener('luminsty', ((event: Event) => {
        console.error(
                'ifaceops: Event: swapdisp: not expected!')
    }) as EventListener);
    (window as any).iface.regeventluminsty( bottom, eventluminsty )
}

export function createSetLuminosityEvent() {
    var eventluminsty: Event = document.createEvent('Event')
    eventluminsty.initEvent('luminsty', false, false)
    bottom.addEventListener('luminsty', ((event: Event) => {
        try {
            fsm.luminsty()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event:  luminsty: fsm.luminsty() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventluminsty( bottom, eventluminsty )
}

// --------------------- setswapdisp ---------------------------

export function ignoreSetSwapDispEvent() {
    var eventswapdisp: Event = document.createEvent('Event')
    eventswapdisp.initEvent('swapdisp', false, false)
    bottom.addEventListener('swapdisp', ((event: Event) => {
        console.error(
                'ifaceops: Event: swapdisp: not expected!')
    }) as EventListener);
    (window as any).iface.regeventswapdisp( bottom, eventswapdisp )
}

export function createSetSwapDispEvent() {
    var eventswapdisp: Event = document.createEvent('Event')
    eventswapdisp.initEvent('swapdisp', false, false)
    bottom.addEventListener('swapdisp', ((event: Event) => {
        try {
            fsm.swapdisp()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event: swapdisp: fsm.swapdisp() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventswapdisp( bottom, eventswapdisp )
}

// --------------------- setclosing ---------------------------

export function ignoreSetClosingEvent() {
    var eventclosing: Event = document.createEvent('Event')
    eventclosing.initEvent('closing', false, false)
    bottom.addEventListener('closing', ((event: Event) => {
        console.error(
                'ifaceops: Event: closing: not served here.')
    }) as EventListener);
    (window as any).iface.regeventclosing( bottom, eventclosing )
}

export function createSetClosingEvent() {
    var eventclosing: Event = document.createEvent('Event')
    eventclosing.initEvent('closing', false, false)
    bottom.addEventListener('closing', ((event: Event) => {
        try {
            fsm.closing()
        }
        catch( error ) {
            console.error(
                'ifaceops: Event: closing: fsm.closing() ' +
                'transition failed, error: ',
                error.message, ' current state: ', fsm.state)
        }
    }) as EventListener);
    (window as any).iface.regeventclosing( bottom, eventclosing )
}
