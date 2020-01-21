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
import initLoad from './init'
import getidAskClient from './getid'
import getidClientAnswer from './getid'
import getConf from '../../src/conf'
import getPathDefaultsIfNew from '../../src/conf'
import clearConf from '../../src/conf'
import prepareConfHalt from '../../src/conf'
import getallAskClient from './path'
import getallClientAnswer from './path'
import getpathAskClient from './path'
import gotAckCheckPath from './path'
import getpathAcknowledged from './path'
import setMenuAllPaths from '../../src/menu'
import setMenuRunTime from '../../src/menu'
import setMenuBackToLoading from '../../src/menu'
import onWaitdataFinalCheck from './data'
import showData from './data'
import clearData from './data'
import prepareDataHalt from './data'
import swapDisplay from './disp'
import getNewLuminosity from './css'

export function createStateMachine() {
    return new StateMachine({
        init: 'window',
        data: {
            // Static
            uid        : '',
            conf       : null,
            perspath   : false,
            // Environmental
            luminosity : 'day',
            locInfo    : getLocInfo(),
            // Functional
            gauge      : [],
            glastvalue : [0],
            // Signal K Paths
            path       : '',
            allpaths   : [],
            menu       : null
        },
        transitions: [
            { name: 'init',     from: 'window',   to: 'loading' },
            { name: 'loaded',   from: 'loading',  to: 'initga' },
            { name: 'initok',   from: 'initga',   to: 'getid' },
            { name: 'setid',    from: 'getid',    to: 'hasid' },
            { name: 'nocfg',    from: 'hasid',    to: 'getall' },
            { name: 'hascfg',   from: 'hasid',    to: 'getpath' },
            { name: 'setall',   from: 'getall',   to: 'showmenu' },
            { name: 'selected', from: 'showmenu', to: 'getpath' },
            { name: 'acksubs',  from: 'getpath',  to: 'waitdata' },
            { name: 'newdata',  from: 'waitdata', to: 'showdata' },
            { name: 'newdata',  from: 'showdata', to: 'showdata' },
            { name: 'chgconf',  from: 'waitdata', to: 'getall' },
            { name: 'chgconf',  from: 'showdata', to: 'getall' },
            { name: 'luminsty', from: 'getid',    to: 'getid' },
            { name: 'luminsty', from: 'hasid',    to: 'hasid' },
            { name: 'luminsty', from: 'getpath',  to: 'getpath' },
            { name: 'luminsty', from: 'waitdata', to: 'waitdata' },
            { name: 'luminsty', from: 'getall',   to: 'getall' },
            { name: 'luminsty', from: 'showmenu', to: 'showmenu' },
            { name: 'luminsty', from: 'showdata', to: 'showdata' },
            { name: 'swapdisp', from: 'showdata', to: 'showdata' },
            { name: 'closing',  from: 'showdata', to: 'halt' }
        ],
        methods: {
            onWindow:   function() {
                if ( dbglevel > 0 ) console.log('onWindow() - state')
            },
            onLoading:  function() {
                if ( dbglevel > 0 ) console.log('onLoading() - state')
                if ( dbglevel > 1 ) console.log('uid: ', this.uid )
                if ( dbglevel > 1 ) console.log('locInfo: ', this.locInfo )
                if ( dbglevel > 1 ) console.log('gauge[', this.gauge.length, ']')
                if ( dbglevel > 1 ) console.log('conf: ', this.conf)
            },
            onInitga:   function() {
                if ( dbglevel > 0 ) console.log('onInitga() - state')
                initLoad( this )
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
                if ( dbglevel > 1 ) console.log('conf: ', this.conf )
                if ( this.conf.display == 'dial' ) {
                    swapDisplay( this, 'down', false ) // re-init
                    swapDisplay( this, 'down', false )
                    swapDisplay( this, 'down', false )
                }
                else if ( this.conf.display == 'simple' ) {
                    this.conf.display = 'dial'
                    swapDisplay( this, 'down', true )
                }
                else {
                    this.conf.display = 'dial'
                    swapDisplay( this, 'up', true )
                }
            },
            onHasid:    function() {
                if ( dbglevel > 0 ) console.log('onHasid() - state')
            },
            onBeforeNocfg: function( lifecycle ) {
                if ( dbglevel > 0 ) console.log('onNocfg() - before transition')
                if ( dbglevel > 2) {
                    console.log('- transition: ', lifecycle.transition)
                    console.log('- from      : ', lifecycle.from)
                    console.log('- to        : ', lifecycle.to) 
                }
                this.perspath = false
            },
            onBeforeChgconf: function() {
                if ( dbglevel > 0 ) console.log('onChgconf() - before transition')
                setMenuBackToLoading( this )
                clearData( this )
                clearConf( this )
            },
            onGetall:   function() {
                if ( dbglevel > 0 ) console.log('onGetall() - state')
                getallAskClient()
            },
            onShowmenu:  function() {
                if ( dbglevel > 0 ) console.log('onShowmenu() - state')
                getallClientAnswer( this )
                if ( dbglevel > 1 ) console.log('allpaths: ', this.allpaths )
                setMenuAllPaths( this )
            },
            onBeforeHascfg: function( lifecycle ) {
                if ( dbglevel > 0 ) console.log('onHascfg() - before transition')
                if ( dbglevel > 2) {
                    console.log('- transition: ', lifecycle.transition)
                    console.log('- from      : ', lifecycle.from)
                    console.log('- to        : ', lifecycle.to) 
                }
                this.perspath = true
                setMenuRunTime( this )
            },
            onBeforeSelected: function() {
                if ( dbglevel > 0 ) console.log('onSelected() - before transition')
                setMenuRunTime( this )
                this.path = window.iface.getselected()
                getPathDefaultsIfNew ( this )
            },
            onGetpath:  function( lifecycle ) {
                if ( dbglevel > 0 ) console.log('onGetpath() - state')
                if ( dbglevel > 2) {
                    console.log('- transition: ', lifecycle.transition)
                    console.log('- from      : ', lifecycle.from)
                    console.log('- to        : ', lifecycle.to) 
                }
                getpathAskClient( this )
            },
            onBeforeAcksubs:   function() {
                if ( dbglevel > 0 ) console.log('onAcksubs() - before transition')
                clearData( this )
                gotAckCheckPath( this )
                getpathAcknowledged( this )
            },
            onWaitdata: function() {
                if ( dbglevel > 0 ) console.log('onWaitdata() - state')
                onWaitdataFinalCheck( this )
            },
            onBeforeNewdata: function() {
                if ( dbglevel > 0 ) console.log('onNewdata() - before transition')
                showData( this )
            },
            onShowdata: function() {
                if ( dbglevel > 0 ) console.log('onShowData() - state')
            },
            onBeforeSwapdisp: function() {
                if ( dbglevel > 0 ) console.log('onSwapdisp() - before transition')
                var kbdDir = (window.iface.getswapdisp()==1?'down':'up')
                swapDisplay( this, kbdDir, true )
            },
            onBeforeLuminsty: function() {
                if ( dbglevel > 0 ) console.log('onLuminsty() - before transition')
                getNewLuminosity( this )
                if ( dbglevel > 1 ) console.log('luminosity: ', this.luminosity )
            },
            onBeforeClosing: function() {
                if ( dbglevel > 0 ) console.log('onClosing() - before transition') 
                prepareDataHalt( this )
                prepareConfHalt( this )
            }
        }
    })
}
