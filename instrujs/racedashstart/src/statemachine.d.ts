
import { conf } from '../../src/conf'
import {locationInfo} from '../../src/location'

export interface StateMachine {
    init(): undefined
    loaded(): undefined
    initok():undefined
    setid(): undefined
    nocfg(): undefined
    hascfg(): undefined
    getfeet(): undefined
    nogetfeet(): undefined
    btnarmw(): undefined
    chkrdy(): undefined
    nochkrdy(): undefined
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
    setall(): undefined
    setalldb(): undefined
    rescan(): undefined
    selected(): undefined
    acksubs(): undefined
    ackschema(): undefined
    getnew(): undefined
    newdata(): undefined
    chgconf(): undefined
    retryget(): undefined
    mrkdataack(): undefined
    newmrkdata(): undefined
    mrkmteaack(): undefined
    mrkumteack(): undefined
    swapdisp(): undefined
    luminsty(): undefined
    closing(): undefined
    is(a:string): boolean
    is(): undefined
    uid: string
    perspath: boolean
    instrurdy: boolean
    feet: boolean
    gotusrsl: boolean
    stbdmark: boolean
    portmark: boolean
    luminosity: string
    locInfo : locationInfo
    conf: conf
    state: string
}

export function createStateMachine(): StateMachine
