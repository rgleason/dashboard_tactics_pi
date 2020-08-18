"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.flux = exports.toFluxValue = exports.fluxExpression = exports.fluxBool = exports.fluxRegExp = exports.fluxDuration = exports.fluxDateTime = exports.fluxFloat = exports.sanitizeFloat = exports.fluxInteger = exports.fluxString = exports.FLUX_VALUE = void 0;
/** Property that offers a function that returns flux-sanitized value of an object.  */
exports.FLUX_VALUE = Symbol('FLUX_VALUE');
class FluxParameter {
    constructor(fluxValue) {
        this.fluxValue = fluxValue;
    }
    toString() {
        return this.fluxValue;
    }
    [exports.FLUX_VALUE]() {
        return this.fluxValue;
    }
}
/**
 * Escapes content of the supplied string so it can be wrapped into double qoutes
 * to become a [flux string literal](https://docs.influxdata.com/flux/v0.65/language/lexical-elements/#string-literals).
 * @param value - string value
 * @returns sanitized string
 */
function sanitizeString(value) {
    if (value === null || value === undefined)
        return '';
    value = value.toString();
    let retVal = undefined;
    let i = 0;
    function prepareRetVal() {
        if (retVal === undefined) {
            retVal = value.substring(0, i);
        }
    }
    for (; i < value.length; i++) {
        const c = value.charAt(i);
        switch (c) {
            case '\r':
                prepareRetVal();
                retVal += '\\r';
                break;
            case '\n':
                prepareRetVal();
                retVal += '\\n';
                break;
            case '\t':
                prepareRetVal();
                retVal += '\\t';
                break;
            case '"':
            case '\\':
                prepareRetVal();
                retVal = retVal + '\\' + c;
                break;
            case '$':
                // escape ${
                if (i + 1 < value.length && value.charAt(i + 1) === '{') {
                    prepareRetVal();
                    i++;
                    retVal += '\\${';
                    break;
                }
                // append $
                if (retVal != undefined) {
                    retVal += c;
                }
                break;
            default:
                if (retVal != undefined) {
                    retVal += c;
                }
        }
    }
    if (retVal !== undefined) {
        return retVal;
    }
    return value;
}
/**
 * Creates a flux string literal.
 */
function fluxString(value) {
    return new FluxParameter(`"${sanitizeString(value)}"`);
}
exports.fluxString = fluxString;
/**
 * Creates a flux integer literal.
 */
function fluxInteger(value) {
    const val = String(value);
    for (const c of val) {
        if (c < '0' || c > '9')
            throw new Error(`not a flux integer: ${val}`);
    }
    return new FluxParameter(val);
}
exports.fluxInteger = fluxInteger;
/**
 * Sanitizes float value to avoid injections.
 * @param value - InfluxDB float literal
 * @returns sanitized float value
 * @throws Error if the the value cannot be sanitized
 */
function sanitizeFloat(value) {
    const val = String(value);
    let dot = false;
    for (const c of val) {
        if (c === '.') {
            if (dot)
                throw new Error(`not a flux float: ${val}`);
            dot = !dot;
        }
        if (c !== '.' && (c < '0' || c > '9'))
            throw new Error(`not a flux float: ${val}`);
    }
    return val;
}
exports.sanitizeFloat = sanitizeFloat;
/**
 * Creates a flux float literal.
 */
function fluxFloat(value) {
    return new FluxParameter(sanitizeFloat(value));
}
exports.fluxFloat = fluxFloat;
function sanitizeDateTime(value) {
    return `time(v: "${sanitizeString(value)}")`;
}
/**
 * Creates flux date-time literal.
 */
function fluxDateTime(value) {
    return new FluxParameter(sanitizeDateTime(value));
}
exports.fluxDateTime = fluxDateTime;
/**
 * Creates flux date-time literal.
 */
function fluxDuration(value) {
    return new FluxParameter(`duration(v: "${sanitizeString(value)}")`);
}
exports.fluxDuration = fluxDuration;
function sanitizeRegExp(value) {
    return `regexp.compile(v: "${sanitizeString(value)}")`;
}
/**
 * Creates flux regexp literal.
 */
function fluxRegExp(value) {
    // let the server decide if it can be parsed
    return new FluxParameter(sanitizeRegExp(value));
}
exports.fluxRegExp = fluxRegExp;
/**
 * Creates flux boolean literal.
 */
function fluxBool(value) {
    if (value === 'true' || value === 'false') {
        return new FluxParameter(value);
    }
    return new FluxParameter((!!value).toString());
}
exports.fluxBool = fluxBool;
/**
 * Assumes that the supplied value is flux expression or literal that does not need sanitizing.
 *
 * @param value - any value
 * @returns the supplied value as-is
 */
function fluxExpression(value) {
    return new FluxParameter(String(value));
}
exports.fluxExpression = fluxExpression;
/**
 * Escapes content of the supplied parameter so that it can be safely embedded into flux query.
 * @param value - parameter value
 * @returns sanitized flux value or an empty string if it cannot be converted
 */
function toFluxValue(value) {
    if (value === undefined) {
        return '';
    }
    else if (value === null) {
        return 'null';
    }
    else if (typeof value === 'boolean') {
        return value.toString();
    }
    else if (typeof value === 'string') {
        return `"${sanitizeString(value)}"`;
    }
    else if (typeof value === 'number') {
        return sanitizeFloat(value);
    }
    else if (typeof value === 'object') {
        if (typeof value[exports.FLUX_VALUE] === 'function') {
            return value[exports.FLUX_VALUE]();
        }
        else if (value instanceof Date) {
            return value.toISOString();
        }
        else if (value instanceof RegExp) {
            return sanitizeRegExp(value);
        }
        else if (Array.isArray(value)) {
            return `[${value.map(toFluxValue).join(',')}]`;
        }
    }
    // use toString value for unrecognized object, bigint, symbol
    return toFluxValue(value.toString());
}
exports.toFluxValue = toFluxValue;
/**
 * Flux is a tagged template that sanitizes supplied parameters
 * to avoid injection attacks in flux.
 */
function flux(strings, ...values) {
    if (strings.length == 1 && (!values || values.length === 0))
        return strings[0]; // the simplest case
    const parts = new Array(strings.length + values.length);
    let partIndex = 0;
    for (let i = 0; i < strings.length; i++) {
        const text = strings[i];
        parts[partIndex++] = text;
        if (i < values.length) {
            const val = values[i];
            let sanitized;
            if (text.endsWith('"') &&
                i + 1 < strings.length &&
                strings[i + 1].startsWith('"')) {
                // parameter is wrapped into flux double quotes
                sanitized = sanitizeString(val);
            }
            else {
                sanitized = toFluxValue(val);
                if (sanitized === '') {
                    throw new Error(`Unsupported parameter literal '${val}' at index: ${i}, type: ${typeof val}`);
                }
            }
            parts[partIndex++] = sanitized;
        }
        else if (i < strings.length - 1) {
            throw new Error('Too few parameters supplied!');
        }
    }
    // return flux expression so that flux can be embedded into another flux as-is
    return fluxExpression(parts.join(''));
}
exports.flux = flux;
