/* $Id: location.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

export function getLocInfo() {
    var href = ''
    var protocol = ''
    var hostname = ''
    var domain = ''
    var port = ''
    if ( window.location ) {
        href = window.location.href
        console.log('href: ', href )
        protocol = window.location.protocol
        console.log('protocol: ', protocol )
        hostname = window.location.hostname
        console.log('hostname: ', hostname )
        if ( window.location.domain )
            domain = window.location.domain
        else
            domain = ''
        console.log('domain: ', domain )
        if ( window.location.port )
            port = window.location.port
        else
            port = ''
        port = window.location.port
        console.log('port: ', port )
    }
    return {
        href: href,
        protocol: protocol,
        hostname: hostname,
        domain: domain,
        port: port
    }
}
