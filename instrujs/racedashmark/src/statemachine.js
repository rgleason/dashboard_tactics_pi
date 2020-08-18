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

import { initButtons, startWaitData } from './buttons'
import { initCppComm, cppAckGetData } from './cppMark'
import {
    initMarkData, newMarkData, startMarkData
} from './markdata'
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
            activeroute: false,
            // Environmental
            luminosity : 'day',
            locInfo    : getLocInfo(),
            // Functional
        },
        transitions: [
            { name: 'init',       from: 'window',   to: 'loading' },
            { name: 'loaded',     from: 'loading',  to: 'initga'  },
            { name: 'initok',     from: 'initga',   to: 'getid'   },
            { name: 'setid',      from: 'getid',    to: 'hasid'   },
            { name: 'nocfg',      from: 'hasid',    to: 'getack'  },
            { name: 'hascfg',     from: 'hasid',    to: 'getack'  },
            { name: 'mrkdataack', from: 'getack',   to: 'waiting' },
            { name: 'actvrte',    from: 'waiting',  to: 'running' },
            { name: 'newmrkdata', from: 'running',  to: 'running' },
            { name: 'noroute',    from: 'running',  to: 'waiting' },
            { name: 'mrkdstopack',from: 'waiting',  to: 'waiting' },
            { name: 'luminsty',   from: 'getack',   to: 'getack'  },
            { name: 'luminsty',   from: 'waiting',  to: 'waiting' },
            { name: 'luminsty',   from: 'running',  to: 'running' },
            { name: 'closing',    from: 'getack',   to: 'halt'    },
            { name: 'closing',    from: 'waiting',  to: 'halt'    },
            { name: 'closing',    from: 'running',  to: 'halt'    },
            // ignored transitions available in the iface.js
            { name: 'setall',     from: 'halt',     to: 'halt'    },
            { name: 'acksubs',    from: 'halt',     to: 'halt'    },
            { name: 'chgconf',    from: 'halt',     to: 'halt'    },
            { name: 'getfeet',    from: 'halt',     to: 'halt'    },
            { name: 'nogetfeet',  from: 'halt',     to: 'halt'    },
            { name: 'chkrdy',     from: 'halt',     to: 'halt'    },
            { name: 'nochkrdy',   from: 'halt',     to: 'halt'    },
            { name: 'usersl',     from: 'halt',     to: 'halt'    },
            { name: 'nousersl',   from: 'halt',     to: 'halt'    },
            { name: 'markack',    from: 'halt',     to: 'halt'    },
            { name: 'sldataack',  from: 'halt',     to: 'halt'    },
            { name: 'newsldata',  from: 'halt',     to: 'halt'    },
            { name: 'sldstopack', from: 'halt',     to: 'halt'    }
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
                initMarkData( this )
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
                    dbgPrintFromTo( 'onBeforeNocfg', lifecycle )
                this.perspath = false
            },
            onBeforeNocfg: function( lifecycle ) {
                if ( dbglevel > 0 ) console.log('onNocfg() - before transition')
                if ( dbglevel > 2)
                    dbgPrintFromTo( 'onBeforeNocfg', lifecycle )
                this.perspath = false
            },
            onGetack:   function() {
                if ( dbglevel > 0 ) console.log('onGetack() - state')
                initButtons( this )
            },
            onBeforeMrkdataack:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeMrkdataack() - transition')
                cppAckGetData()
            },
            onBeforeActvrte:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeActvrte() - transition')
                startMarkData( this )
            },
            onBeforeNoroute:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeNoroute() - transition')
                startWaitData( this )
            },
            onBeforeNewmrkdata:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeNewmrkdata() - transition')
                newMarkData()
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
