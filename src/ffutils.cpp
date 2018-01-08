#include "ffutils.h"

FFUtils::FFUtils()
{

}
AVFrame* FFUtils::alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
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
        av_frame_free(&picture);
        return NULL;
    }

    return picture;
}
std::string FFUtils::getErrorDesc(int err)
{
    char buf[2048] = {0,};
    av_strerror(err,buf,2048);
    return buf;
}
