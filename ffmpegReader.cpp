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

FfmpegReader::FfmpegReader() :
    _ctx(NULL)
{
    av_register_all();
    avformat_network_init();
}

int FfmpegReader::open(const string &filename)
{
    int error = avformat_open_input(&_ctx, filename.c_str(), NULL, NULL);
    return 0;
}

int FfmpegReader::close()
{
    avformat_close_input(&_ctx);
    return 0;
}

int FfmpegReader::readFrame(AVPacket *packet)
{
    return av_read_frame(_ctx, packet);
}

int FfmpegReader::readAll()
{
    AVPacket packet;
    int error = readFrame(&packet);
    long long int fReaded = 1;
    while (error >= 0 && error != AVERROR_EOF)
    {
        processFrame(packet);
        av_packet_unref(&packet);
        error = readFrame(&packet);
        fReaded++;
    }
    av_packet_unref(&packet);
}
