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
import {initLoad} from './init'
import {getidAskClient, getidClientAnswer} from './getid'
import {getConf, getPathDefaultsIfNew, clearConf, prepareConfHalt} from '../../src/conf'
import {getalldbAskClient, getalldbClientAnswer, getpathAskClient, gotAckCheckPath, getpathAcknowledged} from './path'
import {setMenuAllPaths, setMenuRunTime, setMenuBackToLoading} from '../../src/menu'
import {onWaitdataFinalCheck, showData, clearData, prepareDataHalt} from './data'
import {swapDisplay, rollDisplayToSelection} from './disp'
import {getNewLuminosity} from './css'

function dbgPrintFromTo( stateOrTransStr, lifecycle ) {
    console.log( stateOrTransStr )
    console.log('- transition: ', lifecycle.transition)
    console.log('- from      : ', lifecycle.from)
    console.log('- to        : ', lifecycle.to)
}

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
            chart      : [],
            // Signal K Paths
            path       : '',
            allpaths   : [],
            menu       : null
        },
        transitions: [
            { name: 'init',      from: 'window',   to: 'loading' },
            { name: 'loaded',    from: 'loading',  to: 'initga' },
            { name: 'initok',    from: 'initga',   to: 'getid' },
            { name: 'setid',     from: 'getid',    to: 'hasid' },
            { name: 'nocfg',     from: 'hasid',    to: 'getalldb' },
            { name: 'hascfg',    from: 'hasid',    to: 'getschema' },
            { name: 'setalldb',  from: 'getalldb', to: 'showmenu' },
            { name: 'rescan',    from: 'showmenu', to: 'getalldb' },
            { name: 'selected',  from: 'showmenu', to: 'getschema' },
            { name: 'ackschema', from: 'getschema',to: 'getdata' },
            { name: 'getnew',    from: 'showdata', to: 'getdata' },
            { name: 'getlaunch', from: 'getdata',  to: 'waitdata' },
            { name: 'newdata',   from: 'waitdata', to: 'showdata' },
            { name: 'errdata',   from: 'waitdata', to: 'nodata' },
            { name: 'retryget',  from: 'nodata',   to: 'getdata' },
            { name: 'chgconf',   from: 'waitdata', to: 'getalldb' },
            { name: 'chgconf',   from: 'showdata', to: 'getalldb' },
            { name: 'chgconf',   from: 'nodata',   to: 'getalldb' },
            { name: 'luminsty',  from: 'getid',    to: 'getid' },
            { name: 'luminsty',  from: 'hasid',    to: 'hasid' },
            { name: 'luminsty',  from: 'getschema',to: 'getschema' },
            { name: 'luminsty',  from: 'getalldb', to: 'getalldb' },
            { name: 'luminsty',  from: 'getdata',  to: 'getdata' },
            { name: 'luminsty',  from: 'waitdata', to: 'waitdata' },
            { name: 'luminsty',  from: 'nodata',   to: 'nodata' },
            { name: 'luminsty',  from: 'showmenu', to: 'showmenu' },
            { name: 'luminsty',  from: 'showdata', to: 'showdata' },
            { name: 'closing',   from: 'waitdata', to: 'halt' },
            { name: 'closing',   from: 'showdata', to: 'halt' }
        ],
        methods: {
            onWindow:   function() {
                if ( dbglevel > 0 ) console.log('onWindow() - state')
            },
            onLoading:  function() {
                if ( dbglevel > 0 ) console.log('onLoading() - state')
                if ( dbglevel > 1 ) console.log('uid: ', this.uid )
                if ( dbglevel > 1 ) console.log('locInfo: ', this.locInfo )
                if ( dbglevel > 1 ) console.log('chart[', this.chart.length, ']')
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
                rollDisplayToSelection( this )
            },
            onHasid:    function() {
                if ( dbglevel > 0 ) console.log('onHasid() - state')
            },
            onBeforeNocfg: function( lifecycle ) {
                if ( dbglevel > 0 ) console.log('onNocfg() - before transition')
                if ( dbglevel > 2)
                    dbgPrintFromTo( 'onBeforeNocfg', lifecycle )
                this.perspath = false
            },
            onBeforeChgconf: function() {
                if ( dbglevel > 0 ) console.log('onChgconf() - before transition')
                setMenuBackToLoading( this )
                clearData( this )
                clearConf( this )
            },
            onGetalldb:   function() {
                if ( dbglevel > 0 ) console.log('onGetalldb() - state')
                getalldbAskClient()
            },
            onShowmenu:  function() {
                if ( dbglevel > 0 ) console.log('onShowmenu() - state')
                getalldbClientAnswer( this )
                if ( dbglevel > 1 ) console.log('allpaths: ', this.allpaths )
                setMenuAllPaths( this )
            },
            onBeforeRetryget: function( lifecycle ) {
                if ( dbglevel > 0 ) console.log('onRetryget() - before transition')
                if ( dbglevel > 2)
                    dbgPrintFromTo( 'onBeforeRetryget', lifecycle )
                setMenuBackToLoading( this )
            },
            onBeforeHascfg: function( lifecycle ) {
                dbgPrintFromTo( 'onHascfg() - before transition', lifecycle )
                this.perspath = true
                setMenuRunTime( this )
            },
            onBeforeSelected: function() {
                if ( dbglevel > 0 ) console.log('onSelected() - before transition')
                setMenuRunTime( this )
                this.path = window.iface.getselected()
                getPathDefaultsIfNew ( this )
            },
            onGetschema:  function( lifecycle ) {
                dbgPrintFromTo( 'onGetschema() - state', lifecycle )
                getpathAskClient( this )
            },
            onBeforeAckschema:   function() {
                if ( dbglevel > 0 )
                    console.log('onAckschema() - before transition')
                clearData( this )
                gotAckCheckPath( this )
                getpathAcknowledged( this )
            },
            onGetdata: function() {
                if ( dbglevel > 0 )
                    console.log('onGetdata() - state')
                onWaitdataFinalCheck( this )
            },
            onBeforeGetlaunch: function() {
                if ( dbglevel > 0 )
                    console.log('onGetlaunch() - before transition')
            },
            onBeforeNewdata: function() {
                if ( dbglevel > 0 )
                    console.log('onNewdata() - before transition')
                showData( this )
            },
            onWaitdata: function() {
                if ( dbglevel > 0 )
                    console.log('onWaitData() - state')
            },
            onShowdata: function() {
                if ( dbglevel > 0 )
                    console.log('onShowData() - state')
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
                prepareDataHalt( this )
                prepareConfHalt( this )
            }
        }
    })
}
