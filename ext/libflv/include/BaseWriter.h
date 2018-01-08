#ifndef _BASE_WRITER_H
#define _BASE_WRITER_H
#include <ostream>
#include "FlvCommon.h"


class  BaseWriter
{
public:
        virtual int write(int type,uint8_t *buf, size_t len)=0;
};
class FileWriter
{
};
class StreamWriter : public BaseWriter
{
private:
        std::ostream& m_output;
public:
        StreamWriter(std::ostream& st);
        virtual int write(int type,uint8_t *buf, size_t len);
};


#endif //_BASE_WRITER_H
