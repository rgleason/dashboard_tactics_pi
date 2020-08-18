"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
const Logger_1 = require("./Logger");
/* interval between successful retries */
const RETRY_INTERVAL = 1;
/**
 * Retries lines up to a limit of max buffer size.
 */
class RetryBuffer {
    constructor(maxLines, retryLines) {
        this.maxLines = maxLines;
        this.retryLines = retryLines;
        this.size = 0;
        this.nextRetryTime = 0;
        this.closed = false;
        this._timeoutHandle = undefined;
    }
    addLines(lines, retryCount, delay) {
        if (this.closed)
            return;
        if (!lines.length)
            return;
        const retryTime = Date.now() + delay;
        if (retryTime > this.nextRetryTime)
            this.nextRetryTime = retryTime;
        // ensure at most maxLines are in the Buffer
        if (this.first && this.size + lines.length > this.maxLines) {
            const origSize = this.size;
            const newSize = origSize * 0.7; // reduce to 70 %
            do {
                const newFirst = this.first.next;
                this.size -= this.first.lines.length;
                this.first = newFirst;
            } while (this.first && this.size + lines.length > newSize);
            Logger_1.default.error(`RetryBuffer: ${origSize -
                this
                    .size} oldest lines removed to keep buffer size under the limit of ${this.maxLines} lines`);
        }
        const toAdd = {
            lines,
            retryCount,
        };
        if (this.last) {
            this.last.next = toAdd;
            this.last = toAdd;
        }
        else {
            this.first = toAdd;
            this.last = toAdd;
            this.scheduleRetry(delay);
        }
        this.size += lines.length;
    }
    removeLines() {
        if (this.first) {
            const toRetry = this.first;
            this.first = this.first.next;
            this.size -= toRetry.lines.length;
            if (!this.first)
                this.last = undefined;
            return toRetry;
        }
        return undefined;
    }
    scheduleRetry(delay) {
        this._timeoutHandle = setTimeout(() => {
            const toRetry = this.removeLines();
            if (toRetry) {
                this.retryLines(toRetry.lines, toRetry.retryCount)
                    .then(() => {
                    // continue with successfull retry
                    this.scheduleRetry(RETRY_INTERVAL);
                })
                    .catch(_e => {
                    // already logged
                    this.scheduleRetry(this.nextRetryTime - Date.now());
                });
            }
            else {
                this._timeoutHandle = undefined;
            }
        }, delay);
    }
    flush() {
        return __awaiter(this, void 0, void 0, function* () {
            let toRetry;
            while ((toRetry = this.removeLines())) {
                yield this.retryLines(toRetry.lines, toRetry.retryCount);
            }
        });
    }
    close() {
        if (this._timeoutHandle) {
            clearTimeout(this._timeoutHandle);
            this._timeoutHandle = undefined;
        }
        this.closed = true;
        return this.size;
    }
}
exports.default = RetryBuffer;
