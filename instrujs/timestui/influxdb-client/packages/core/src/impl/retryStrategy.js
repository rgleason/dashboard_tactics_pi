"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.createRetryDelayStrategy = exports.RetryStrategyImpl = void 0;
const errors_1 = require("../errors");
const options_1 = require("../options");
/**
 * Applies a variant of exponential backoff with initial and max delay and a random
 * jitter delay. It also respects `retry delay` when specified together with an error.
 */
class RetryStrategyImpl {
    constructor(options) {
        this.options = Object.assign(Object.assign({}, options_1.DEFAULT_RetryDelayStrategyOptions), options);
        this.success();
    }
    nextDelay(error, failedAttempts) {
        const delay = errors_1.getRetryDelay(error);
        if (delay && delay > 0) {
            return delay + Math.round(Math.random() * this.options.retryJitter);
        }
        else {
            let delay = this.currentDelay;
            if (failedAttempts && failedAttempts > 0) {
                // compute delay
                delay = this.options.minRetryDelay;
                for (let i = 1; i < failedAttempts; i++) {
                    delay = delay * this.options.exponentialBase;
                    if (delay >= this.options.maxRetryDelay) {
                        break;
                    }
                }
                return (Math.min(Math.max(delay, 1), this.options.maxRetryDelay) +
                    Math.round(Math.random() * this.options.retryJitter));
            }
            else if (this.currentDelay) {
                this.currentDelay = Math.min(Math.max(this.currentDelay * this.options.exponentialBase, 1) +
                    Math.round(Math.random() * this.options.retryJitter), this.options.maxRetryDelay);
            }
            else {
                this.currentDelay =
                    this.options.minRetryDelay +
                        Math.round(Math.random() * this.options.retryJitter);
            }
            return this.currentDelay;
        }
    }
    success() {
        this.currentDelay = undefined;
    }
}
exports.RetryStrategyImpl = RetryStrategyImpl;
/**
 * Creates a new instance of retry strategy.
 * @param options - retry options
 * @returns retry strategy implementation
 */
function createRetryDelayStrategy(options) {
    return new RetryStrategyImpl(options);
}
exports.createRetryDelayStrategy = createRetryDelayStrategy;
