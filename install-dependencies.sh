#!/usr/bin/env bash
apt-get install -y libglew-dev libsdl2-dev libsdl2-image-dev libegl1-mesa-dev libgles2-mesa-dev libpthread-stubs0-dev libbullet-dev libboost-dev libboost-log-dev libboost-program-options-dev libboost-test-dev libboost-regex-dev

apt-get install -y unzip

mkdir external
cd external

wget https://github.com/g-truc/glm/releases/download/0.9.9.0/glm-0.9.9.0.zip
unzip glm-0.9.9.0.zip
rm glm-0.9.9.0.zip

wget https://github.com/g-truc/gli/releases/download/0.8.2.0/gli-0.8.2.0.zip
unzip gli-0.8.2.0.zip
rm gli-0.8.2.0.zip

cd ..
