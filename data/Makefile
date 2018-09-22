COMPRESSOR = ~/Pobrane/Compressonator_Linux_x86_64_3.0.105/CompressonatorCLI
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

%.bmp: %.jpg
	convert $< -set colorspace RGBA $@

%.bmp: %.jpeg
	convert $< -set colorspace RGBA $@

stormydays.ktx: $(CUBEMAP_INPUT)
	$(TO_KTX) --cubemap $@ $^

terrain-heightmap-01.ktx: terrain-heightmap-01.png
	$(COMPRESSOR) $< -fs ARGB_8888 -fd RGB_888 -nomipmap $@ > /dev/null

all: $(KTX_FILES) stormydays.ktx

clean:
	rm $(KTX_FILES) $(TMP_BMP_FILES)

.PHONY: clean all

