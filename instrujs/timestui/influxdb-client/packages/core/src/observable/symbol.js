"use strict";
/* Observable interop typing. Taken from https://github.com/ReactiveX/rxjs */
Object.defineProperty(exports, "__esModule", { value: true });
exports.symbolObservable = void 0;
/** Symbol.observable or a string "\@\@observable". Used for interop */
exports.symbolObservable = (() => (typeof Symbol === 'function' && Symbol.observable) || '@@observable')();
