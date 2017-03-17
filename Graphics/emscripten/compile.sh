~/Downloads/emsdk_portable/emscripten/master/emmake make
~/Downloads/emsdk_portable/emscripten/master/emcc -lGL -lEGL -lX11 \
--preload-file basemap.tga \
emscripten-webgl.bc -o emscripten-webgl.html