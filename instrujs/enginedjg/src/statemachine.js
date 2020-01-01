/* $Id: statemachine.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */
import { Machine, interpret } from 'xstate';

/*
  Designed with https://xstate.js.org/viz/
  gist: https://git.io/Jexfk
  Works on non-IE browsers, unfortunately wxWidgets WebView backend on Windows is IE!
*/
const instrujsMachine =  Machine({
    // Machine identifier
    id: 'enginedjg',

    // Initial state
    initial: 'window',

    // Local context for entire machine
    context: {
        elapsed: 0,
    },

    // State definitions
    states: {
        window: {
            on: {
                FETCH: 'loading'
            }
        },
        loading: {
            on: {
                NOCFG: 'askall',
                LOADCFG: 'askpath'
            }
        },
        askall: {
            on: {
                ALLAVLB: 'showmenu'
            }
        },
        showmenu: {
            on: {
                SELECTION: 'askpath'
            }
        },
        askpath: {
            on: {
                ACKSUBSR: 'showdata'
            }
        },
        showdata: {
            on: {
                CHGCONF: 'askall',
                NEWDATA: 'showdata',
                WATCHDOG: 'askpath'
            }
        }
    }
});

const instrujsService = interpret(instrujsMachine)
  .onTransition(state => console.log(state.context.count))
  .start();

export function getInstrujsService() { return instrujsService }
