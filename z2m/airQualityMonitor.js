const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const extend = require('zigbee-herdsman-converters/lib/extend');
const e = exposes.presets;
const ea = exposes.access;

const definition = {
    zigbeeModel: ['AirQualityMonitor_v1.0'],
    model: 'AirQualityMonitor_v1.0',
    vendor: 'DIY',
    description: 'Air quality monitor',
    fromZigbee: [fz.temperature, fz.humidity],
    toZigbee: [], // Should be empty, unless device can be controlled (e.g. lights, switches).
    exposes: [e.temperature(), e.humidity()],
};

module.exports = definition;