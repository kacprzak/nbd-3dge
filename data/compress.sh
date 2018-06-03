#!/usr/bin/env bash
COMPRESSOR=~/Pobrane/Compressonator_Linux_x86_64_3.0.105/CompressonatorCLI

for extension in png jpeg tga; do
    for filename in $(find . -name \*.$extension); do
        echo $filename
        $COMPRESSOR -fd DXT1 "$filename" "${filename%.$extension}.ktx" > /dev/null
    done
done
