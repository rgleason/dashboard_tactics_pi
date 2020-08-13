/* $Id: confupgrade.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

// An actor to upgrade a configuration
var alertsenabled = window.instrustat.alerts
var dbglevel = window.instrustat.debuglevel

/* ------------------------------------------------------------------------------------------- */
export function upgradeConfVersion( loadecConfVersion, loadedConf ) {
    return null // for now, only one version possible exists: 1.
}
