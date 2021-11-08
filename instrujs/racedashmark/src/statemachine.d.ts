
import { conf } from '../../src/conf'
import {locationInfo} from '../../src/location'

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
    actvrte(): undefined
    noroute(): undefined
    newmrkdata(): undefined
    mrkmteaack(): undefined
    mrkumteack(): undefined
    luminsty(): undefined
    swapdisp(): undefined
    closing(): undefined
    is(a:string): boolean
    is(): undefined
    uid: string
    perspath: boolean
    instrurdy: boolean
    activeroute: boolean
    luminosity: string
    locInfo : locationInfo
    conf: conf
    state: string
}

export function createStateMachine(): StateMachine
