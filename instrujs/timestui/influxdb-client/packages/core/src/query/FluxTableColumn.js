"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Column metadata class of a {@link http://bit.ly/flux-spec#table | flux table} column.
 */
class FluxTableColumn {
    /**
     * Creates a flux table column from an object supplied.
     * @param object - source object
     * @returns column instance
     */
    static from(object) {
        var _a;
        const retVal = new FluxTableColumn();
        retVal.label = object.label;
        retVal.dataType = object.dataType;
        retVal.group = Boolean(object.group);
        retVal.defaultValue = (_a = object.defaultValue) !== null && _a !== void 0 ? _a : '';
        return retVal;
    }
}
exports.default = FluxTableColumn;
