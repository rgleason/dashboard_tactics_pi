"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.DEFAULT_WriteOptions = exports.DEFAULT_RetryDelayStrategyOptions = exports.DEFAULT_ConnectionOptions = void 0;
/** default connection options */
exports.DEFAULT_ConnectionOptions = {
    timeout: 10000,
};
/** default RetryDelayStrategyOptions */
exports.DEFAULT_RetryDelayStrategyOptions = Object.freeze({
    retryJitter: 200,
    minRetryDelay: 5000,
    maxRetryDelay: 180000,
    exponentialBase: 5,
});
/** default writeOptions */
exports.DEFAULT_WriteOptions = Object.freeze(Object.assign({ batchSize: 1000, flushInterval: 60000, writeFailed: function () { }, maxRetries: 3, maxBufferLines: 32000 }, exports.DEFAULT_RetryDelayStrategyOptions));
