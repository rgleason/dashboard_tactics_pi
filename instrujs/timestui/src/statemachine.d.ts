
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
    setall(): undefined
    setalldb(): undefined
    rescan(): undefined
    selected(): undefined
    ackschema(): undefined
    getnew(): undefined
    newdata(): undefined
    errdata(): undefined
    retryget(): undefined
    ackschema(): undefined
    acksubs(): undefined
    chgconf(): undefined
    getfeet(): undefined
    nogetfeet(): undefined
    chkrdy(): undefined
    nochkrdy(): undefined
    usersl(): undefined
    nousersl(): undefined
    markack(): undefined
    sldataack(): undefined
    newsldata(): undefined
    sldstopack(): undefined
    mrkdataack(): undefined
    newmrkdata(): undefined
    mrkmteaack(): undefined
    mrkumteack(): undefined
    luminsty(): undefined
    swapdisp(): undefined
    closing(): undefined
    is(a:string): boolean
    is(): undefined
    state: string
    databusy: boolean
    conf: StateMachineConf
}

export function createStateMachine(): StateMachine
