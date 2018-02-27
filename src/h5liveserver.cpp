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
int H5liveServer::getClientNum(const std::string& name)
{
	return WebSocketSvrImpl::instance().getClientNum(name);
}

int H5liveServer::start(const std::string webroot, int port)
{

    _websrv.registerURI("/websocket", new Poco::Instantiator<WebSocketRequestHandler,Poco::Net::HTTPRequestHandler>());

    bool res = _websrv.start(webroot, port);

	return res?0:-1;
}

int H5liveServer::push264(const std::string &name,unsigned char *data, int size, long long pts)
{
    Poco::FastMutex::ScopedLock _lock(_mutex);
    if(_channels.find(name) == _channels.end())
        return -1;
    _channels[name]->push(data, size, pts);
    return 0;
}

int H5liveServer::addStream(const std::string &name)
{
    Poco::FastMutex::ScopedLock _lock(_mutex);
    if(_channels.find(name) != _channels.end())
        return -1;
	try{
        _channels[name] = new StreamChannel(name);
	}
	catch(Poco::Exception& e)
	{
		cw_error("addStream err %s",e.displayText().c_str());
		return -2;
	}
  
    return 0;
}

int H5liveServer::removeStream(const std::string &name)
{
    Poco::FastMutex::ScopedLock _lock(_mutex);
    if(_channels.find(name) == _channels.end())
        return -1;
    _channels.erase (name);
	return 0;
}

