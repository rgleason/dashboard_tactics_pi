/* $Id: statemachine.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
/*
  Visualize with https://xstate.js.org/viz/ current schema https://git.io/JexUj
  (not implementable with 'xstate', it is not working on IE :( )
  Note: with javascript-state-machine, avoid keyword 'init' in state/transition!
*/
var dbglevel = window.instrustat.debuglevel

import StateMachine from 'javascript-state-machine';
import getLocInfo from '../../src/location'
import { initLoad } from './init'
import { getidAskClient, getidClientAnswer, getidConf } from './getid'
import { getallAskClient, getallClientAnswer, getpathAskClient } from './path'
import { setMenuAllPaths, setMenuRunTime, setMenuBackToLoading } from '../../src/menu'
import { showData, clearData } from './data'
import { getNewLuminosity } from './css'

// import { loadConf } from '../../src/persistence'

export function createStateMachine() {
    return new StateMachine({
        init: 'window',
        data: {
            // Static
            uid        : '',
            conf       : null,
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
            { name: 'fetch',    from: 'window',   to: 'loading' },
            { name: 'loaded',   from: 'loading',  to: 'initga' },
            { name: 'initok',   from: 'initga',   to: 'getid' },
            { name: 'setid',    from: 'getid',    to: 'getid' },
            { name: 'nocfg',    from: 'getid',    to: 'getall' },
            { name: 'hascfg',   from: 'getid',    to: 'getpath' },
            { name: 'setall',   from: 'getall',   to: 'showmenu' },
            { name: 'selected', from: 'showmenu', to: 'getpath' },
            { name: 'newdata',  from: 'getpath',  to: 'showdata' },
            { name: 'newdata',  from: 'showdata', to: 'showdata' },
            { name: 'chgconf',  from: 'showdata', to: 'getall' },
            { name: 'luminsty', from: 'getid',    to: 'getid' },
            { name: 'luminsty', from: 'getpath',  to: 'getpath' },
            { name: 'luminsty', from: 'getall',   to: 'getall' },
            { name: 'luminsty', from: 'showmenu', to: 'showmenu' },
            { name: 'luminsty', from: 'showdata', to: 'showdata' }
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
                if ( dbglevel > 1 ) console.log('uid: ', this.uid )
                if ( dbglevel > 1 ) console.log('locInfo: ', this.locInfo )
                if ( dbglevel > 1 ) console.log('gauge[', this.gauge.length, ']')
                if ( dbglevel > 1 ) console.log('conf: ', this.conf)
            },
            onGetid:    function() {
                if ( dbglevel > 0 ) console.log('onGetid() - state')
                getidAskClient()
            },
            onSetid:    function() {
                if ( dbglevel > 0 ) console.log('onSetid() - transition')
                getidClientAnswer( this )
                if ( dbglevel > 1 ) console.log('uid : ', this.uid )
                getidConf( this )
                // this.conf = loadConf( this.uid, this.locInfo.protocol )
                if ( dbglevel > 1 ) console.log('conf: ', this.conf )
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
            onSelected: function() {
                if ( dbglevel > 0 ) console.log('onSelected() - transition')
                setMenuRunTime( this )
            },
            onChgconf: function() {
                if ( dbglevel > 0 ) console.log('onChgconf() - transition')
                setMenuBackToLoading( this )
                clearData( this )
            },
            onGetpath:  function() {
                if ( dbglevel > 0 ) console.log('onGetpath() - state')
                getpathAskClient( this )
            },
            onNewdata: function() {
                if ( dbglevel > 0 ) console.log('onNewdata() - transition')
                showData( this )
            },
            onShowdata: function() {
                if ( dbglevel > 0 ) console.log('onShowData() - state')
            },
            onLuminsty: function() {
                if ( dbglevel > 0 ) console.log('onLuminsty() - transition')
                getNewLuminosity( this )
                if ( dbglevel > 1 ) console.log('luminosity: ', this.luminosity )
            }
        }
    })
}
