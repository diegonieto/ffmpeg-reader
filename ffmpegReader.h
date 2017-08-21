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


    protected:
        AVFormatContext *_ctx;
};



#endif /* FFMPEGREADER_H_ */
