
export interface StateMachineConf {
    path: string
}

export interface StateMachine {
    init(): undefined
    loaded(): undefined
    btnarmw(): undefined
    nousersl(): undefined
    usersl(): undefined
    btnportd1(): undefined
    oneport(): undefined
    btnstbdd1(): undefined
    onestbd(): undefined
    btnportd2(): undefined
    twoport(): undefined
    btnstbdd2(): undefined
    twostbd(): undefined
    markack(): undefined
    sldataack(): undefined
    newsldata(): undefined
    sldstopack(): undefined
    btnfaded(): undefined
    btnarmc(): undefined
    btnarma(): undefined
    luminsty(): undefined
    closing(): undefined
    is(a:string): boolean
    is(): undefined
    uid: string
    perspath: boolean
    gotusrsl: boolean
    stbdmark: boolean
    portmark: boolean
    luminosity: string
    conf: StateMachineConf
    state: string
}

export function createStateMachine(): StateMachine
