#!/usr/bin/env bash
COMPRESSOR=~/Pobrane/Compressonator_Linux_x86_64_3.0.105/CompressonatorCLI
CUBE_MAKER=~/code/github/ktx-cube-maker/build/ktx-cube-maker

# for extension in jpeg jpg; do
#     for filename in $(find . -name \*.$extension); do
#         convert "$filename" -colorspace RGB "${filename%.$extension}.png"
#     done
# done

for extension in png tga; do
    for filename in $(find . -name \*.$extension); do
        convert "$filename" -flip "$filename"
    done
done

for extension in png tga; do
    for filename in $(find . -name \*.$extension); do
        echo $filename
        #$COMPRESSOR -fd DXT1 "$filename" "${filename%.$extension}.ktx" > /dev/null
        $COMPRESSOR "$filename" -fs ARGB_8888 -fd ARGB_8888 -mipsize 1 "${filename%.$extension}.ktx"
    done
done

$CUBE_MAKER ./envmap_stormydays/stormydays_*.ktx stormydays.ktx

