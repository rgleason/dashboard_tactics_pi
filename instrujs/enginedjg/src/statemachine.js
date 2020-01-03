/* $Id: statemachine.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
/*
  Visualize with https://xstate.js.org/viz/ current schema https://git.io/JexUj
  (not implementable with 'xstate', it is not working on IE :( )
  Note: with javascript-state-machine, avoid keyword 'init' in state/transition!
*/
import StateMachine from 'javascript-state-machine';
import getLocInfo from '../../src/location'
import { initLoad } from './init.js'
import { getidAskClient, getidClientAnswer } from './getid.js'

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
            gauge      : []
        },
        transitions: [
            { name: 'fetch',    from: 'window',   to: 'loading' },
            { name: 'loaded',   from: 'loading',  to: 'initga' },
            { name: 'initok',   from: 'initga',   to: 'getid' },
            { name: 'setid',    from: 'getid',    to: 'getid' },
            { name: 'nocfg',    from: 'getid',    to: 'getall' },
            { name: 'loadcfg',  from: 'getid',    to: 'getpath' },
            { name: 'allavlb',  from: 'getall',   to: 'showmenu' },
            { name: 'selected', from: 'showmenu', to: 'askpath' },
            { name: 'acksubsr', from: 'getpath',  to: 'showdata' },
            { name: 'chgconf',  from: 'showdata', to: 'getall' },
            { name: 'newdata',  from: 'showdata', to: 'showdata' },
            { name: 'watchcat', from: 'showdata', to: 'askpath' },
            { name: 'luminsty', from: 'getid',    to: 'getid' },
            { name: 'luminsty', from: 'getpath',  to: 'getpath' },
            { name: 'luminsty', from: 'getall',   to: 'getall' },
            { name: 'luminsty', from: 'showmenu', to: 'showmenu' },
            { name: 'luminsty', from: 'showdata', to: 'showdata' }
        ],
        methods: {
            onWindow:   function() { console.log('onWindow()')    },
            onLoading:  function() { console.log('onLoading()')
                                     console.log('uid: ', this.uid )
                                     console.log('locInfo: ', this.locInfo )
                                     console.log('gauge[', this.gauge.length, ']')
                                     console.log('conf: ', this.conf)
                                   },
            onInitga:   function() { console.log('onInitga()')
                                     initLoad( this )
                                     console.log('uid: ', this.uid )
                                     console.log('locInfo: ', this.locInfo )
                                     console.log('gauge[', this.gauge.length, ']')
                                     console.log('conf: ', this.conf)
                                   },
            onGetid:   function() { console.log('onInit()')
                                    getidAskClient()
                                  },
            onSetid:    function() { console.log('onSetid()')
                                     getidClientAnswer( this )
                                     console.log('uid: ', this.uid )
                                     console.log('locInfo: ', this.locInfo )
                                     console.log('gauge[', this.gauge.length, ']')
                                     console.log('conf: ', this.conf)
                                   }
        }
    })
}
