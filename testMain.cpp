#include <iostream>

#include "ffmpegReader.h"

class FfmpegReaderShowPacket : public FfmpegReader {
    public:
        void processFrame(const AVPacket &packet);
};

void FfmpegReaderShowPacket::processFrame(const AVPacket &packet)
{
    const AVStream *stream = _ctx->streams[packet.stream_index];
    //std::cout << "Stream type: " << stream->codec->codec_type
    //        << " is key frame: " << (packet.flags & AV_PKT_FLAG_KEY)
    //        << " duration " << packet.duration
    //        << " pts: " << packet.pts
    //        << " dts: " << packet.dts << std::endl;
	//av_pkt_dump2(stdout, &packet, 0, _ctx->streams[0]);

    decode_and_save_frame(_codecContext, _frame, (AVPacket*)&packet, "/home/dnieto/picture");

	_packetsFile.write((const char*)packet.data, packet.size);
}

int main(int argc, char **argv)
{
    FfmpegReaderShowPacket reader;
    //std::string filePath = "/home/dnieto/Descargas/key-mp4/trickplays.mp4";
    //std::string filePath = "/home/dnieto/Documentos/DASH/TTML/TTML.mp4";
    //std::string filePath = "/home/dnieto/Descargas/GoMalta/1.ts";
    //std::string filePath = "/home/dnieto/Descargas/drm-fail.mp4";
    std::string filePath = "/tmp/0.ts";
    if (argc == 2) {
    	filePath = argv[1];
    }
    if (!reader.open(filePath)) {
        reader.readAll();
        reader.close();
    } else {
        std::cout << "Error when opening /tmp/0.ts" << std::endl;
    }

    std::cout << "Done" << std::endl;
    return 0;
}
