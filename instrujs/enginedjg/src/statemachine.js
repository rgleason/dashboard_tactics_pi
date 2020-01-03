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
import { initLoad } from './init'
import { getidAskClient, getidClientAnswer } from './getid'
import { getallAskClient, getallClientAnswer, getpathAskClient } from './path'
import { showData } from './data'
import { getNewLuminosity } from './css'

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
            allpaths   : []
        },
        transitions: [
            { name: 'fetch',    from: 'window',   to: 'loading' },
            { name: 'loaded',   from: 'loading',  to: 'initga' },
            { name: 'initok',   from: 'initga',   to: 'getid' },
            { name: 'setid',    from: 'getid',    to: 'getid' },
            { name: 'nocfg',    from: 'getid',    to: 'getall' },
            { name: 'hascfg',   from: 'getid',    to: 'getpath' },
            { name: 'setall',   from: 'getall',   to: 'showmenu' },
            { name: 'selected', from: 'showmenu', to: 'askpath' },
            { name: 'newdata',  from: 'getpath',  to: 'showdata' },
            { name: 'chgconf',  from: 'showdata', to: 'getall' },
            { name: 'newdata',  from: 'showdata', to: 'showdata' },
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
            onGetid:    function() { console.log('onInit()')
                                     getidAskClient()
                                   },
            onSetid:    function() { console.log('onSetid()')
                                     getidClientAnswer( this )
                                     console.log('uid: ', this.uid )
                                   },
            onGetall:   function() { console.log('onGetall()')
                                     getallAskClient()
                                   },
            onAllavlb:  function() { console.log('onAllavlb()')
                                     getiallClientAnswer( this )
                                     console.log('allpaths: ', this.allpaths )
                                   },
            onGetpath:  function() { console.log('onGetpath()')
                                     getpathAskClient( this )
                                   },
            onShowdata: function() { console.log('onShowData()')
                                     showData( this )
                                   },
            onLuminsty: function() { console.log('onLuminsty()')
                                     getNewLuminosity( this )
                                     console.log('luminosity: ', this.luminosity )
                                   }
        }
    })
}
