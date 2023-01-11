const fz = require("zigbee-herdsman-converters/converters/fromZigbee");
const tz = require("zigbee-herdsman-converters/converters/toZigbee");
const exposes = require("zigbee-herdsman-converters/lib/exposes");
const reporting = require("zigbee-herdsman-converters/lib/reporting");
const extend = require("zigbee-herdsman-converters/lib/extend");
const constants = require("zigbee-herdsman-converters/lib/constants");
const e = exposes.presets;
const ea = exposes.access;

const definition = {
    zigbeeModel: ["AirQualityMonitor_v1.0"],
    model: "AirQualityMonitor_v1.0",
    vendor: "Custom devices (DiY)",
    description: "Air quality monitor (https://github.com/nobodyguy/zigbee_air_quality_monitor_firmware)",
    fromZigbee: [fz.temperature, fz.humidity, fz.co2],
    toZigbee: [], // Should be empty, unless device can be controlled (e.g. lights, switches).
    exposes: [e.temperature(), e.humidity(), e.co2()],
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpointID = 1;
        const endpoint = device.getEndpoint(endpointID);
        const clusters = ["msTemperatureMeasurement", "msRelativeHumidity", "msCO2"];
        await reporting.bind(endpoint, coordinatorEndpoint, clusters);
        await reporting.temperature(endpoint, {min: 1, max: constants.repInterval.MINUTES_5, change: 10}); // 0.1 degree change
        await reporting.humidity(endpoint, {min: 1, max: constants.repInterval.MINUTES_5, change: 10}); // 0.1 % change
        await reporting.co2(endpoint, {min: 5, max: constants.repInterval.MINUTES_5, change: 0.00005}); // 50 ppm change
    },
};

module.exports = definition;
