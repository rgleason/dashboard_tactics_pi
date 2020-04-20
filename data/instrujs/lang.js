/* $Id: lang.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in / WebKit based instruments
 * Licensed under MIT - see distribution.
 */
/* >>>> If you plan to modify this file, please make a backup first! <<<< */
/* ----   Javascript is friendly: a smallest error here stops it!    ---- */
/* ----     New language? Contribute/report https://git.io/JejKQ     ---- */

var instrulang = {
    errCommonJs : 'ERROR in common.js!',
    errConfStatJs : 'ERROR in confstat.js!',
    savingNotAvailable : 'Saving of settings not available.',
    systemPolicyPrevents : 'System policy prevents local storage',
    theProtocolIs : 'The protocol (ini/conf) for instrument is',
    tryAnotherProtocol : 'Try another protocol or static configuration.',
    rightClickHereToSubscribe: '<-- right click here to subscribe',
    loading: 'Loading...',
    errSubscriptionAck1: 'ERROR: requested to subscribe to path',
    errSubscriptionAck2: 'but got path',
    errNofConfKeysDoNotMatch1: 'ERROR: got (probably static) configuration with incorrect',
    errNofConfKeysDoNotMatch2: 'number of keys and values.',
    errNofConfKeysDoNotMatch3: 'Expecting',
    errNofConfKeysDoNotMatch4: 'got',
    alertTitle:   '*********** ALERT ***********',
    alertLolimit: 'passed under the lower limit with value:',
    alertHilimit: 'passed over the high limit with value:',
    menuPathWaitMsg: 'loading.wait', // keep the dot
    menuPathRunningReconfig: 'running.reconfigure', // keep the dot
    pathHasNoDescription1: 'has no configuration, will not subscribe without one.',
    pathHasNoDescription2: 'See data/instrujs/common.js to add your own definition.',
    pathHasNoDescription3: 'Please contribute it back here: https://git.io/JejKQ',
    noDataFromDbQry1: 'No data returned by the database query. Check:',
    noDataFromDbQry2: '- is DashT streaming out right now',
    noDataFromDbQry3: '- the time on the InfluxDB server',
    dataFromDbNoTime: 'Data returned by database query does not contain _time fields',
    dataFromDbBadTime: 'Data returned by database query contains unknwon _time format',
    dataFromDbNoValue: 'Data returned by database query does not contain _value fields',
    dataFromDbBadValue: 'Data returned by database query contains a non-number _value field',
    dataFunctionAbbrv: 'fn():'
}
window.instrulang = instrulang
