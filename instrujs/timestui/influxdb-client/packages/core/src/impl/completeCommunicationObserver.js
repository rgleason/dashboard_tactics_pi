"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function completeCommunicationObserver(callbacks = {}) {
    let state = 0;
    const retVal = {
        next: (data) => {
            if (state === 0 &&
                callbacks.next &&
                data !== null &&
                data !== undefined) {
                callbacks.next(data);
            }
        },
        error: (error) => {
            /* istanbul ignore else propagate error at most once */
            if (state === 0) {
                state = 1;
                /* istanbul ignore else safety check */
                if (callbacks.error)
                    callbacks.error(error);
            }
        },
        complete: () => {
            if (state === 0) {
                state = 2;
                /* istanbul ignore else safety check */
                if (callbacks.complete)
                    callbacks.complete();
            }
        },
        responseStarted: (headers) => {
            if (callbacks.responseStarted)
                callbacks.responseStarted(headers);
        },
    };
    return retVal;
}
exports.default = completeCommunicationObserver;
