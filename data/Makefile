COMPRESSOR = C:/Program Files/Compressonator/CompressonatorCLI.exe
CMFT=~/code/github/cmft/_build/linux64_gcc/bin/cmftRelease
#CUBE_MAKER = ~/code/github/ktx-cube-maker/build/ktx-cube-maker
TO_KTX = ~/toktx

START_DIR = .
#START_DIR = envmap_stormydays
OUT_FORMAT = DXT5 -mipsize 1

INPUT_PNG_FILES = $(shell find $(START_DIR) -name \*.png)
INPUT_JPG_FILES = $(shell find $(START_DIR) -name \*.jpg)
INPUT_JPEG_FILES = $(shell find $(START_DIR) -name \*.jpeg)
INPUT_TGA_FILES = $(shell find $(START_DIR) -name \*.tga)

TMP_BMP_FILES = $(INPUT_JPG_FILES:.jpg=.bmp) $(INPUT_JPEG_FILES:.jpeg=.bmp)
KTX_FILES = $(INPUT_PNG_FILES:.png=.ktx) $(INPUT_TGA_FILES:.tga=.ktx) $(TMP_BMP_FILES:.bmp=.ktx)

CUBEMAP_INPUT = envmap_stormydays/stormydays_ft.ppm \
envmap_stormydays/stormydays_bk.ppm \
envmap_stormydays/stormydays_up.ppm \
envmap_stormydays/stormydays_dn.ppm \
envmap_stormydays/stormydays_rt.ppm \
envmap_stormydays/stormydays_lf.ppm

%.ktx: %.tga
	$(COMPRESSOR) $< -fs ARGB_8888 -fd $(OUT_FORMAT) $@ > /dev/null

%.ktx: %.png
	$(COMPRESSOR) $< -fs ARGB_8888 -fd $(OUT_FORMAT) $@ > /dev/null

%.ktx: %.bmp
	$(COMPRESSOR) $< -fs ARGB_8888 -fd $(OUT_FORMAT) $@ > /dev/null

%.ppm: %.tga
	convert $< $@

envmap_stormydays/stormydays_%.ppm: envmap_stormydays/stormydays_%.tga
	convert -resize 50% $< $@

%.bmp: %.jpg
	convert $< -set colorspace RGBA $@

%.bmp: %.jpeg
	convert $< -set colorspace RGBA $@

all: $(KTX_FILES) stormydays.ktx stormydays_rad.ktx stormydays_irr.ktx

stormydays.ktx: $(CUBEMAP_INPUT)
	$(TO_KTX) --cubemap $@ $^

stormydays_rad.ktx: stormydays.ktx
	$(CMFT) --input $< --filter radiance --lightingModel phongbrdf --numCpuProcessingThreads 6 --mipCount 9 --output0 stormydays_rad --output0params ktx,rgb16f,cubemap

stormydays_irr.ktx: stormydays.ktx
	$(CMFT) --input $< --filter irradiance --numCpuProcessingThreads 6 --mipCount 1 --output0 stormydays_irr --output0params ktx,rgb16f,cubemap

terrain-heightmap-01.ktx: terrain-heightmap-01.png
	$(COMPRESSOR) $< -fs ARGB_8888 -fd RGB_888 -nomipmap $@ > /dev/null

clean:
	rm $(KTX_FILES) $(TMP_BMP_FILES)

zip:
	zip -r9 data.zip brdfLUT.ktx BoomBox SciFiHelmet ubuntu.fnt ubuntu.ktx stormydays*.ktx assets.xml scene.xml -x **/*.png

data.zip:
	wget http://gdurl.com/3JGL7/download/14ad53a3385ae8a331c52621fb15e226 -O data.zip

.PHONY: clean all zip

