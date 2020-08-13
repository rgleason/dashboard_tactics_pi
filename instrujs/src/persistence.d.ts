// module level functions
export function loadConf( cid: string, locProtocol: string ): string
export function saveConf( cid: string, confObj: string ): boolean
// module internals
export function saveObj( cid: string, cobj: string ): boolean
export function getObj( cid: string ): string
export function deleteObj( cid: string ): boolean
export function saveParam( cname: string, cid: string, cvalue: string, inexdays: string ): boolean
export function getParam( cname: string, cid: string ): string
export function deleteParam( cname: string, cid: string ): boolean
export function saveCookieObj( cid: string, cobj: string ): boolean
export function getCookieObj( cid: string ): string
export function deleteCookieObj( cid: string ): boolean
export function selfTest( locProtocol: string ): undefined
