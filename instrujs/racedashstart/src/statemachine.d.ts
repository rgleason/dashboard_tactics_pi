
export interface StateMachineConf {
    path: string
}

export interface StateMachine {
    init(): undefined
    loaded(): undefined
    btnarmw(): undefined
    btnportd1(): undefined
    btnstbdd1(): undefined
    btnportd2(): undefined
    btnstbdd2(): undefined
    btnarmc(): undefined
    btnarma(): undefined
    luminsty(): undefined
    closing(): undefined
    is(a:string): boolean
    is(): undefined
    state: string
    databusy: boolean
    conf: StateMachineConf
}

export function createStateMachine(): StateMachine
