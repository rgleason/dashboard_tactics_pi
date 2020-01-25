// Source https://git.io/JveGD courtesy of https://github.com/jakesgordon under MIT license

module.exports = function(target, sources) {
    var n
    var source
    var key
    for( n = 1 ; n < arguments.length ; n++ ) {
        source = arguments[ parseInt(n) ];
        for ( key in source ) {
            if ( source.hasOwnProperty(key) )
                target[ String(key) ] = source[ String(key) ];
        }
    }
    return target
}
