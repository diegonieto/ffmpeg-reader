FFMPEG_PATH=/home/dnieto/ffmpeg-ins/
PC_64=/usr/local/
# PC64
LIBS=-L ${PC_64}lib/
INCS=-I ${PC_64}include/

# PC
# LIBS=-L ${FFMPEG_PATH}lib/
# INCS=-I ${FFMPEG_PATH}include/


DBG_FLAGS=-ggdb3

all:
	g++ ${DBG_FLAGS} ${INCS} ${LIBS} testMain.cpp ffmpegReader.cpp -lavformat -lavutil -lavcodec -lswresample 
