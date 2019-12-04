#! /bin/sh
dir=$(cd $(dirname $0)&&pwd)
cd $dir
echo $dir
~/Development/emsdk/upstream/emscripten/em++ \
	-O3 \
	-std=c++11 \
	-Wc++11-extensions \
	--memory-init-file 0 \
	-s VERBOSE=1 \
	-s WASM=0 \
	-s ASSERTIONS=1 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s EXPORTED_FUNCTIONS="['_setup']" \
	-s EXTRA_EXPORTED_RUNTIME_METHODS="['cwrap']" \
	-s TOTAL_MEMORY=33554432 \
	main.cpp \
	-o ./docs/libs.js