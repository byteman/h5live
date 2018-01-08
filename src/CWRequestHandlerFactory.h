#pragma once

#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "CWRequestHandler.h"
#include "Poco/DynamicFactory.h"


using Poco::Net::HTTPRequestHandlerFactory;

class CWRequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
    typedef Poco::AbstractInstantiator<HTTPRequestHandler> HttpReqFactory;
    CWRequestHandlerFactory();
	~CWRequestHandlerFactory(void);

	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request);
    bool registerURI(std::string uri,HttpReqFactory* pFactory);
    HTTPRequestHandler* createRequestHandler (const std::string& className) const
    {
        return _httpReqFactory.createInstance (className);
    }
    void setPath(std::string& path);


public:
	std::string m_webdir;

    std::map<std::string, std::string> uriRoute;
    Poco::DynamicFactory<HTTPRequestHandler> _httpReqFactory;
};

