/* $Id: escapeHTML.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// Implementation originally Mozilla
// https://github.com/mozilla-b2g/gaia/commit/5bd5ac20771a76a2f359cfbd14875c8fab86c247

export default function Tagged() {
    var Tagged = {
        _entity: /[&<>"'/]/g,
    
        _entities: {
            '&': '&amp;',
            '<': '&lt;',
            '>': '&gt;',
            '"': '&quot;',
            '\'': '&apos;',
            '/': '&#x2F;'
        },
        
        getEntity: function (s) {
            return Tagged._entities[s]
        },
        
        /**
         * Escapes HTML for all values in a tagged template string.
         */
        escapeHTML: function (strings, ...values) {
            var result = ''
            
            for (var i = 0; i < strings.length; i++) {
                result += strings[i]
                if (i < values.length) {
                    result += String(values[i]).replace(Tagged._entity, Tagged.getEntity)
                }
            }
            
            return result
        },
        /**
         * Escapes HTML and returns a wrapped object to be used during DOM insertion
         */
        createSafeHTML: function (strings, ...values) {
            var escaped = Tagged.escapeHTML(strings, ...values)
            return {
                __html: escaped,
                toString: function () {
                    return '[object WrappedHTMLObject]'
                },
                info: 'This is a wrapped HTML object. See https://developer.mozilla.or'+
                    'g/en-US/Firefox_OS/Security/Security_Automation for more.'
            }
        },
        /**
         * Unwrap safe HTML created by createSafeHTML or a custom replacement that
         * underwent security review.
         */
        unwrapSafeHTML: function (htmlObject) {
            return htmlObject.__html
        }
    };
    return Tagged
}
