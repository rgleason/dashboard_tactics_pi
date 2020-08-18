"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.defaultRowMapping = void 0;
function defaultRowMapping(values, tableMeta) {
    return tableMeta.toObject(values);
}
exports.defaultRowMapping = defaultRowMapping;
