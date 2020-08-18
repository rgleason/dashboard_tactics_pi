"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.toLineObserver = void 0;
const LineSplitter_1 = require("../util/LineSplitter");
const FluxTableColumn_1 = require("../query/FluxTableColumn");
const FluxTableMetaData_1 = require("../query/FluxTableMetaData");
function toLineObserver(consumer) {
    const splitter = new LineSplitter_1.default().withReuse();
    let columns;
    let expectMeta = true;
    let firstColumnIndex = 0;
    let lastMeta;
    return {
        error(error) {
            consumer.error(error);
        },
        next(line) {
            if (line === '') {
                expectMeta = true;
                columns = undefined;
            }
            else {
                const values = splitter.splitLine(line);
                const size = splitter.lastSplitLength;
                if (expectMeta) {
                    // create columns
                    if (!columns) {
                        columns = new Array(size);
                        for (let i = 0; i < size; i++) {
                            columns[i] = new FluxTableColumn_1.default();
                        }
                    }
                    if (!values[0].startsWith('#')) {
                        // fill in column names
                        if (values[0] === '') {
                            firstColumnIndex = 1;
                            columns = columns.slice(1);
                        }
                        else {
                            firstColumnIndex = 0;
                        }
                        for (let i = firstColumnIndex; i < size; i++) {
                            columns[i - firstColumnIndex].label = values[i];
                        }
                        lastMeta = new FluxTableMetaData_1.default(columns);
                        expectMeta = false;
                    }
                    else if (values[0] === '#datatype') {
                        for (let i = 1; i < size; i++) {
                            columns[i].dataType = values[i];
                        }
                    }
                    else if (values[0] === '#default') {
                        for (let i = 1; i < size; i++) {
                            columns[i].defaultValue = values[i];
                        }
                    }
                    else if (values[0] === '#group') {
                        for (let i = 1; i < size; i++) {
                            columns[i].group = values[i][0] === 't';
                        }
                    }
                }
                else {
                    consumer.next(values.slice(firstColumnIndex, size), lastMeta);
                }
            }
        },
        complete() {
            consumer.complete();
        },
        useCancellable(cancellable) {
            if (consumer.useCancellable)
                consumer.useCancellable(cancellable);
        },
    };
}
exports.toLineObserver = toLineObserver;
