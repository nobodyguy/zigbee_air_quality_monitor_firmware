#!/bin/bash

for dir in ./patches/*/ ; do
    MODULE=$(basename $dir)
    echo "Applying ${MODULE} patches..."
    git apply --whitespace=fix --unsafe-paths --verbose --directory /workdir/${MODULE} /workdir/project/patches/${MODULE}/*.patch
done