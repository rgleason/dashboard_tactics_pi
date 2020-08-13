export interface locationInfo{
    href: string,
    protocol: string,
    hostname: string,
    domain: string,
    port: string
}

export default function getLocInfo(): locationInfo
