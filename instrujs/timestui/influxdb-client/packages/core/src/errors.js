"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.AbortError = exports.RequestTimedOutError = exports.getRetryDelay = exports.canRetryHttpCall = exports.HttpError = exports.IllegalArgumentError = exports.isStatusCodeRetriable = void 0;
const retriableStatusCodes = [404, 408, 425, 429, 500, 502, 503, 504];
/** isStatusCodeRetriable checks whether the supplied HTTP status code is retriable. */
function isStatusCodeRetriable(statusCode) {
    return retriableStatusCodes.includes(statusCode);
}
exports.isStatusCodeRetriable = isStatusCodeRetriable;
/** IllegalArgumentError is thrown when illegal argument is supplied. */
class IllegalArgumentError extends Error {
    /* istanbul ignore next */
    constructor(message) {
        super(message);
        Object.setPrototypeOf(this, IllegalArgumentError.prototype);
    }
}
exports.IllegalArgumentError = IllegalArgumentError;
/**
 * A general HTTP error.
 */
class HttpError extends Error {
    /* istanbul ignore next because of super() not being covered*/
    constructor(statusCode, statusMessage, body, retryAfter) {
        super();
        this.statusCode = statusCode;
        this.statusMessage = statusMessage;
        this.body = body;
        Object.setPrototypeOf(this, HttpError.prototype);
        if (body) {
            this.message = `${statusCode} ${statusMessage} : ${body}`;
        }
        else {
            this.message = `${statusCode} ${statusMessage}`;
        }
        this.setRetryAfter(retryAfter);
    }
    setRetryAfter(retryAfter) {
        if (typeof retryAfter === 'string') {
            // try to parse the supplied number as milliseconds
            if (/^[0-9]+$/.test(retryAfter)) {
                this._retryAfter = parseInt(retryAfter);
            }
            else {
                this._retryAfter = 0;
            }
        }
        else {
            this._retryAfter = 0;
        }
    }
    canRetry() {
        return isStatusCodeRetriable(this.statusCode);
    }
    retryAfter() {
        return this._retryAfter;
    }
}
exports.HttpError = HttpError;
//see https://nodejs.org/api/errors.html
const RETRY_CODES = [
    'ECONNRESET',
    'ENOTFOUND',
    'ESOCKETTIMEDOUT',
    'ETIMEDOUT',
    'ECONNREFUSED',
    'EHOSTUNREACH',
    'EPIPE',
];
/**
 * Tests the error in order to know if an HTTP call can be retried.
 * @param error - error to test
 * @returns true for a retriable error
 */
function canRetryHttpCall(error) {
    if (!error) {
        return false;
    }
    else if (typeof error.canRetry === 'function') {
        return !!error.canRetry();
    }
    else if (error.code && RETRY_CODES.includes(error.code)) {
        return true;
    }
    return false;
}
exports.canRetryHttpCall = canRetryHttpCall;
/**
 * Gets retry delay from the supplied error, possibly using random number up to retryJitter.
 */
function getRetryDelay(error, retryJitter) {
    if (!error) {
        return 0;
    }
    else {
        let retVal;
        if (typeof error.retryAfter === 'function') {
            return error.retryAfter();
        }
        else {
            retVal = 0;
        }
        if (retryJitter && retryJitter > 0) {
            return retVal + Math.round(Math.random() * retryJitter);
        }
        else {
            return retVal;
        }
    }
}
exports.getRetryDelay = getRetryDelay;
/** RequestTimedOutError indicates request timeout in the communication with the server */
class RequestTimedOutError extends Error {
    /* istanbul ignore next because of super() not being covered */
    constructor() {
        super();
        Object.setPrototypeOf(this, RequestTimedOutError.prototype);
        this.message = 'Request timed out';
    }
    canRetry() {
        return true;
    }
    retryAfter() {
        return 0;
    }
}
exports.RequestTimedOutError = RequestTimedOutError;
/** AbortError indicates that the communication with the server was aborted */
class AbortError extends Error {
    /* istanbul ignore next because of super() not being covered */
    constructor() {
        super();
        this.name = 'AbortError';
        Object.setPrototypeOf(this, AbortError.prototype);
        this.message = 'Response aborted';
    }
    canRetry() {
        return true;
    }
    retryAfter() {
        return 0;
    }
}
exports.AbortError = AbortError;
