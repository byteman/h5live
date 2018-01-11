
#include "CWHttpServer.h"
#include <string>
#include "CWLogger.h"
#include "CWRequestHandlerFactory.h"
#include "Poco/FileStream.h"
using namespace Poco::Dynamic;
using Poco::Util::AbstractConfiguration;




CWHttpServer::CWHttpServer(void):
    m_factory(NULL)
{
    m_factory = new CWRequestHandlerFactory();
    m_webdir = Poco::Path::current() + "/web";
	m_nPort = 7000;

}


CWHttpServer::~CWHttpServer(void)
{
	stop();
}


void CWHttpServer::setPort(int num)
{
	m_nPort = num;
}

void CWHttpServer::setWebDir(std::string path)
{
	m_webdir = path;
}
bool CWHttpServer::registerURI(std::string uri,Poco::AbstractInstantiator<HTTPRequestHandler>* pFactory)
{
    if(m_factory)
    {
       return m_factory->registerURI (uri,pFactory);
    }
    return false;
}

bool CWHttpServer::start(std::string path, int port)
{
    try
    {
        setPort(port);
        setWebDir(path);

        cw_info("HttpServerModuleImpl start \r\n");
        m_factory->setPath(path);
        std::string mime_path = Poco::Path::current ()+"mime.types";
        _media.load(mime_path);

        svs=new ServerSocket(m_nPort);

        Poco::Net::HTTPServerParams *pParams = new Poco::Net::HTTPServerParams();

        pParams->setMaxQueued(100);
        pParams->setMaxThreads(16);
        pParams->setKeepAlive(true);
        pParams->setKeepAliveTimeout(Poco::Timespan(10,0));
        pParams->setMaxKeepAliveRequests(100);

        srv = new HTTPServer(m_factory, *svs, pParams);

        srv->start();
    }
    catch(Poco::Exception& e)
    {
        cw_error("Start HttpServer %s ",e.displayText().c_str());
        return false;
    }
    return true;
	cw_info("CWHttpServer start ok\r\n");
}

void CWHttpServer::stop()
{
	if(srv != NULL)
	{
		srv->stop();

		delete srv;
		srv = NULL;
	}

	if(svs != NULL)
	{
		delete svs;
		svs = NULL;
	}
}

