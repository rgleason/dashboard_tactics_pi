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
import {initButtons, btmarmwButtons, btmarmcButtons} from './buttons'
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
            // Environmental
            luminosity : 'day',
            locInfo    : getLocInfo(),
            // Functional
        },
        transitions: [
            { name: 'init',      from: 'window',   to: 'loading' },
            { name: 'loaded',    from: 'loading',  to: 'waiting' },
            { name: 'btnarmw',   from: 'waiting',  to: 'marking' },
            { name: 'btnportd1', from: 'marking',  to: 'onemark' },
            { name: 'btnstbdd1', from: 'marking',  to: 'onemark' },
            { name: 'btnportd2', from: 'onemark',  to: 'armed'   },
            { name: 'btnstbdd2', from: 'onemark',  to: 'armed'   },
            { name: 'btnarmc',   from: 'marking',  to: 'waiting' },
            { name: 'btnarmc',   from: 'onemark',  to: 'waiting' },
            { name: 'luminsty',  from: 'waiting',  to: 'waiting' },
            { name: 'luminsty',  from: 'marking',  to: 'marking' },
            { name: 'luminsty',  from: 'onemark',  to: 'onemark' },
            { name: 'luminsty',  from: 'armed',    to: 'armed'   },
            { name: 'closing',   from: 'waiting',  to: 'halt'    },
            { name: 'closing',   from: 'marking',  to: 'halt'    },
            { name: 'closing',   from: 'onemark',  to: 'halt'    },
            { name: 'closing',   from: 'armed',    to: 'halt'    }
        ],
        methods: {
            onWindow:   function() {
                if ( dbglevel > 0 ) console.log('onWindow() - state')
            },
            onLoading:  function() {
                if ( dbglevel > 0 ) console.log('onLoading() - state')
                if ( dbglevel > 1 ) console.log('locInfo: ', this.locInfo )
            },
            onBeforeLoaded:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeLoaded() - transition')
                initButtons( this )
            },
            onWaiting:   function() {
                if ( dbglevel > 0 ) console.log('onWaiting() - state')
            },
            onBeforeBtnarmw:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnarmw() - transition')
                btmarmwButtons()
            },
            onMarking:   function() {
                if ( dbglevel > 0 ) console.log('onMarking() - state')
            },
            onBeforeBtnarmc:    function() {
                if ( dbglevel > 0 ) console.log('onBeforeBtnarmc() - transition')
                btmarmcButtons( this )
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
            }
        }
    })
}
