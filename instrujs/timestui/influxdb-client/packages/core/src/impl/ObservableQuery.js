"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const index_1 = require("../observable/index");
class QuerySubscription {
    constructor(observer, executor) {
        this.isClosed = false;
        try {
            executor({
                next: value => {
                    observer.next(value);
                },
                error: e => {
                    this.isClosed = true;
                    observer.error(e);
                },
                complete: () => {
                    this.isClosed = true;
                    observer.complete();
                },
                useCancellable: c => {
                    this.cancellable = c;
                },
            });
        }
        catch (e) {
            this.isClosed = true;
            observer.error(e);
        }
    }
    get closed() {
        return this.isClosed;
    }
    unsubscribe() {
        var _a;
        (_a = this.cancellable) === null || _a === void 0 ? void 0 : _a.cancel();
        this.isClosed = true;
    }
}
function noop() { }
function completeObserver(observer) {
    const { next, error, complete } = observer;
    return {
        next: next ? next.bind(observer) : noop,
        error: error ? error.bind(observer) : noop,
        complete: complete ? complete.bind(observer) : noop,
    };
}
class ObservableQuery {
    constructor(executor, decorator) {
        this.executor = executor;
        this.decorator = decorator;
    }
    subscribe(observerOrNext, error, complete) {
        const observer = completeObserver(typeof observerOrNext !== 'object' || observerOrNext === null
            ? { next: observerOrNext, error, complete }
            : observerOrNext);
        return new QuerySubscription(this.decorator(observer), this.executor);
    }
    [index_1.symbolObservable]() {
        return this;
    }
}
exports.default = ObservableQuery;
