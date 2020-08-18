"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const pureJsChunkCombiner = {
    concat(first, second) {
        const retVal = new Uint8Array(first.length + second.length);
        retVal.set(first);
        retVal.set(second, first.length);
        return retVal;
    },
    toUtf8String(chunk, start, end) {
        // see https://en.wikipedia.org/wiki/UTF-8 for details
        let c1, c2, c3, c4;
        let out = '';
        let i = start;
        while (i < end) {
            c1 = chunk[i++];
            switch (c1 >> 4) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    // 0xxxxxxx
                    out += String.fromCharCode(c1);
                    break;
                case 12:
                case 13:
                    // 110x xxxx   10xx xxxx
                    c2 = chunk[i++];
                    out += String.fromCharCode(((c1 & 0x1f) << 6) | (c2 & 0x3f));
                    break;
                case 14:
                    // 1110 xxxx  10xx xxxx  10xx xxxx
                    c2 = chunk[i++];
                    c3 = chunk[i++];
                    out += String.fromCharCode(((c1 & 0x0f) << 12) | ((c2 & 0x3f) << 6) | (c3 & 0x3f));
                    break;
                case 15:
                    // 1111 0xxx  10xx xxxx  10xx xxxx 10xx xxxx
                    c2 = chunk[i++];
                    c3 = chunk[i++];
                    c4 = chunk[i++];
                    out += String.fromCodePoint(((c1 & 0x07) << 18) |
                        ((c2 & 0x3f) << 12) |
                        ((c3 & 0x3f) << 6) |
                        (c4 & 0x3f));
                    break;
            }
        }
        return out;
    },
    copy(chunk, start, end) {
        const retVal = new Uint8Array(end - start);
        retVal.set(chunk.slice(start, end));
        return retVal;
    },
};
exports.default = pureJsChunkCombiner;
