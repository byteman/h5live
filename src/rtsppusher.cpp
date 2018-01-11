#include "rtsppusher.h"
#include "Poco/URI.h"
#include <stdio.h>
#include "CWLogger.h"
#include "h5liveserver.h"

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

void RtspPusher::setId(std::string id)
{
    _id = id;
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
    H5liveServer::get().push264(_id, sps.data(), sps.size(),0);
    H5liveServer::get().push264(_id, pps.data(), pps.size(),0);
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
int RtspPusher::pushVideo(unsigned char *raw_h264, int size,long long pts,bool keyframe)
{

   H5liveServer::get().push264(_id,raw_h264, size,pts);

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


