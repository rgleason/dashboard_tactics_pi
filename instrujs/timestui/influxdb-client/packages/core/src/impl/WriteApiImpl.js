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
const options_1 = require("../options");
const Logger_1 = require("./Logger");
const errors_1 = require("../errors");
const escape_1 = require("../util/escape");
const currentTime_1 = require("../util/currentTime");
const retryStrategy_1 = require("./retryStrategy");
const RetryBuffer_1 = require("./RetryBuffer");
class WriteBuffer {
    constructor(maxChunkRecords, flushFn, scheduleSend) {
        this.maxChunkRecords = maxChunkRecords;
        this.flushFn = flushFn;
        this.scheduleSend = scheduleSend;
        this.length = 0;
        this.lines = new Array(maxChunkRecords);
    }
    add(record) {
        if (this.length === 0) {
            this.scheduleSend();
        }
        this.lines[this.length] = record;
        this.length++;
        if (this.length >= this.maxChunkRecords) {
            this.flush().catch(_e => {
                // an error is logged in case of failure, avoid UnhandledPromiseRejectionWarning
            });
        }
    }
    flush() {
        const lines = this.reset();
        if (lines.length > 0) {
            return this.flushFn(lines);
        }
        else {
            return Promise.resolve();
        }
    }
    reset() {
        const retVal = this.lines.slice(0, this.length);
        this.length = 0;
        return retVal;
    }
}
class WriteApiImpl {
    constructor(transport, org, bucket, precision, writeOptions) {
        this.transport = transport;
        this.closed = false;
        this.sendOptions = {
            method: 'POST',
            headers: {
                'content-type': 'text/plain; charset=utf-8',
            },
        };
        this._timeoutHandle = undefined;
        this.httpPath = `/api/v2/write?org=${encodeURIComponent(org)}&bucket=${encodeURIComponent(bucket)}&precision=${precision}`;
        this.writeOptions = Object.assign(Object.assign({}, options_1.DEFAULT_WriteOptions), writeOptions);
        this.currentTime = currentTime_1.currentTime[precision];
        this.dateToProtocolTimestamp = currentTime_1.dateToProtocolTimestamp[precision];
        if (this.writeOptions.defaultTags) {
            this.useDefaultTags(this.writeOptions.defaultTags);
        }
        const scheduleNextSend = () => {
            if (this.writeOptions.flushInterval > 0) {
                this._clearFlushTimeout();
                /* istanbul ignore else manually reviewed, hard to reproduce */
                if (!this.closed) {
                    this._timeoutHandle = setTimeout(() => this.sendBatch(this.writeBuffer.reset(), this.writeOptions.maxRetries + 1).catch(_e => {
                        // an error is logged in case of failure, avoid UnhandledPromiseRejectionWarning
                    }), this.writeOptions.flushInterval);
                }
            }
        };
        // write buffer
        this.writeBuffer = new WriteBuffer(this.writeOptions.batchSize, lines => {
            this._clearFlushTimeout();
            return this.sendBatch(lines, this.writeOptions.maxRetries + 1);
        }, scheduleNextSend);
        this.sendBatch = this.sendBatch.bind(this);
        // retry buffer
        this.retryStrategy = retryStrategy_1.createRetryDelayStrategy(this.writeOptions);
        this.retryBuffer = new RetryBuffer_1.default(this.writeOptions.maxBufferLines, this.sendBatch);
    }
    sendBatch(lines, attempts) {
        // eslint-disable-next-line @typescript-eslint/no-this-alias
        const self = this;
        if (!this.closed && lines.length > 0) {
            return new Promise((resolve, reject) => {
                this.transport.send(this.httpPath, lines.join('\n'), this.sendOptions, {
                    error(error) {
                        const failedAttempts = self.writeOptions.maxRetries + 2 - attempts;
                        // call the writeFailed listener and check if we can retry
                        const onRetry = self.writeOptions.writeFailed.call(self, error, lines, failedAttempts);
                        if (onRetry) {
                            onRetry.then(resolve, reject);
                            return;
                        }
                        if (!self.closed &&
                            attempts > 1 &&
                            (!(error instanceof errors_1.HttpError) ||
                                error.statusCode >= 429)) {
                            Logger_1.default.warn(`Write to InfluxDB failed (remaining attempts: ${attempts -
                                1}).`, error);
                            self.retryBuffer.addLines(lines, attempts - 1, self.retryStrategy.nextDelay(error, failedAttempts));
                            reject(error);
                            return;
                        }
                        Logger_1.default.error(`Write to InfluxDB failed.`, error);
                        reject(error);
                    },
                    complete() {
                        self.retryStrategy.success();
                        resolve();
                    },
                });
            });
        }
        else {
            return Promise.resolve();
        }
    }
    _clearFlushTimeout() {
        if (this._timeoutHandle !== undefined) {
            clearTimeout(this._timeoutHandle);
            this._timeoutHandle = undefined;
        }
    }
    writeRecord(record) {
        this.writeBuffer.add(record);
    }
    writeRecords(records) {
        for (let i = 0; i < records.length; i++) {
            this.writeBuffer.add(records[i]);
        }
    }
    writePoint(point) {
        const line = point.toLineProtocol(this);
        if (line)
            this.writeBuffer.add(line);
    }
    writePoints(points) {
        for (let i = 0; i < points.length; i++) {
            this.writePoint(points[i]);
        }
    }
    flush(withRetryBuffer) {
        return __awaiter(this, void 0, void 0, function* () {
            yield this.writeBuffer.flush();
            if (withRetryBuffer) {
                return yield this.retryBuffer.flush();
            }
        });
    }
    close() {
        const retVal = this.writeBuffer.flush().finally(() => {
            const remaining = this.retryBuffer.close();
            if (remaining) {
                Logger_1.default.error(`Retry buffer closed with ${remaining} items that were not written to InfluxDB!`, null);
            }
            this.closed = true;
        });
        return retVal;
    }
    dispose() {
        this._clearFlushTimeout();
        this.closed = true;
        return this.retryBuffer.close() + this.writeBuffer.length;
    }
    useDefaultTags(tags) {
        this.defaultTags = undefined;
        Object.keys(tags).forEach((key) => {
            ;
            (this.defaultTags || (this.defaultTags = {}))[key] = escape_1.escape.tag(tags[key]);
        });
        return this;
    }
    convertTime(value) {
        if (value === undefined) {
            return this.currentTime();
        }
        else if (typeof value === 'string') {
            return value.length > 0 ? value : undefined;
        }
        else if (value instanceof Date) {
            return this.dateToProtocolTimestamp(value);
        }
        else if (typeof value === 'number') {
            return String(Math.floor(value));
        }
        else {
            // Logger.warn(`unsupported timestamp value: ${value}`)
            return String(value);
        }
    }
}
exports.default = WriteApiImpl;
