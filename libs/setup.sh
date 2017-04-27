#!/bin/bash

echo "Downloading and building portaudio..."
curl -sS http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz > portaudio.tgz
tar -xzvf portaudio.tgz
rm portaudio.tgz
cd portaudio
./configure && make
cd ..

echo "Downloading and building aqulia..."
git clone git://github.com/zsiciarz/aquila.git aquila
cd aquila
mkdir build
cd build
cmake ../../aquila/
make
make install