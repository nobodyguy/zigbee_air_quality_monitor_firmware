#!/bin/bash

echo "Applying Zephyr patches..."
git apply --unsafe-paths --verbose --directory /workdir/zephyr /workdir/project/patches/zephyr/*.patch