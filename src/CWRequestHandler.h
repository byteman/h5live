
#ifndef CWREQ_HANDLER_H
#define CWREQ_HANDLER_H

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"



using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPRequestHandler;


class CWRequestHandler : public HTTPRequestHandler
{
public:
    CWRequestHandler(std::string& path);
	~CWRequestHandler(void);

	int responseFile(HTTPServerResponse& response, std::string sendfilepath, std::string contentType);
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
	bool handleWithParaRequest(HTTPServerRequest& request, HTTPServerResponse& response, std::string &url);

private:

	std::string m_website_path;
	std::string m_index_path;
	std::string m_error_path;

    int sendError(HTTPServerResponse &response);
};

#endif
