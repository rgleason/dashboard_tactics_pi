/** InfluxDB v2 URL */
export var url = process.env['INFLUXDB_URL'] || 'http://localhost:8089'
/** InfluxDB authorization token */
export var token = process.env['INFLUXDB_TOKEN'] || 'iupP8J7WFVwcOV2Bk1P1nuHrOMYhSVi8whJIQnLGrIhyiobX3LpMhN0bgffEh5av2SQfvpk-9H_UHT2Z15u6zw=='
/** Organization within InfluxDB URL  */
export var org = process.env['INFLUXDB_ORG'] || 'ocarina'
/**InfluxDB bucket used in examples  */
export var bucket = 'nmea'
// ONLY onboarding example
/**InfluxDB user  */
export var username = 'my-user'
/**InfluxDB password  */
export var password = 'my-password'
