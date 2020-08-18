"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const escape_1 = require("./util/escape");
/**
 * Point defines values of a single measurement.
 */
class Point {
    /**
     * Create a new Point with specified a measurement name.
     *
     * @param measurementName - the measurement name
     */
    constructor(measurementName) {
        this.tags = {};
        this.fields = {};
        if (measurementName)
            this.name = measurementName;
    }
    /**
     * Sets point's measurement.
     *
     * @param name - measurement name
     * @returns this
     */
    measurement(name) {
        this.name = name;
        return this;
    }
    /**
     * Adds a tag.
     *
     * @param name - tag name
     * @param value - tag value
     * @returns this
     */
    tag(name, value) {
        this.tags[name] = value;
        return this;
    }
    /**
     * Adds a boolean field.
     *
     * @param field - field name
     * @param value - field value
     * @returns this
     */
    booleanField(name, value) {
        this.fields[name] = value ? 'T' : 'F';
        return this;
    }
    /**
     * Adds an integer field.
     *
     * @param name - field name
     * @param value - field value
     * @returns this
     */
    intField(name, value) {
        if (typeof value !== 'number') {
            let val;
            if (isNaN((val = parseInt(String(value))))) {
                throw new Error(`Expected integer value for field ${name}, but got '${value}'!`);
            }
            value = val;
        }
        this.fields[name] = `${Math.floor(value)}i`;
        return this;
    }
    /**
     * Adds a number field.
     *
     * @param name - field name
     * @param value - field value
     * @returns this
     */
    floatField(name, value) {
        if (typeof value !== 'number') {
            let val;
            if (isNaN((val = parseFloat(value)))) {
                throw new Error(`Expected float value for field ${name}, but got '${value}'!`);
            }
            value = val;
        }
        this.fields[name] = String(value);
        return this;
    }
    /**
     * Adds a string field.
     *
     * @param name - field name
     * @param value - field value
     * @returns this
     */
    stringField(name, value) {
        if (value !== null && value !== undefined) {
            if (typeof value !== 'string')
                value = String(value);
            this.fields[name] = escape_1.escape.quoted(value);
        }
        return this;
    }
    /**
     * Sets point time. A string or number value can be used
     * to carry an int64 value of a precision that depends
     * on WriteApi, nanoseconds by default. An undefined value
     * generates a local timestamp using the client's clock.
     * An empty string can be used to let the server assign
     * the timestamp.
     *
     * @param value - point time
     * @returns this
     */
    timestamp(value) {
        this.time = value;
        return this;
    }
    /**
     * Creates an InfluxDB protocol line out of this instance.
     * @param settings - settings define the exact representation of point time and can also add default tags
     * @returns an InfxluDB protocol line out of this instance
     */
    toLineProtocol(settings) {
        if (!this.name)
            return undefined;
        let fieldsLine = '';
        Object.keys(this.fields)
            .sort()
            .forEach(x => {
            if (x) {
                const val = this.fields[x];
                if (fieldsLine.length > 0)
                    fieldsLine += ',';
                fieldsLine += `${escape_1.escape.tag(x)}=${val}`;
            }
        });
        if (fieldsLine.length === 0)
            return undefined; // no fields present
        let tagsLine = '';
        const tags = settings && settings.defaultTags
            ? Object.assign(Object.assign({}, settings.defaultTags), this.tags) : this.tags;
        Object.keys(tags)
            .sort()
            .forEach(x => {
            if (x) {
                const val = tags[x];
                if (val) {
                    tagsLine += ',';
                    tagsLine += `${escape_1.escape.tag(x)}=${escape_1.escape.tag(val)}`;
                }
            }
        });
        let time = this.time;
        if (settings && settings.convertTime) {
            time = settings.convertTime(time);
        }
        return `${escape_1.escape.measurement(this.name)}${tagsLine} ${fieldsLine}${time !== undefined ? ' ' + time : ''}`;
    }
    toString() {
        const line = this.toLineProtocol(undefined);
        return line ? line : `invalid point: ${JSON.stringify(this, undefined)}`;
    }
}
exports.default = Point;
