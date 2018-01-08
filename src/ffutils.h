#ifndef FFUTILS_H
#define FFUTILS_H

#include "ffconfig.h"
#include <string>
class FFUtils
{
public:
    FFUtils();
    static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);
    static std::string getErrorDesc(int err);
};



#endif // FFUTILS_H
