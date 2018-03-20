
#ifndef CWHTTPSERVER_H
#define CWHTTPSERVER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/String.h"

#include <iostream>
#include "Poco/DynamicFactory.h"
#include "Poco/Instantiator.h"
#include "mediatype.h"
using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::MessageHeader;
class CWRequestHandlerFactory;
#ifdef _WIN32
#ifdef H5LIVESERVER_EXPORTS
#define HTTP_SERVER_API __declspec(dllexport)
#else
#define HTTP_SERVER_API
#endif
#else
#define HTTP_SERVER_API
#endif
class HTTP_SERVER_API CWHttpServer
{
public:
	CWHttpServer(void);
	~CWHttpServer(void);

public:
    bool start(std::string path, int port);
	void stop();
	void setPort(int num);
	void setWebDir(std::string path);
	std::string getWebDir(){return m_webdir;}
    bool registerURI(std::string uri,Poco::AbstractInstantiator<HTTPRequestHandler>* pFactory);

private:
	int m_nPort;
	ServerSocket *svs;
	HTTPServer *srv;
	std::string m_webdir;
    CWRequestHandlerFactory* m_factory;
    MediaType _media;
};
#endif
