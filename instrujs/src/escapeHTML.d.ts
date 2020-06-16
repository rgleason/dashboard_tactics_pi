export default class sanitizer{
    constructor()
    escapeHTML(a: string, i1?: number, i2?: number, i3?: number): string
    createSafeHTML(a: string, i1?: number, i2?: number, i3?: number): string
    unwrapSafeHTML(s: string): string
}
