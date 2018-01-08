#ifndef RTSPPUSHER_H
#define RTSPPUSHER_H

#include <string>
#include "Poco/SharedPtr.h"
#include "Poco/BasicEvent.h"
#include "dataqueue.h"
#include <map>
#include <queue>
#include "streamchannel.h"
class RtspPusher
{
public:
    RtspPusher();
    virtual ~RtspPusher();
    bool setURL(std::string host,int port,std::string stream,std::string user, std::string pass);
    bool setURL(std::string url);
    bool equal(std::string url);
    bool setPpsSps(std::vector<unsigned char> &sps, std::vector<unsigned char> &pps);
    int  connect();
    int  disconect();
    std::string getURL(){return _url;}
    Poco::BasicEvent<int> event;
    int pushVideo(unsigned char *raw_h264, int size,int64_t pts,bool keyframe);

    bool addReader(std::string key);
    bool waitPacket(std::string key, Packet& pkt,int timeout);
private:
    bool getStreamName(std::string &stream);
    std::string _url;
    std::string _host;
    std::string _stream;
    int _port;
    std::string _user;
    std::string _pass;
    bool _connected;
    bool _quit;
    //std::map<std::string,Poco::SharedPtr<IReaderWriter>> _writers;
    int nActivated;
    Poco::FastMutex _mutex;
    std::string _id;
    DataQueue<Packet> _queue;
    std::vector<Packet> _gop_queue;
    int timestamp;
    StreamChannel _channel;

};

#endif // RTSPPUSHER_H
