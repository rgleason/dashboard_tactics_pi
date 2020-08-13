/* $Id: statemachine.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
/*
  Visualize with GraphViz, see iface.getgraphvizout(), feed a web app
  Note: with javascript-state-machine, avoid keyword 'init' in state/transition!
*/
var dbglevel = window.instrustat.debuglevel

import StateMachine from 'javascript-state-machine'
import getLocInfo from '../../src/location'
import { initInit } from './init'
import {getidAskClient, getidClientAnswer} from './getid'
import {
    getConf, memorizeSettings, prepareConfHalt
    } from '../../src/conf'
/*
 Why this is here? Because Bootstrap v3 is not TypeScript compatible and about
 all other modules below are TypeScript the few functions Bootstrap v3 needs
 are initialized here: StateMachine happens also be only available in
 JavaScript. Please not that about all other features of Bootstraps's SASS
 version we are using are selectable in ../sass/.bootstrap.sass
 */
import tooltip from '../node_modules/bootstrap-sass/assets/javascripts/bootstrap/tooltip'
import popover from '../node_modules/bootstrap-sass/assets/javascripts/bootstrap/popover'
import modal   from '../node_modules/bootstrap-sass/assets/javascripts/bootstrap/modal'

import {
    initButtons, btmarmwButtons, btmarwButtonsInstruNoChkrdyAck,
    btmarmcButtons, btmarmedButtons, btmarmaButtons
    } from './buttons'
import {
    initCppComm,
    cppGetIsDistanceUnitFeet, cppInstruNoDistFeet, cppInstruDistFeet,
    cppCheckForInstruRdy, cppInstruNotRdy, cppInstruRdy,
    cppCheckForUserStartline, cppNoUserStartline, cppUserStartline,
    cppDropStbdMark, cppAckStbdMark, cppDropPortMark, cppAckPortMark,
    cppGetData, cppAckGetData, cppStopData, cppAckStopData
    } from './cppStartline'
import {
    initLineData, armedLineData, newLineData, quitLineData
    } from './linedata'
import {getNewLuminosity} from './css'

function dbgPrintFromTo( stateOrTransStr, lifecycle ) {
    console.log( 'racestartdash statemachine ', stateOrTransStr )
    console.log('- transition : ', lifecycle.transition)
    console.log('- from       : ', lifecycle.from)
    console.log('- to         : ', lifecycle.to)
}

export function createStateMachine() {
    return new StateMachine({
        init: 'window',
        data: {
            // Static
            uid        : '',
            conf       : null,
            perspath   : false,
            instrurdy  : false,
            feet       : false,
            gotusrsl   : false,
            stbdmark   : false,
            portmark   : false,
            // Environmental
            luminosity : 'day',
            locInfo    : getLocInfo(),
            // Functional
        },
        transitions: [
            { name: 'init',      from: 'window',   to: 'loading' },
            { name: 'loaded',    from: 'loading',  to: 'initga'  },
            { name: 'initok',    from: 'initga',   to: 'getid'   },
            { name: 'setid',     from: 'getid',    to: 'hasid'   },
            { name: 'nocfg',     from: 'hasid',    to: 'getdisf' },
            { name: 'hascfg',    from: 'hasid',    to: 'getdisf' },
            { name: 'getfeet',   from: 'getdisf',  to: 'waiting' },
            { name: 'nogetfeet', from: 'getdisf',  to: 'waiting' },
            { name: 'sldstopack',from: 'waiting',  to: 'waiting' },
            { name: 'newsldata', from: 'waiting',  to: 'waiting' },
            { name: 'btnarmw',   from: 'waiting',  to: 'isready' },
            { name: 'chkrdy',    from: 'isready',  to: 'getusrs' },
            { name: 'nochkrdy',  from: 'isready',  to: 'waiting' },
            { name: 'nousersl',  from: 'getusrs',  to: 'marking' },
            { name: 'usersl',    from: 'getusrs',  to: 'btnfade' },
            { name: 'btnportd1', from: 'marking',  to: 'oneport' },
            { name: 'btnstbdd1', from: 'marking',  to: 'onestbd' },
            { name: 'markack',   from: 'oneport',  to: 'onemark' },
            { name: 'markack',   from: 'onestbd',  to: 'onemark' },
            { name: 'btnportd2', from: 'onemark',  to: 'twoport' },
            { name: 'btnstbdd2', from: 'onemark',  to: 'twostbd' },
            { name: 'markack',   from: 'twoport',  to: 'btnfade' },
            { name: 'markack',   from: 'twostbd',  to: 'btnfade' },
            { name: 'btnfaded',  from: 'btnfade',  to: 'armed'   },
            { name: 'sldataack', from: 'armed',    to: 'armed'   },
            { name: 'newsldata', from: 'armed',    to: 'armed'   },
            { name: 'btnarmc',   from: 'marking',  to: 'waiting' },
            { name: 'btnarmc',   from: 'onemark',  to: 'waiting' },
            { name: 'btnarma',   from: 'btnfade',  to: 'waiting' },
            { name: 'btnarma',   from: 'armed',    to: 'waiting' },
            { name: 'luminsty',  from: 'waiting',  to: 'waiting' },
            { name: 'luminsty',  from: 'isready',  to: 'isready' },
            { name: 'luminsty',  from: 'getusrs',  to: 'getusrs' },
            { name: 'luminsty',  from: 'marking',  to: 'marking' },
            { name: 'luminsty',  from: 'oneport',  to: 'oneport' },
            { name: 'luminsty',  from: 'onestbd',  to: 'onestbd' },
            { name: 'luminsty',  from: 'onemark',  to: 'onemark' },
            { name: 'luminsty',  from: 'twoport',  to: 'twoport' },
            { name: 'luminsty',  from: 'twostbd',  to: 'twostbd' },
            { name: 'luminsty',  from: 'btnfade',  to: 'btnfade' },
            { name: 'luminsty',  from: 'armed',    to: 'armed'   },
            { name: 'closing',   from: 'waiting',  to: 'halt'    },
            { name: 'closing',   from: 'isready',  to: 'halt'    },
            { name: 'closing',   from: 'getusrs',  to: 'halt'    },
            { name: 'closing',   from: 'marking',  to: 'halt'    },
            { name: 'closing',   from: 'oneport',  to: 'halt'    },
            { name: 'closing',   from: 'onestbd',  to: 'halt'    },
            { name: 'closing',   from: 'onemark',  to: 'halt'    },
            { name: 'closing',   from: 'twoport',  to: 'halt'    },
            { name: 'closing',   from: 'twostbd',  to: 'halt'    },
            { name: 'closing',   from: 'btnfade',  to: 'halt'    },
            { name: 'closing',   from: 'armed',    to: 'halt'    }
        ],
        methods: {
            onWindow:   function() {
                if ( dbglevel > 0 ) console.log('onWindow() - state')
            },
            onBeforeLoaded:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeLoaded() - transition')
            },
            onLoading:  function() {
                if ( dbglevel > 0 ) console.log('onLoading() - state')
                if ( dbglevel > 1 ) console.log('uid: ', this.uid )
                if ( dbglevel > 1 ) console.log('conf: ', this.conf)
                if ( dbglevel > 1 ) console.log('locInfo: ', this.locInfo )
            },
            onInitga:   function() {
                if ( dbglevel > 0 ) console.log('onInitga() - state')
                initInit( this )
                initCppComm( this )
            },
            onGetid:    function() {
                if ( dbglevel > 0 ) console.log('onGetid() - state')
                getidAskClient()
            },
            onBeforeSetid:    function() {
                if ( dbglevel > 0 ) console.log('onSetid() - before transition')
                getidClientAnswer( this )
                if ( dbglevel > 1 ) console.log('uid : ', this.uid )
                getConf( this )
                if ( !(this.conf === null) )
                    if ( (this.conf.path === null) || ( this.conf.path === '') )
                        this.conf.path = 'racedashstart' // no data path but dummy
            },
            onHasid:    function() {
                if ( dbglevel > 0 ) console.log('onHasid() - state')
            },
            onBeforeHascfg: function( lifecycle ) {
                if ( dbglevel > 0 ) console.log('onHascfg() - before transition')
                if ( dbglevel > 2)
                    dbgPrintFromTo( 'onBeforeHascfg', lifecycle )
                this.perspath = false
            },
            onBeforeNocfg: function( lifecycle ) {
                if ( dbglevel > 0 ) console.log('onNocfg() - before transition')
                if ( dbglevel > 2)
                    dbgPrintFromTo( 'onBeforeNocfg', lifecycle )
                this.perspath = false
            },
            onGetdisf:   function() {
                if ( dbglevel > 0 ) console.log('onGetdisf() - state')
                cppGetIsDistanceUnitFeet()
                initButtons( this )
                initLineData( this )
            },
            onBeforeGetfeet:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeGetfeet() - transition')
                cppInstruDistFeet()
                initButtons( this )
                initLineData( this )
            },
            onBeforeNogetfeet:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeNogetfeet() - transition')
                cppInstruNoDistFeet()
            },
            onWaiting:   function() {
                if ( dbglevel > 0 ) console.log('onWaiting() - state')
                cppStopData()
            },
            onBeforeSldstopack:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeSldstopack() - transition')
                cppAckStopData()
            },
            onBeforeBtnarmw:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeNousersl() - transition')
                if ( this.is('waiting') )
                    cppCheckForInstruRdy()
            },
            onIsready:   function() {
                if ( dbglevel > 0 ) console.log('onIsready() - state')
            },
            onBeforeChkrdy:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeChkrdy() - transition')
                cppInstruRdy()
                cppCheckForUserStartline()
            },
            onBeforeNochkrdy:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeNochkrdy() - transition')
                cppInstruNotRdy()
                btmarwButtonsInstruNoChkrdyAck()
            },
            onGetusrs:   function() {
                if ( dbglevel > 0 ) console.log('onGetUsrs() - state')
            },
            onBeforeNousersl:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeNousersl() - transition')
                cppNoUserStartline()
                btmarmwButtons()
            },
            onBeforeUsersl:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeUsersl() - transition')
                cppUserStartline()
            },
            onMarking:   function() {
                if ( dbglevel > 0 ) console.log('onMarking() - state')
                btmarwButtonsInstruNoChkrdyAck()
            },
            onBeforeBtnarmc:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnarmc() - transition')
                btmarmcButtons( this )
            },
            onBeforeBtnportd1:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnportd1() - transition')
                cppDropPortMark()
            },
            onOneport:   function() {
                if ( dbglevel > 0 ) console.log('onOneport() - state')
            },
            onBeforeBtnstbdd1:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnstbdd1() - transition')
                cppDropStbdMark()
            },
            onOnestbd:   function() {
                if ( dbglevel > 0 ) console.log('onOnestbd() - state')
            },
            onOnemark:   function() {
                if ( dbglevel > 0 ) console.log('onOnemark() - state')
                btmarwButtonsInstruNoChkrdyAck()
            },
            onBeforeBtnportd2:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnportd2() - transition')
                cppDropPortMark()
            },
            onBeforeBtnstbdd2:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnstbdd2() - transition')
                cppDropStbdMark()
            },
            onBeforeMarkack:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeMarkack() - transition')
                if ( this.is('onestbd') || this.is('twostbd') )
                    cppAckStbdMark()
                else
                    cppAckPortMark()
            },
            onBtnfade:   function() {
                if ( dbglevel > 0 ) console.log('onBtnfade() - state')
                btmarmedButtons( this )
            },
            onBeforeBtnfaded:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnfaded() - transition')
            },
            onArmed:   function() {
                if ( dbglevel > 0 ) console.log('onArmed() - state')
                armedLineData( this )
                cppGetData()
            },
            onBeforeSldataack:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeSldataack() - transition')
                cppAckGetData()
            },
            onBeforeNewsldata:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeNewsldata() - transition')
                if ( this.is('armed') )
                    newLineData()
            },
            onBeforeBtnarma:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnarma() - transition')
                btmarmaButtons( this )
                quitLineData( this )
            },
            onBeforeLuminsty: function() {
                if ( dbglevel > 0 )
                    console.log('onLuminsty() - before transition')
                getNewLuminosity( this )
                if ( dbglevel > 1 )
                    console.log('luminosity: ', this.luminosity )
            },
            onBeforeClosing: function() {
                if ( dbglevel > 0 )
                    console.log('onClosing() - before transition')
                prepareConfHalt( this )
            }
        }
    })
}
