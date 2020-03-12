/** InfluxDB v2 URL */
export var url = process.env['INFLUXDB_URL'] || 'http://localhost:9999'
/** InfluxDB authorization token */
export var token = process.env['INFLUXDB_TOKEN'] || 'my-token'
/** Organization within InfluxDB URL  */
export var org = process.env['INFLUXDB_ORG'] || 'my-org'
/**InfluxDB bucket used in examples  */
export var bucket = 'my-bucket'
// ONLY onboarding example
/**InfluxDB user  */
export var username = 'my-user'
/**InfluxDB password  */
export var password = 'my-password'
