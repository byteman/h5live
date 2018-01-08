#include "rtsppusher.h"
#include "Poco/URI.h"
#include <stdio.h>
#include "CWLogger.h"


RtspPusher::RtspPusher():
    _quit(true),
    _connected(false),
    nActivated(0),
    timestamp(0)
{

}

RtspPusher::~RtspPusher()
{
    disconect();
}

bool RtspPusher::setURL(std::string host, int port, std::string stream, std::string user, std::string pass)
{

    _host = host;
    _user = user;
    _pass = pass;
    _stream = stream;
    _port = port;
    return true;
}

bool RtspPusher::setURL(std::string url)
{
    try
    {
        _channel.open(8000);
        Poco::URI uri(url);
        std::string scheme = uri.getScheme();
        std::string host = uri.getHost();
        int port  = uri.getPort();
        if(scheme.empty()) return false;
        std::string stream = uri.getPath();
        if(!getStreamName(stream)) return false;

        if(stream.empty()) return false;
        if(host.empty()) return false;
        _url = url;
        return setURL(host,port,stream,"admin","admin");
    }
    catch(Poco::Exception& e)
    {

    }


    return false;
}

bool RtspPusher::equal(std::string url)
{
    return url==_url;
}

bool RtspPusher::setPpsSps(std::vector<unsigned char> &sps, std::vector<unsigned char> &pps)
{
    _channel.push(sps.data(), sps.size());
    _channel.push(pps.data(), pps.size());
    return true;
}



int RtspPusher::connect()
{


    timestamp = 0;
    return 0;
}

int RtspPusher::disconect()
{

    return 0;
}
//mux flv and cache gop
int RtspPusher::pushVideo(unsigned char *raw_h264, int size,int64_t pts,bool keyframe)
{

   _channel.push(raw_h264, size,pts);

   return 0;
}
bool RtspPusher::waitPacket(std::string key, Packet& pkt, int timeout)
{
    return _queue.take(timeout,pkt);

}
bool RtspPusher::addReader(std::string key)
{
    return true;
}
bool RtspPusher::getStreamName(std::string &stream)
{

    return true;
}


