#include "CWWebSocketServer.h"
#include "CWLogger.h"

WebSocketSvrImpl::WebSocketSvrImpl()
    : m_isRunning(false)
{

}
WebSocketSvrImpl::~WebSocketSvrImpl()
{
    stop();
}
void WebSocketSvrImpl::stop()
{
    //m_isRunning = false;
    Poco::FastMutex::ScopedLock lock(m_mutex);

    std::map< std::string, std::vector<WebSocket*> >::iterator itm = m_wsMaps.begin();
    for(; itm != m_wsMaps.end(); itm++)
    {
        std::vector<WebSocket*> wsVecs = itm->second;
        std::vector<WebSocket*>::iterator itv = wsVecs.begin();
        for (; itv != wsVecs.end(); itv++)
        {
            if (*itv != NULL)
                delete *itv;
        }
    }

    m_wsMaps.clear();
}


int WebSocketSvrImpl::addWebSocket(HTTPServerRequest& request, HTTPServerResponse& response)
{

    std::string uri = request.getURI();
    if(uri.length() <= 1)
        return -1;

    std::string cameraid(uri.c_str()+1);

    try
    {
        WebSocket* ws = new WebSocket(request, response);
        Poco::Timespan sendTimeOut(300);
        ws->setSendTimeout(sendTimeOut);
        Poco::FastMutex::ScopedLock lock(m_mutex);
        std::map< std::string, std::vector<WebSocket*> >::iterator it = m_wsMaps.find(cameraid);
        if(it != m_wsMaps.end())
        {
            it->second.push_back(ws);
        }
        else
        {
            std::vector<WebSocket*> wsVec;
            wsVec.push_back(ws);
            m_wsMaps.insert(std::make_pair(cameraid, wsVec));

        }
        onConnect.notify(this, ws);
    }
    catch (WebSocketException& exc)
    {
        switch (exc.code())
        {
        case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
            response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
            // fallthrough
        case WebSocket::WS_ERR_NO_HANDSHAKE:
        case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
        case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
            response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentLength(0);
            response.send();
            break;
        }
    }
    catch (...)
    {

    }
    return 0;
}
int WebSocketSvrImpl::sendFrame(const std::string& key, const char* buffer, int size)
{
	Poco::FastMutex::ScopedLock lock(_WebSendMutex, 500);

    std::map< std::string, std::vector<WebSocket*> >::iterator it = m_wsMaps.find(key);
	if (it != m_wsMaps.end())
	{
		std::vector<WebSocket*>* wsVecs = &(it->second);
        cw_warn("want send = %d",size);
		for (int i = 0; i < wsVecs->size(); i++)
		{
			WebSocket* ws = (*wsVecs)[i];
			try
			{

				int sendLen = 0;
				do
				{
                    int len = ws->sendFrame((char*)buffer + sendLen, size - sendLen,Poco::Net::WebSocket::FRAME_BINARY);
                    if (len < 0)
                    {
                        cw_error("send error=%d",len);
						goto __CLOSEWS;
                    }
					sendLen += len;
                    cw_warn("send = %d\n",sendLen);
					if (sendLen >= size)
						break;

				} while (1);
			}
            catch(Poco::Exception& e)
            {
               cw_error("websocket error = %s",e.displayText().c_str());
               goto __CLOSEWS;
            }
			catch (...)
			{
                cw_error("websocket unkown error");
				goto __CLOSEWS;
			}

			continue;

		__CLOSEWS:
			(*wsVecs)[i] = NULL;
			delete ws, ws = NULL;
		}

		Poco::FastMutex::ScopedLock lock(m_mutex);
		std::vector<WebSocket*>::iterator it = wsVecs->begin();
		for (; it != wsVecs->end();)
		{
			if (*it == NULL)
            {
                cw_warn("delete websocket");
				it = wsVecs->erase(it);
            }
			else
				it++;
		}
	}

	return 0;
}
WebSocketSvrImpl& WebSocketSvrImpl::instance()
{
    static  Poco::SingletonHolder<WebSocketSvrImpl> sh;
    return *sh.get();
}




