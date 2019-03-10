/**                                                                                                                                                                                                          
 * @file ffmpegReader.h
 * @author Diego Nieto <diego.nieto.m@outlook.com>
 * @date 21 ago. 2017
 */

#ifndef FFMPEGREADER_H_
#define FFMPEGREADER_H_

extern "C"
{
    #include <libavformat/avformat.h>
}

#include <string>
#include <fstream>

using namespace std;

class FfmpegReader {
    public:
        FfmpegReader();

        virtual ~FfmpegReader() {}

        virtual int open(const std::string &filename);

        virtual int readFrame(AVPacket *packet);

        virtual int readAll();

        virtual void processFrame(const AVPacket &packet) = 0;

        virtual int close();

        virtual void prepareForDecode();

        virtual void decode_and_save_frame(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                const char *filename);

    protected:
        AVFormatContext *_ctx;
        AVCodec *_codec;
        AVCodecContext *_codecContext= NULL;
        AVFrame *_frame;  // To store the picture
        long long int _fRead;

        ofstream _packetsFile;
};



#endif /* FFMPEGREADER_H_ */
