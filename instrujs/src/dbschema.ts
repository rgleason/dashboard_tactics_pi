/* $Id: dbschema.ts, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

export default interface DbSchema {
    path: string;
    url: string;
    org: string;
    token: string;
    bucket: string;
    sMeasurement: string;
    sProp1: string;
    sProp2: string;
    sProp3: string;
    sField1: string;
    sField2: string;
    sField3: string;
}
