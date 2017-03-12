java -jar ../../closure-compiler-v20170218.jar \
../src/base.js ../src/tdl/string.js ../src/tdl/log.js ../src/tdl/misc.js ../src/tdl/webgl.js \
../src/tdl/buffers.js ../src/tdl/fast.js ../src/tdl/fps.js ../src/tdl/math.js \
../src/tdl/models.js ../src/tdl/primitives.js ../src/tdl/programs.js ../src/tdl/textures.js \
../src/perf-harness.js ../src/lots-o-objects-draw-elements.js  \
--create_source_map webgl.closure.js.map --source_map_format=V3 \
--js_output_file webgl.closure.js