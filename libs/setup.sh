#!/bin/bash

set -e

TARGET="portaudio"
if [[ ! -d "${TARGET}" ]]; then
	if [[ ! -d "pa_stable_v190600_20161030.tgz" ]]; then
		echo "Downloading ${TARGET}..."
		wget http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz
	fi

	echo "Unpacking ${TARGET}..."
	tar -xzf pa_stable_v190600_20161030.tgz
	rm pa_stable_v190600_20161030.tgz

	echo "Building ${TARGET}..."
	cd "${TARGET}"
	./configure && make
	cd ..
	
	echo "Done building ${TARGET}!"
else
	echo "Building ${TARGET}..."
	cd "${TARGET}"
	make clean
	./configure && make
	cd ..
	echo "Done building ${TARGET}!"
fi

TARGET="aquila"
if [[ ! -d "${TARGET}" ]]; then
	echo "Downloading ${TARGET}..."
	git clone git://github.com/zsiciarz/aquila.git

	echo "Building ${TARGET}..."
	cd "${TARGET}" && mkdir build && cd build
	cmake .. && make
	cd ../..
	
	echo "Done building ${TARGET}!"
else
	echo "Building ${TARGET}..."
	cd "${TARGET}" && cd build
	rm -rf *
	cmake .. && make
	cd ../..
	echo "Done building ${TARGET}!"
fi

echo "Done building libraries!"

