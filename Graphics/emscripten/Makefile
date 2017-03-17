INCDIR=-I./Common
LIBS=-lGLESv2 -lEGL -lm -lX11

COMMONSRC=./Common/esShader.c    \
          ./Common/esTransform.c \
          ./Common/esShapes.c    \
          ./Common/esUtil.c

COMMONHRD=esUtil.h
GL_SRC=webgl.c

default: emscripten-webgl.bc

clean:
	find . -name "emscripten-webgl*" | xargs rm -f

emscripten-webgl.bc: ${COMMONSRC} ${COMMONHDR} ${GL_SRC}
	$(CC) ${COMMONSRC} ${GL_SRC} -o ./$@ ${INCDIR} ${LIBS}

emscripten-webgl-debug: ${COMMONSRC} ${COMMONHDR} ${GL_SRC}
	$(CC) ${COMMONSRC} ${GL_SRC} -g -o ./$@ ${INCDIR} ${LIBS}