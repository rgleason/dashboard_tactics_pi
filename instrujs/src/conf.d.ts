export interface conf{
    version: number
    path: string
    title: string
    symbol: string
    unit: string
    display: string
    decimals: number
    minval: number
    loalert: number
    hialert: number
    maxval: number
    multiplier: number
    divider: number
    offset: number
    dbfunc: string
    dbnum: number
    wrnmsg: boolean
}

export function createEmptyConf(): conf
export function getConf( fsm: any ): undefined
export function getPathDefaultsIfNew( fsm: any ): undefined
export function memorizeSettings( fsm: any ): undefined
export function clearConf( fsm: any ): undefined
export function prepareConfHalt( fsm: any ): undefined
