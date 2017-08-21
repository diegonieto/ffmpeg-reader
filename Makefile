FFMPEG_PATH=/home/dnieto/ffmpeg-ins/
LIBS=${FFMPEG_PATH}lib/
INCS=${FFMPEG_PATH}include/

DBG_FLAGS=-ggdb3

all:
	g++ ${DBG_FLAGS} -I ${INCS} -L ${LIBS} testMain.cpp ffmpegReader.cpp -lavformat -lavutil -lavcodec -lswresample 
