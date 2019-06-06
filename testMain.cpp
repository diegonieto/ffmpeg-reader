#include <iostream>
#include <vector>

#include <assert.h>

#include "ffmpegReader.h"

class FfmpegReaderShowPacket : public FfmpegReader {
    public:
        void processFrame(const AVPacket &packet);
};

class PesHeader {
public:
    PesHeader(const bool ptsEn, const bool dtsEn);

    void init(int64_t pts, int64_t dts, int dataLength, uint8_t id);
    std::vector<unsigned char> data() { return _data; };
    int size() { return _data.size(); };

private:
    /** Number of bytes up to pes packet length. */
    const uint8_t NROFBYTES_UPTO_PES_PACKET_LENGTH;
    /** Number of bytes, after pes packet length, up to pes_hader_data_length. */
    const uint8_t NROFBYTES_UPTO_PES_HEADER_DATA_LENGTH;
    /** True if PTS optional field is present. */
    const bool PTS_EN;
    /** True if DTS optional field is present. */
    const bool DTS_EN;
    /** PES Marker bit (1), as defined in ISO/IEC 1318-1. */
    const uint8_t PES_MARKER_BIT;

    std::vector<unsigned char> _data;
};

PesHeader::PesHeader(const bool ptsEn, const bool dtsEn) :
    NROFBYTES_UPTO_PES_PACKET_LENGTH(6),
    NROFBYTES_UPTO_PES_HEADER_DATA_LENGTH(3),
    PTS_EN(ptsEn),
    DTS_EN(dtsEn),
    PES_MARKER_BIT(1)
{
    _data.resize(NROFBYTES_UPTO_PES_PACKET_LENGTH + NROFBYTES_UPTO_PES_HEADER_DATA_LENGTH);

    // packet_start_code_prefix
    _data[0] = 0;
    _data[1] = 0;
    _data[2] = 1;

    // header_flag_A
    _data[6] = 0x81;

    // (_data[7]) header_flag_B
    // (_data[8]) PES_header_data_length
    if (PTS_EN && DTS_EN) {
        _data[7] = 0xc0;
        _data[8] = 10;
    } else if (PTS_EN) {
        _data[7] = 0x80;
        _data[8] = 5;
    } else {
        _data[7] = 0x00;
        _data[8] = 0;
    }

    // Resize to total PES packet size without stream data.
    _data.resize(_data[8] + NROFBYTES_UPTO_PES_PACKET_LENGTH +
            NROFBYTES_UPTO_PES_HEADER_DATA_LENGTH);
}

void PesHeader::init(int64_t pts, int64_t dts, int dataLength, uint8_t id)
{
    _data[3] = id;

    // If the pesPacketLength turns out to be < 0 or > 65536, then we init to 0.
    uint16_t pesPacketLength;
    if (dataLength < 0 || dataLength > 65536) {
        //assert(false);
        pesPacketLength = 0;
    } else {
        int size = _data.size() - NROFBYTES_UPTO_PES_PACKET_LENGTH + dataLength;
        pesPacketLength = (size > 65536) ? 0 : (uint16_t)size;
    }
    _data[4] = pesPacketLength >> 8;
    _data[5] = pesPacketLength;

    int i = 9;
    if (PTS_EN) {
        // 4b '0010b', 3b PTS[32..30], 1b marker_bit
        _data[i++] = 0x20 | (((pts >> 30) & 0x7) << 1) | PES_MARKER_BIT;
        // 15b PTS[29..15], 1b marker_bit
        _data[i++] = pts >> 22;
        _data[i++] = (((pts >> 15) & 0x7f) << 1) | PES_MARKER_BIT;
        // 15b PTS[14..0], 1b marker_bit
        _data[i++] = pts >> 7;
        _data[i++] = ((pts & 0x7f) << 1) | PES_MARKER_BIT;
    }
    if (DTS_EN) {
        // 4b '0001b', 3b DTS[32..30], 1b marker_bit
        _data[i++] = 0x10 | (((dts >> 30) & 0x7) << 1) | PES_MARKER_BIT;
        // 15b DTS[29..15], 1b marker_bit
        _data[i++] = dts >> 22;
        _data[i++] = (((dts >> 15) & 0x7f) << 1) | PES_MARKER_BIT;
        // 15b DTS[14..0], 1b marker_bit
        _data[i++] = dts >> 7;
        _data[i++] = ((dts & 0x7f) << 1) | PES_MARKER_BIT;
    }
}

void FfmpegReaderShowPacket::processFrame(const AVPacket &packet)
{
    const AVStream *stream = _ctx->streams[packet.stream_index];
    //std::cout << "Stream type: " << stream->codec->codec_type
    //        << " is key frame: " << (packet.flags & AV_PKT_FLAG_KEY)
    //        << " duration " << packet.duration
    //        << " pts: " << packet.pts
    //        << " dts: " << packet.dts << std::endl;
//	av_pkt_dump2(stdout, &packet, 0, _ctx->streams[0]);

    //decode_and_save_frame(_codecContext, _frame, (AVPacket*)&packet, "/home/dnieto/picture");

    PesHeader pesHeader(true, true);
    pesHeader.init(av_q2d(_ctx->streams[packet.stream_index]->time_base) * packet.pts * 90000.0f,
                   av_q2d(_ctx->streams[packet.stream_index]->time_base) * packet.dts * 90000.0f,
                   packet.size, (uint8_t) 0xe1);

    //_packetsFile.write((const char*)pesHeader.data().data(), pesHeader.size());
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
