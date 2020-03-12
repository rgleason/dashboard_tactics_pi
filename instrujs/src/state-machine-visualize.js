// Source https://git.io/JveGn courtesy of https://github.com/jakesgordon under MIT license

//-------------------------------------------------------------------------------------------------

var mixin = require('./mixin')

//-------------------------------------------------------------------------------------------------

function pad(name) {
  return ' ' + name + ' '
}

function quote(name) {
  return '"' + name + '"'
}

//-------------------------------------------------------------------------------------------------

function dotcfg(fsm, options) {

    options = options || {}

    var config      = dotcfg.fetch(fsm),
        name        = options.name,
        rankdir     = dotcfg.rankdir(options.orientation),
        states      = dotcfg.states(config, options),
        transitions = dotcfg.transitions(config, options),
        result      = { }

    if (name)
        result.name = name

    if (rankdir)
        result.rankdir = rankdir

    if (states && states.length > 0)
        result.states = states

    if (transitions && transitions.length > 0)
        result.transitions = transitions

    return result
}

//-------------------------------------------------------------------------------------------------

dotcfg.fetch = function(fsm) {
    return (typeof fsm === 'function') ? fsm.prototype._fsm.config
        : fsm._fsm.config
}

dotcfg.rankdir = function(orientation) {
    if (orientation === 'horizontal')
        return 'LR'
    else if (orientation === 'vertical')
        return 'TB'
}

dotcfg.states = function(config, options) {
    var index
    var states = config.states
    if (!options.init) { // if not showing init transition, then slice out the implied init :from state
        index  = states.indexOf(config.init.from)
        states = states.slice(0, index).concat(states.slice(index+1))
    }
    return states
}

dotcfg.transitions = function(config, options) {
    var n
    var max
    var transition
    var init        = config.init
    var transitions = config.options.transitions || [] // easier to visualize using the ORIGINAL transition declarations rather than our run-time mapping
    var output = []
    if (options.init && init.active)
        dotcfg.transition(init.name, init.from, init.to, init.dot, config, options, output)
    for (n = 0, max = transitions.length ; n < max ; n++) {
        transition = config.options.transitions[parseInt(n)]
        dotcfg.transition(transition.name, transition.from, transition.to, transition.dot, config, options, output)
    }
    return output
}

dotcfg.transition = function(name, from, to, dot, config, options, output) {
    var n
    var max
    var wildcard = config.defaults.wildcard

    if (Array.isArray(from))
        for(n = 0, max = from.length ; n < max ; n++)
            dotcfg.transition(name, from[parseInt(n)], to, dot, config, options, output)
    else if (from === wildcard || (typeof from === 'undefined') )
        for(n = 0, max = config.states.length ; n < max ; n++)
            dotcfg.transition(name, config.states[parseInt(n)], to, dot, config, options, output)
    else if (to === wildcard || (typeof to === 'undefined') )
        dotcfg.transition(name, from, from, dot, config, options, output)
    else if (typeof to === 'function') {
        // do nothing, can't display conditional transition
    }
    else
        output.push(mixin({}, { from: from, to: to, label: pad(name) }, dot || {}))
}


function dotify(dotcfg) {

    dotcfg = dotcfg || {}

    var name        = dotcfg.name || 'fsm'
    var states      = dotcfg.states || []
    var transitions = dotcfg.transitions || []
    var rankdir     = dotcfg.rankdir
    var output      = []
    var n
    var max

    output.push('digraph ' + quote(name) + ' {')
    if (rankdir)
        output.push('  rankdir=' + rankdir + ';')
    for(n = 0, max = states.length ; n < max ; n++)
        output.push(dotify.state(states[parseInt(n)]))
    for(n = 0, max = transitions.length ; n < max ; n++)
        output.push(dotify.edge(transitions[parseInt(n)]))
    output.push('}')
    return output.join('\n')

}

dotify.state = function(state) {
    return '  ' + quote(state) + ';'
}

dotify.edge = function(edge) {
    return '  ' + quote(edge.from) + ' -> ' + quote(edge.to) + dotify.edge.attr(edge) + ';'
}

dotify.edge.attr = function(edge) {
    var n
    var max
    var keys = Object.keys(edge).sort()
    var output = []
    for( n = 0, max = keys.length ; n < max ; n++ ) {
        var key = keys[parseInt(n)]
        if (key !== 'from' && key !== 'to')
            output.push(key + '=' + quote(edge[ String(key) ]))
    }
    return output.length > 0 ? ' [ ' + output.join(' ; ') + ' ]' : ''
}

//-------------------------------------------------------------------------------------------------
export default function visualize(fsm, options) {
    return dotify(dotcfg(fsm, options))
}
//-------------------------------------------------------------------------------------------------
