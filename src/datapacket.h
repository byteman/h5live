#ifndef DATAPACKET_H
#define DATAPACKET_H
#include <vector>
struct Packet{
    Packet():
        key(false){

    }
    Packet(unsigned char* _data, int size,bool _key=false):
        key(_key)
    {
        data.assign(_data,_data+size);
    }
    bool key;
    unsigned int nTimeStamp;
    std::vector<unsigned char> data;
};
#endif // DATAPACKET_H

