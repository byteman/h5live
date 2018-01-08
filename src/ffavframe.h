#ifndef FFAVFRAME_H
#define FFAVFRAME_H

#include "ffconfig.h"

class FFAvFrame
{
public:
    FFAvFrame();
    ~FFAvFrame();
    int alloc(int width, int height,AVPixelFormat format,int algin=16);
    int setBuffer(void* data,int width, int height,AVPixelFormat format);
    int getBuffer(void* data, int size);
    int free();

    AVFrame* get();
    FFAvFrame(FFAvFrame& frame);
    FFAvFrame& operator=(FFAvFrame& frame);
private:

    AVFrame *_frame;
    bool _bAlloc;

};

#endif // FFAVFRAME_H
