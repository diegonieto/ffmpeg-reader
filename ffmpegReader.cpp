/**                                                                                                                                                                                                          
 * @file ffmpegReader.cpp
 * @author Diego Nieto <diego.nieto@mirada.tv>
 * @date 21 ago. 2017
 *
 * @section LICENSE
 * Copyright (c) 2016 Mirada. All rights reserved.
 *
 * This Software is provided "AS IS," without a warranty of any kind.  All
 * express or implied representations and warranties, including any implied
 * warranty of merchantability, fitness for a particular purpose or
 * non-infringement, are hereby excluded.
 *
 * Mirada shall not be liable for any damages suffered by licensee or any third
 * party as a result of using or distributing software. In no event will Fresh
 * liable for any lost revenue, profit or data, or for direct, indirect,
 * special, consequential, incidental or punitive damages, however caused and
 * regardless of the theory of liability, arising out of the use of or
 * inability to use software, even if he has been advised of the possibility of
 * such damages.
 */

#include "ffmpegReader.h"

#include <iostream>

#define INBUF_SIZE 4096

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;
    f = fopen(filename,"w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

/**
 * Using decode example from ffmpeg
 */
void FfmpegReader::decode_and_save_frame(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                   const char *filename)
{
    char buf[1024];
    int ret;
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        }
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        printf("saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);
        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
        pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, buf);
    }
}

FfmpegReader::FfmpegReader() :
    _ctx(NULL),
	_fRead(0)
{
    av_register_all();
    avformat_network_init();
    _packetsFile.open ("packets", ios::binary);
}

int FfmpegReader::open(const string &filename)
{
    int error = avformat_open_input(&_ctx, filename.c_str(), NULL, NULL);
    avformat_find_stream_info(_ctx, 0);
    prepareForDecode(); // Needed when trying to generate images
    return 0;
}

int FfmpegReader::close()
{
    avformat_close_input(&_ctx);
    _packetsFile.close();
    return 0;
}

void FfmpegReader::prepareForDecode()
{
    // The de decoder associated to the stream
    _codec = avcodec_find_decoder(_ctx->streams[0]->codecpar->codec_id);
    if (!_codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    _codecContext = avcodec_alloc_context3(_codec);
    _frame = av_frame_alloc();

    /* open it */
    if (avcodec_open2(_codecContext, _codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
}

int FfmpegReader::readFrame(AVPacket *packet)
{
    return av_read_frame(_ctx, packet);
}

int FfmpegReader::readAll()
{
    AVPacket packet;
    int error = readFrame(&packet);
    while (error >= 0 && error != AVERROR_EOF)
    {
        processFrame(packet);

        av_packet_unref(&packet);
        error = readFrame(&packet);
        _fRead++;
    }
    av_packet_unref(&packet);
}
