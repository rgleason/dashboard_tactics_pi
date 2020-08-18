"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.dateToProtocolTimestamp = exports.currentTime = exports.useProcessHrtime = void 0;
const zeroPadding = '000000000';
let useHrTime = false;
function useProcessHrtime(use) {
    /* istanbul ignore else */
    if (!process.env.ROLLUP_BROWSER) {
        return (useHrTime = use && process && typeof process.hrtime === 'function');
    }
    else {
        return false;
    }
}
exports.useProcessHrtime = useProcessHrtime;
useProcessHrtime(true); // preffer node
let startHrMillis = undefined;
let startHrTime = undefined;
let lastMillis = Date.now();
let stepsInMillis = 0;
function nanos() {
    if (!process.env.ROLLUP_BROWSER && useHrTime) {
        const hrTime = process.hrtime();
        let millis = Date.now();
        if (!startHrTime) {
            startHrTime = hrTime;
            startHrMillis = millis;
        }
        else {
            hrTime[0] = hrTime[0] - startHrTime[0];
            hrTime[1] = hrTime[1] - startHrTime[1];
            // istanbul ignore next "cannot mock system clock, manually reviewed"
            if (hrTime[1] < 0) {
                hrTime[0] -= 1;
                hrTime[1] += 1000000000;
            }
            millis =
                startHrMillis +
                    hrTime[0] * 1000 +
                    Math.floor(hrTime[1] / 1000000);
        }
        const nanos = String(hrTime[1] % 1000000);
        return String(millis) + zeroPadding.substr(0, 6 - nanos.length) + nanos;
    }
    else {
        const millis = Date.now();
        if (millis !== lastMillis) {
            lastMillis = millis;
            stepsInMillis = 0;
        }
        else {
            stepsInMillis++;
        }
        const nanos = String(stepsInMillis);
        return String(millis) + zeroPadding.substr(0, 6 - nanos.length) + nanos;
    }
}
function micros() {
    if (!process.env.ROLLUP_BROWSER && useHrTime) {
        const hrTime = process.hrtime();
        const micros = String(Math.trunc(hrTime[1] / 1000) % 1000);
        return (String(Date.now()) + zeroPadding.substr(0, 3 - micros.length) + micros);
    }
    else {
        return String(Date.now()) + zeroPadding.substr(0, 3);
    }
}
function millis() {
    return String(Date.now());
}
function seconds() {
    return String(Math.floor(Date.now() / 1000));
}
/**
 * Exposes functions that creates strings that represent a timestamp that
 * can be used in the line protocol. Micro and nano timestamps are emulated
 * depending on the js platform in use.
 */
exports.currentTime = Object.freeze({
    s: seconds,
    ms: millis,
    us: micros,
    ns: nanos,
    seconds: seconds,
    millis: millis,
    micros: micros,
    nanos: nanos,
});
/**
 * dateToProtocolTimestamp provides converters for JavaScript Date to InfluxDB Write Protocol Timestamp. Keys are supported precisions.
 */
exports.dateToProtocolTimestamp = {
    s: (d) => `${Math.floor(d.getTime() / 1000)}`,
    ms: (d) => `${d.getTime()}`,
    us: (d) => `${d.getTime()}000`,
    ns: (d) => `${d.getTime()}000000`,
};
