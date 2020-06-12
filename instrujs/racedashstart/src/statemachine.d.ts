
export interface StateMachineConf {
    path: string
}

export interface StateMachine {
    init(): undefined
    loaded(): undefined
    btnarmw(): undefined
    luminsty(): undefined
    closing(): undefined
    is(a:string): boolean
    is(): undefined
    state: string
    databusy: boolean
    conf: StateMachineConf
}

export function createStateMachine(): StateMachine
