"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.QueryApiImpl = void 0;
const QueryApi_1 = require("../QueryApi");
const ChunksToLines_1 = require("./ChunksToLines");
const linesToTables_1 = require("./linesToTables");
const ObservableQuery_1 = require("./ObservableQuery");
const DEFAULT_dialect = {
    header: true,
    delimiter: ',',
    quoteChar: '"',
    commentPrefix: '#',
    annotations: ['datatype', 'group', 'default'],
};
const identity = (value) => value;
class QueryApiImpl {
    constructor(transport, org) {
        this.transport = transport;
        this.options = { org };
    }
    with(options) {
        this.options = Object.assign(Object.assign({}, this.options), options);
        return this;
    }
    lines(query) {
        return new ObservableQuery_1.default(this.createExecutor(query), identity);
    }
    rows(query) {
        return new ObservableQuery_1.default(this.createExecutor(query), observer => {
            return linesToTables_1.toLineObserver({
                next(values, tableMeta) {
                    observer.next({ values, tableMeta });
                },
                error(e) {
                    observer.error(e);
                },
                complete() {
                    observer.complete();
                },
            });
        });
    }
    queryLines(query, consumer) {
        this.createExecutor(query)(consumer);
    }
    queryRows(query, consumer) {
        this.createExecutor(query)(linesToTables_1.toLineObserver(consumer));
    }
    collectRows(query, rowMapper = QueryApi_1.defaultRowMapping) {
        const retVal = [];
        return new Promise((resolve, reject) => {
            this.queryRows(query, {
                next(values, tableMeta) {
                    const toAdd = rowMapper.call(this, values, tableMeta);
                    if (toAdd !== undefined) {
                        retVal.push(toAdd);
                    }
                },
                error(error) {
                    reject(error);
                },
                complete() {
                    resolve(retVal);
                },
            });
        });
    }
    collectLines(query) {
        const retVal = [];
        return new Promise((resolve, reject) => {
            this.queryLines(query, {
                next(line) {
                    retVal.push(line);
                },
                error(error) {
                    reject(error);
                },
                complete() {
                    resolve(retVal);
                },
            });
        });
    }
    createExecutor(query) {
        const { org, type, gzip } = this.options;
        return (consumer) => {
            this.transport.send(`/api/v2/query?org=${encodeURIComponent(org)}`, JSON.stringify(this.decorateRequest({
                query: query.toString(),
                dialect: DEFAULT_dialect,
                type,
            })), {
                method: 'POST',
                headers: {
                    'content-type': 'application/json; encoding=utf-8',
                    'accept-encoding': gzip ? 'gzip' : 'identity',
                },
            }, new ChunksToLines_1.default(consumer, this.transport.chunkCombiner));
        };
    }
    decorateRequest(request) {
        var _a;
        if (typeof this.options.now === 'function') {
            request.now = this.options.now();
        }
        // https://v2.docs.influxdata.com/v2.0/api/#operation/PostQuery requires type
        request.type = (_a = this.options.type) !== null && _a !== void 0 ? _a : 'flux';
        return request;
    }
}
exports.QueryApiImpl = QueryApiImpl;
exports.default = QueryApiImpl;
