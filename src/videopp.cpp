#include "videopp.h"

VideoPP::VideoPP():
    _convert(false),
    sws_ctx(NULL),
    dst_frame(NULL)
{

}

VideoPP::~VideoPP()
{
    uninit();
}

bool VideoPP::uninit()
{
    if(!_convert) return false;
    if(dst_frame!=NULL)
    {
        av_frame_free(&dst_frame);
        dst_frame = NULL;
    }
    if(sws_ctx!=NULL)
    sws_freeContext(sws_ctx);
    sws_ctx = NULL;
    _convert = false;
    return true;

}
static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");

        return NULL;
    }

    return picture;
}

bool VideoPP::init(AVPixelFormat sFmt, int sw, int sh, AVPixelFormat dFmt, int dw, int dh)
{
    uninit();
    if(sFmt != dFmt || sw!=dw || sh!= dh)
    {
        _convert = true;
        sws_ctx = sws_getContext(sw, sh,sFmt,
                                               dw,dh,
                                               dFmt,
                                               SWS_POINT, NULL, NULL, NULL);
         if (!sws_ctx) {
            return false;
         }
         dst_frame = alloc_picture(dFmt, dw, dh);
         if (dst_frame == NULL) {
             fprintf(stderr, "Could not allocate video frame\n");
             return false;
         }
         _sh = sh;
    }

     return true;
}

AVFrame *VideoPP::filter(AVFrame *frame)
{
    if(frame ==NULL) return NULL;
    if(!_convert) return frame;
    if(!sws_ctx)  return NULL;
    sws_scale(sws_ctx,
                   (const uint8_t * const *)frame->data, frame->linesize,
                   0, _sh, dst_frame->data, dst_frame->linesize);
    return dst_frame;

}

