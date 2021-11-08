export interface iface{
    setFlag( elem: string, cmd: string ): undefined
    clearFlagById( elem: string ): undefined
    // setid
    regeventsetid( elem: string, eventid: Event ): undefined
    setid( newuid: string ): undefined
    getid(): string
    // setall
    regeventsetall( elem: string, eventid: Event ): undefined
    setall( alllist: string[] ): undefined
    getall(): string[]
    // setalldb
    regeventsetalldb( elem: string, eventid: Event ): undefined
    setalldb( alllist: string[] ): undefined
    getalldb(): string[]
    // setrescan
    regeventrescan( elem: string, eventid: Event ): undefined
    setrescan(): undefined
    // setselected
    regeventselected( elem: string, eventid: Event ): undefined
    setselected( newpath: string ): undefined
    getselected(): string
    // acksubs
    regeventacksubs( elem: string, eventid: Event ): undefined
    acksubs( forpath: string ): undefined
    getacksubs(): string
    // ackschema
    regeventackschema( elem: string, eventid: Event ): undefined
    ackschema( newschema: string ): undefined
    getdbschema(): string
    // setgetnew
    regeventgetnew( elem: string, eventid: Event ): undefined
    setgetnew(): undefined
    // newdata
    regeventnewdata( elem: string, eventid: Event ): undefined
    newdata( newvalue: number ): undefined
    getdata(): number
    // seterrdata
    regeventerrdata( elem: string, eventid: Event ): undefined
    seterrdata(): undefined
    // setchgconf
    regeventchgconf( elem: string, eventid: Event ): undefined
    setchgconf( newpath: string ): undefined
    getchgconf(): string
    // setretryget
    regeventretryget( elem: string, eventid: Event ): undefined
    setretryget(): undefined
    // setgetfeet
    regeventgetfeet( elem: string, eventid: Event ): undefined
    regeventnogetfeet( elem: string, eventid: Event ): undefined
    setgetfeet( newval: boolean ): undefined
    getgetfeet(): boolean
    // setchkrdy
    regeventchkrdy( elem: string, eventid: Event ): undefined
    regeventnochkrdy( elem: string, eventid: Event ): undefined
    setchkrdy( newval: boolean ): undefined
    getchkrdy(): boolean
    // setusersl
    regeventusersl( elem: string, eventid: Event ): undefined
    regeventnousersl( elem: string, eventid: Event ): undefined
    setusersl( newval: boolean ): undefined
    getusersl(): boolean
    // setmarkack
    regeventmarkack( elem: string, eventid: Event ): undefined
    setmarkack( newval: boolean ): undefined
    getmarkack(): boolean
    // setsldataack
    regeventsldataack( elem: string, eventid: Event ): undefined
    setsldataack( newval: boolean ): undefined
    getsldataack(): boolean
    // newsldata
    regeventsldataack( elem: string, eventid: Event ): undefined
    newsldata(
        dtogo: number, closestp: number,
        wbias: number, adv: number ): undefined
    getsldistancetogo(): number
    getslclosestpoint(): number
    getslwindbias(): number
    getsladvantage(): number
    // setsldstopack
    regeventsldstopack( elem: string, eventid: Event ): undefined
    setsldstopack( newval: boolean ): undefined
    getsldstopack(): boolean
    // setmrkdataack
    regeventmrkdataack( elem: string, eventid: Event ): undefined
    regeventnewmrkdata( elem: string, eventid: Event ): undefined
    setmrkdataack( newval: boolean ): undefined
    getmrkdataack(): boolean
    newmrkdata(
        actrte: boolean, irdy: boolean,
        m1name: string,  m2name: string, m3name: string,
        t1live: number,  t1short: number, t1long: number, cur1: number,
        t2live: number,  t2short: number, t2long: number, cur2: number,
        t3live: number,  t3short: number, t3long: number, cur3: number,
        brgbck: number
    ): undefined
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
    // setmrkmteaack
    regeventmrkmteaack( elem: string, eventid: Event ): undefined
    setmrkmteaack( newval: boolean ): undefined
    getmrkmteaack(): boolean
    // setmrkumteack
    regeventmrkumteack( elem: string, eventid: Event ): undefined
    setmrkumteack( newval: boolean ): undefined
    getmrkumteack(): boolean
    // setluminsty
    regeventchgconf( elem: string, eventid: Event ): undefined
    setluminsty( newlum: string ): undefined
    getluminsty(): string
    // setswapdisp
    regeventchgconf( elem: string, eventid: Event ): undefined
    setswapdisp( newdir: number ): undefined
    getswapdisp(): number
    // setclosing
    regeventclosing( elem: string, eventid: Event ): undefined
    setclosing(): undefined
    // graphwizdot
    setgraphwizdot( dot: string ): undefined
    getgraphwizdot(): string
}
