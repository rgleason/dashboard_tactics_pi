
import { conf } from '../../src/conf'
import {locationInfo} from '../../src/location'

export interface StateMachine {
    init(): undefined
    loaded(): undefined
    initok():undefined
    setid(): undefined
    nocfg(): undefined
    hascfg(): undefined
    mrkdataack(): undefined
    newmrkdata(): undefined
    mrkdstopack(): undefined
    actvrte(): undefined
    noroute(): undefined
    luminsty(): undefined
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
