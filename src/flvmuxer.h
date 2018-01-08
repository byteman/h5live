#ifndef FLVMUXER_H
#define FLVMUXER_H

#include "Poco/Foundation.h"
#include "dataqueue.h"
#include "Poco/FIFOBuffer.h"
#include "FlvEncoder.h"
#include "Poco/FileStream.h"
class FlvTag{
public:
    FlvTag(){}
    FlvTag(unsigned char* buf, int size)

    {
        data.clear();
        data.assign(buf, buf+size);
    }
    void push(unsigned char* buf, int size)
    {
        data.clear();
        data.assign(buf, buf+size);
    }
    int type;
    std::vector<unsigned char> data;
};


class FlvMuxer:public BaseWriter
{
public:
    FlvMuxer();
    void push_h264(Poco::UInt8* nalu,int size,Poco::Int64 pts);
    bool setPpsSps(std::vector<unsigned char> &sps, std::vector<unsigned char> &pps);
    bool pop_tag(FlvTag& tag,int timeout);
    bool get_header(std::vector<unsigned char> &header);
    bool get_ppssps(std::vector<unsigned char> &ppssps);
    bool get_gop(std::vector<FlvTag> &gop);

    virtual int write(int type,uint8_t *buf, size_t len);
private:
    DataQueue<FlvTag> _flv_queue;
    std::vector<FlvTag> _gop_queue;
    H264FlvEncoder *_flv_encoder;
    std::vector<unsigned char> _header;
    std::vector<unsigned char> _ppssps;
    Poco::SharedPtr<Poco::FileOutputStream> _fos;

};

#endif // FLVMUXER_H
