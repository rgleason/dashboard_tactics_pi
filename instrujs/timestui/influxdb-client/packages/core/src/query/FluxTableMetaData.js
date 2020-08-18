"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.typeSerializers = void 0;
const errors_1 = require("../errors");
const identity = (x) => x;
/**
 * A dictionary of serializers of particular types returned by a flux query.
 * See {@link https://v2.docs.influxdata.com/v2.0/reference/syntax/annotated-csv/#valid-data-types }
 */
exports.typeSerializers = {
    boolean: (x) => x === 'true',
    unsignedLong: (x) => (x === '' ? null : +x),
    long: (x) => (x === '' ? null : +x),
    double: (x) => (x === '' ? null : +x),
    string: identity,
    base64Binary: identity,
    dateTime: (x) => (x === '' ? null : x),
    duration: (x) => (x === '' ? null : x),
};
/**
 * Represents metadata of a {@link http://bit.ly/flux-spec#table | flux table}.
 */
class FluxTableMetaData {
    constructor(columns) {
        columns.forEach((col, i) => (col.index = i));
        this.columns = columns;
    }
    /**
     * Gets columns by name
     * @param label - column label
     * @returns table column
     * @throws IllegalArgumentError if column is not found
     **/
    column(label) {
        for (let i = 0; i < this.columns.length; i++) {
            const col = this.columns[i];
            if (col.label === label)
                return col;
        }
        throw new errors_1.IllegalArgumentError(`Column ${label} not found!`);
    }
    /**
     * Creates an object out of the supplied values with the help of columns .
     * @param values - a row with data for each column
     */
    toObject(values) {
        var _a;
        const acc = {};
        for (let i = 0; i < this.columns.length && i < values.length; i++) {
            let val = values[i];
            const column = this.columns[i];
            if (val === '' && column.defaultValue) {
                val = column.defaultValue;
            }
            acc[column.label] = ((_a = exports.typeSerializers[column.dataType]) !== null && _a !== void 0 ? _a : identity)(val);
        }
        return acc;
    }
}
exports.default = FluxTableMetaData;
