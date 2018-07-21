#!/usr/bin/env bash
apt-get install -y libegl1-mesa-dev libxcursor-dev libxinerama-dev libxi-dev libxrandr-dev libxss-dev
apt-get install libc++-dev
apt-get install -y unzip

mkdir external
cd external

wget https://github.com/g-truc/gli/archive/master.zip
unzip master.zip
rm master.zip
mv gli-master gli

cd ..
