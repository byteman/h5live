#include "BaseWriter.h"


StreamWriter::StreamWriter(std::ostream& st):m_output(st)
{

}

int StreamWriter::write(int type,uint8_t *buf, size_t len)
{
    m_output.write((char*)buf, len);
    return 0;
}


