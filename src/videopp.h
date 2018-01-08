#ifndef VIDEOPP_H
#define VIDEOPP_H

#include "ffconfig.h"
class VideoPP
{
public:
    VideoPP();
    ~VideoPP();
    bool uninit();
    bool init(AVPixelFormat sFmt, int sw, int sh,AVPixelFormat dFmt, int dw, int dh);
    AVFrame* filter(AVFrame* frame);
private:
    AVFrame* dst_frame;
    struct SwsContext *sws_ctx;
    bool _convert;
    int _sh;
};

#endif // VIDEOPP_H
