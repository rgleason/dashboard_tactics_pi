export interface iface{
    setFlag( elem: string, cmd: string ): undefined
    clearFlagById( elem: string ): undefined
    getsldistancetogo(): number
    getslclosestpoint(): number
    getslwindbias(): number
    getsladvantage(): number
}
