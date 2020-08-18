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
var __rest = (this && this.__rest) || function (s, e) {
    var t = {};
    for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p) && e.indexOf(p) < 0)
        t[p] = s[p];
    if (s != null && typeof Object.getOwnPropertySymbols === "function")
        for (var i = 0, p = Object.getOwnPropertySymbols(s); i < p.length; i++) {
            if (e.indexOf(p[i]) < 0 && Object.prototype.propertyIsEnumerable.call(s, p[i]))
                t[p[i]] = s[p[i]];
        }
    return t;
};
Object.defineProperty(exports, "__esModule", { value: true });
// IE11 AbortController:
require("isomorphic-fetch");
require("abortcontroller-polyfill/dist/polyfill-patch-fetch");
const pureJsChunkCombiner_1 = require("../pureJsChunkCombiner");
const errors_1 = require("../../errors");
const completeCommunicationObserver_1 = require("../completeCommunicationObserver");
const Logger_1 = require("../Logger");
const version_1 = require("../version");
/**
 * Transport layer that use browser fetch.
 */
class FetchTransport {
    constructor(connectionOptions) {
        this.connectionOptions = connectionOptions;
        this.chunkCombiner = pureJsChunkCombiner_1.default;
        this.defaultHeaders = {
            'content-type': 'application/json; charset=utf-8',
            'User-Agent': `influxdb-client-js/${version_1.CLIENT_LIB_VERSION}`,
        };
        if (this.connectionOptions.token) {
            this.defaultHeaders['Authorization'] =
                'Token ' + this.connectionOptions.token;
        }
    }
    send(path, body, options, callbacks) {
        const observer = completeCommunicationObserver_1.default(callbacks);
        if (callbacks && callbacks.useCancellable && !options.signal) {
            const controller = new AbortController();
            const signal = controller.signal;
            callbacks.useCancellable({
                cancel() {
                    controller.abort();
                },
                isCancelled() {
                    return signal.aborted;
                },
            });
        }
        this.fetch(path, body, options)
            .then((response) => __awaiter(this, void 0, void 0, function* () {
            if (callbacks === null || callbacks === void 0 ? void 0 : callbacks.responseStarted) {
                const headers = {};
                response.headers.forEach((value, key) => {
                    const previous = headers[key];
                    if (previous === undefined) {
                        headers[key] = value;
                    }
                    else if (Array.isArray(previous)) {
                        previous.push(value);
                    }
                    else {
                        headers[key] = [previous, value];
                    }
                });
                observer.responseStarted(headers);
            }
            if (response.status >= 300) {
                return response
                    .text()
                    .then((text) => {
                    if (!text) {
                        const headerError = response.headers.get('x-influxdb-error');
                        if (headerError) {
                            text = headerError;
                        }
                    }
                    observer.error(new errors_1.HttpError(response.status, response.statusText, text, response.headers.get('retry-after')));
                })
                    .catch((e) => {
                    Logger_1.default.warn('Unable to receive error body', e);
                    observer.error(new errors_1.HttpError(response.status, response.statusText, undefined, response.headers.get('retry-after')));
                });
            }
            else {
                if (response.body) {
                    const reader = response.body.getReader();
                    let chunk;
                    do {
                        chunk = yield reader.read();
                        observer.next(chunk.value);
                    } while (!chunk.done);
                }
                else if (response.arrayBuffer) {
                    const buffer = yield response.arrayBuffer();
                    observer.next(new Uint8Array(buffer));
                }
                else {
                    const text = yield response.text();
                    observer.next(new TextEncoder().encode(text));
                }
            }
        }))
            .catch(e => observer.error(e))
            .finally(() => observer.complete());
    }
    request(path, body, options) {
        return __awaiter(this, void 0, void 0, function* () {
            const response = yield this.fetch(path, body, options);
            const { status, headers } = response;
            const responseContentType = headers.get('content-type') || '';
            let data = undefined;
            try {
                if (responseContentType.includes('json')) {
                    data = yield response.json();
                }
                else if (responseContentType.includes('text')) {
                    data = yield response.text();
                }
            }
            catch (_e) {
                // ignore
                Logger_1.default.warn('Unable to read error body', _e);
            }
            if (status >= 300) {
                if (!data) {
                    const headerError = headers.get('x-influxdb-error');
                    if (headerError) {
                        data = headerError;
                    }
                }
                throw new errors_1.HttpError(status, response.statusText, data, response.headers.get('retry-after'));
            }
            return data;
        });
    }
    fetch(path, body, options) {
        const { method, headers } = options, other = __rest(options, ["method", "headers"]);
        return fetch(`${this.connectionOptions.url}${path}`, Object.assign({ method: method, body: method === 'GET' || method === 'HEAD'
                ? undefined
                : typeof body === 'string'
                    ? body
                    : JSON.stringify(body), headers: Object.assign(Object.assign({}, this.defaultHeaders), headers), credentials: 'omit' }, other));
    }
}
exports.default = FetchTransport;
