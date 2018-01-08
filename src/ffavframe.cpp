#include "ffavframe.h"

#include "assert.h"




FFAvFrame::FFAvFrame():
    _bAlloc(false)
{
   _frame = av_frame_alloc();
}

int FFAvFrame::alloc(int width, int height, AVPixelFormat format,int algin)
{
    int ret = av_frame_get_buffer(_frame, 32);
    //int ret = av_image_alloc(_frame->data,_frame->linesize,width,height,format, algin);
    if(ret < 0)
    {
        return -2;
    }

    return 0;
}


FFAvFrame::~FFAvFrame()
{
    free();
}

AVFrame *FFAvFrame::get()
{
    return _frame;
}

FFAvFrame::FFAvFrame(FFAvFrame &f)
{
    this->_frame  = f._frame;
    this->_bAlloc = f._bAlloc;

    f._frame = NULL;
}
FFAvFrame& FFAvFrame::operator=(FFAvFrame& f)
{
    this->_frame  = f._frame;
    this->_bAlloc = f._bAlloc;

    f._frame = NULL;
    return *this;
}

int FFAvFrame::setBuffer(void* data,int width, int height,AVPixelFormat format)
{

    AVFrame tmpframe;
    if(!_bAlloc)
    {
        if(0!=alloc(width,height,format))
        {
            return -1;
        }
        _bAlloc = true;
    }
    if( (_frame->width  != width) ||
        (_frame->height != height) ||
        (_frame->format != format))
    {
        return -2;
    }
    av_frame_copy_props(&tmpframe,_frame);

    av_image_fill_arrays(tmpframe.data,tmpframe.linesize,(const uint8_t*)data,format,width,height,32);

    av_frame_copy(_frame,&tmpframe);

    return 0;
}

int FFAvFrame::getBuffer(void *data, int size)
{
    return 0;
}

int FFAvFrame::free()
{
    if(_frame != NULL)
        av_frame_free(&_frame);
    if(_bAlloc)
    {

    }
    return 0;
}

