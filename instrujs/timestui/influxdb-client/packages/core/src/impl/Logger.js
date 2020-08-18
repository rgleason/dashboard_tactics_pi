"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.setLogger = exports.consoleLogger = void 0;
/**
 * Logger that logs to console.out
 */
exports.consoleLogger = Object.freeze({
    error(message, error) {
        // eslint-disable-next-line no-console
        console.error('ERROR: ' + message, error ? error : '');
    },
    warn(message, error) {
        // eslint-disable-next-line no-console
        console.warn('WARN: ' + message, error ? error : '');
    },
});
let provider = exports.consoleLogger;
const Logger = {
    error(message, error) {
        provider.error(message, error);
    },
    warn(message, error) {
        provider.warn(message, error);
    },
};
/**
 * Sets custom logger.
 * @param logger - logger to use
 * @returns previous logger
 */
function setLogger(logger) {
    const previous = provider;
    provider = logger;
    return previous;
}
exports.setLogger = setLogger;
exports.default = Logger;
