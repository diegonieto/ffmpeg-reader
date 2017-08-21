#include <iostream>

#include "ffmpegReader.h"

class FfmpegReaderShowPacket : public FfmpegReader {
    public:
        void processFrame(const AVPacket &packet);
};

void FfmpegReaderShowPacket::processFrame(const AVPacket &packet)
{
    const AVStream *stream = _ctx->streams[packet.stream_index];
    std::cout << "Stream type: " << stream->codec->codec_type << std::endl;
}

int main()
{
    FfmpegReaderShowPacket reader;
    std::string filePath = "/home/dnieto/Descargas/video.m4s";
    if (!reader.open(filePath)) {
        reader.readAll();
        reader.close();
    }

    std::cout << "Done" << std::endl;
    return 0;
}
