#!/usr/bin/env bash
mkdir external
cd external

wget https://github.com/g-truc/gli/archive/master.zip
unzip master.zip
rm master.zip
mv gli-master gli

cd ..
