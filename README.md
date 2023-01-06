# Zigbee air quality monitor firmware
Opensource HA compatible air quality monitor with CO2, temperature and humidity measurements.\
Based on **nRF Connect SDK v2.2.99**.

## Building
`west build -b xiao_ble`

## Flashing
`west flash --runner blackmagicprobe`

## Debugging
`west debug --runner blackmagicprobe`\
*TODO add cortex-debug config.*
