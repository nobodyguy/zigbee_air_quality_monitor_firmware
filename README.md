# Zigbee air quality monitor firmware
Opensource HA compatible air quality monitor with CO2, temperature and humidity measurements.\
Based on **nRF Connect SDK v2.2.99**.

## Building
`west build -b xiao_ble`

## Flashing
`west flash --runner blackmagicprobe`

## Debugging
GDB:\
`west debug --runner blackmagicprobe`

Or you can use this `./.vscode/launch.json` config for Cortex-Debug extension:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Zephyr nRF52840",
            "cwd": "${workspaceFolder}",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "bmp",
            "BMPGDBSerialPort": "/dev/ttyACM0",
            "armToolchainPath": "${workspaceFolder}/../zephyr-sdk/arm-zephyr-eabi/bin",
            "gdbPath": "${workspaceFolder}/../zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb",
            "interface": "swd",
            "runToEntryPoint": "main",
            "executable": "${workspaceFolder}/build/zephyr/zephyr.hex",
            "preLaunchCommands": [
                "add-symbol-file ${workspaceFolder}/build/zephyr/zephyr.elf"
              ]
        },
    ]
}
```
