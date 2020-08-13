export interface iface{
    setFlag( elem: string, cmd: string ): undefined
    clearFlagById( elem: string ): undefined
    getsldistancetogo(): number
    getslclosestpoint(): number
    getslwindbias(): number
    getsladvantage(): number
    getmarkack(): boolean
    getmrkhasactiveroute(): boolean
    getmrk1name(): string
    getmrkinstrurdy(): boolean
    getmrk1twalive(): number
    getmrk1twashort(): number
    getmrk1twalong(): number
    getmrk1current(): number
    getmrk2name(): string
    getmrk2twalive(): number
    getmrk2twashort(): number
    getmrk2twalong(): number
    getmrk2current(): number
    getmrk3name(): string
    getmrk3twalive(): number
    getmrk3twashort(): number
    getmrk3twalong(): number
    getmrk3current(): number
    getmrkbrgback(): number
}
