#include "streamchannel.h"
#include "Poco/Delegate.h"
#include "CWWebSocketServer.h"
#include "Poco/Timestamp.h"

StreamChannel::StreamChannel(const std::string &name):
    _name(name),
    _activity(this, &StreamChannel::run)
{
    open();
}

StreamChannel::~StreamChannel()
{
    close();
}

int StreamChannel::open()
{

    WebSocketSvrImpl::instance().onConnect += Poco::delegate(this, &StreamChannel::onWebSocketConnected);


    _activity.start();

    return true;
}

int StreamChannel::close()
{
    WebSocketSvrImpl::instance().onConnect -= Poco::delegate(this, &StreamChannel::onWebSocketConnected);

    _activity.stop();
    _activity.wait();
    return 0;
}

int StreamChannel::push(Poco::UInt8 *data, int size,Poco::Int64 pts)
{
	Poco::Timestamp ts;

	Poco::Int64 _ts = pts;
	if(_ts == 0)
	{
		_ts = 	ts.epochMicroseconds() /1000;
	}
    _muxer.push_h264(data, size, _ts);

    return 0;
}

int StreamChannel::flush()
{
    return 0;
}

void StreamChannel::run()
{

     while (!_activity.isStopped())
     {

        FlvTag tag;
        cw_info("stream run\n");
        if(_muxer.pop_tag(tag,5000))
        {
            cw_info("1.send size=%d\n",tag.data.size());

            WebSocketSvrImpl::instance().sendFrame(_name,(const char*)tag.data.data(),tag.data.size());
            cw_info("2.send size=%d\n",tag.data.size());
        }
        else
        {
            cw_warn("5s Timeout:StremChannel is empty\n");
        }

     }

}
bool StreamChannel::sendFrame(Poco::Net::WebSocket* socket, const char* buffer, int sz)
{
    try
    {

        int sendLen = 0;
        do
        {
            int len = socket->sendFrame((char*)buffer + sendLen, sz - sendLen,Poco::Net::WebSocket::FRAME_BINARY);
            if (len < 0)
            {
                return false;
            }

            sendLen += len;
            if (sendLen >= sz)
                break;
        } while (1);
    }
    catch (...)
    {
        return false;
    }
    return true;
}
void StreamChannel::onWebSocketConnected(const void* pSender,Poco::Net::WebSocket*& socket)
{
    cw_warn("websocket connected\n");
    std::vector<unsigned char> data;
    if(_muxer.get_header(data))
    {

        sendFrame(socket,(const char*)data.data(),data.size());
    }
    if(_muxer.get_ppssps(data))
    {
        sendFrame(socket,(const char*)data.data(),data.size());
    }
    std::vector<FlvTag> gops;
    if(_muxer.get_gop(gops))
    {
        for(size_t i = 0; i < gops.size(); i++)
        {
           sendFrame(socket,(const char*)gops[i].data.data(),gops[i].data.size());
        }

    }
}

