#!/usr/bin/env bash
apt-get install -y libsdl2-image-dev

apt-get install -y unzip

mkdir external
cd external

wget https://github.com/g-truc/gli/releases/download/0.8.2.0/gli-0.8.2.0.zip
unzip gli-0.8.2.0.zip
rm gli-0.8.2.0.zip

cd ..
