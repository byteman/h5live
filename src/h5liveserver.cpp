#include "h5liveserver.h"
#include "Poco/SingletonHolder.h"
#include "CWWebSocketServer.h"
#include "streamchannel.h"
#include <map>

static Poco::FastMutex _mutex;
static CWHttpServer  _websrv;
static std::map<std::string, Poco::SharedPtr<StreamChannel> > _channels;
H5liveServer::H5liveServer()
{

}

H5liveServer &H5liveServer::get()
{
    static Poco::SingletonHolder<H5liveServer> sh;
    return *sh.get();
}

bool H5liveServer::start(const std::string webroot, int port)
{

    _websrv.registerURI("/websocket", new Poco::Instantiator<WebSocketRequestHandler,Poco::Net::HTTPRequestHandler>());

    return _websrv.start(webroot, port);

}

bool H5liveServer::push264(const std::string &name,unsigned char *data, int size, long long pts)
{
    Poco::FastMutex::ScopedLock _lock(_mutex);
    if(_channels.find(name) == _channels.end())
        return false;
    _channels[name]->push(data, size, pts);
    return true;
}

void H5liveServer::addStream(const std::string &name)
{
    Poco::FastMutex::ScopedLock _lock(_mutex);
    if(_channels.find(name) != _channels.end())
        return ;
    _channels[name] = new StreamChannel();

    return;
}

void H5liveServer::removeStream(const std::string &name)
{
    Poco::FastMutex::ScopedLock _lock(_mutex);
    if(_channels.find(name) == _channels.end())
        return;
    _channels.erase (name);
}

