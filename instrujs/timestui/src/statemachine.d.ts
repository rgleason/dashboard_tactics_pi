
export interface StateMachineConf {
    path: string
}

export interface StateMachine {
    init(): undefined
    loaded(): undefined
    initok(): undefined
    setid(): undefined
    hascfg(): undefined
    nocfg(): undefined
    setalldb(): undefined
    rescan(): undefined
    selected(): undefined
    ackschema(): undefined
    getnew(): undefined
    getlaunch(): undefined
    newdata(): undefined
    errdata(): undefined
    retryget(): undefined
    ackschema(): undefined
    chgconf(): undefined
    luminsty(): undefined
    swapdisp(): undefined
    closing(): undefined
    is(a:string): boolean
    is(): undefined
    state: string
    conf: StateMachineConf
}

export function createStateMachine(): StateMachine
