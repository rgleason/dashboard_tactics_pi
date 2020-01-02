/* $Id: statemachine.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
/*
  Visualize with https://xstate.js.org/viz/ current schema https://git.io/JexUj
  (not implementable with 'xstate', it is not working on IE :( )
*/
import StateMachine from 'javascript-state-machine';
import { getidAskClient, getidClientAnswer } from './getid.js'

export function createStateMachine() {
    return new StateMachine({
        init: 'window',
        data: {
            luminosity : 'day'
        },
        transitions: [
            { name: 'fetch',    from: 'window',   to: 'loading' },
            { name: 'loaded',   from: 'loading',  to: 'getid' },
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
            onFetch:    function() { console.log('onFetch()')    },
            onLoaded:   function() {
                getidAskClient()
            },
            onSetid:    function() { console.log('onSetid()')    }
        }
    })
}
