#include "CWRequestHandlerFactory.h"
#include "CWRequestHandler.h"
#include <iostream>
//#include "FileHandler.h"
#include <stdio.h>
#include "CWLogger.h"
#include "CWWebSocketServer.h"
CWRequestHandlerFactory::CWRequestHandlerFactory()
{

}


CWRequestHandlerFactory::~CWRequestHandlerFactory(void)
{

}

HTTPRequestHandler* CWRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request)
{
    std::string url = request.getURI();
    if (request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0)
    {
        url = "/websocket";
    }

    if(uriRoute.find (url) != uriRoute.end ())
    {
        //已经注册了的http路由
        std::string  &className = uriRoute[url] ;
        return _httpReqFactory.createInstance (className);

    }
    //直接处理文件.
    return new CWRequestHandler(m_webdir);
}

bool CWRequestHandlerFactory::registerURI(std::string uri,HttpReqFactory* pFactory)
{
    uriRoute[uri] = uri;
    _httpReqFactory.registerClass(uri, pFactory);
    return true;
}

void CWRequestHandlerFactory::setPath(std::string &path)
{
    m_webdir = path;
}
