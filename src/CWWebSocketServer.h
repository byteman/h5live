
#ifndef CWWebSocketServer_H
#define CWWebSocketServer_H


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/DynamicFactory.h"
#include "Poco/Format.h"
#include "CWLogger.h"
#include <iostream>


using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::ThreadPool;

class WebSocketSvrImpl/* : public Poco::Runnable*/
{
public:
    WebSocketSvrImpl();
    ~WebSocketSvrImpl();


    void stop();

    int addWebSocket(HTTPServerRequest& request, HTTPServerResponse& response);

    int sendFrame(const std::string& key, const char* buffer, int size);
	
	int getClientNum(const std::string& key);
    static WebSocketSvrImpl& instance();
    Poco::BasicEvent<WebSocket*> onConnect;
private:
	bool						m_isRunning;
	Poco::FastMutex				m_mutex;
	//Poco::FastMutex				_WebSendMutex;
    std::map< std::string, std::vector<WebSocket*> >		m_wsMaps;
};

class WebSocketRequestHandler : public HTTPRequestHandler
	/// Handle a WebSocket connection.
{
public:
    WebSocketRequestHandler(){}
    ~WebSocketRequestHandler(){}
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        WebSocketSvrImpl::instance().addWebSocket(request,response);
	}
private:
    WebSocketSvrImpl* _socket;
};



#endif

