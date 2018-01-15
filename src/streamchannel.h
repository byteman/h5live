#ifndef STREAMCHANNEL_H
#define STREAMCHANNEL_H
#include "flvmuxer.h"
#include "CWHttpServer.h"
#include "Poco/Activity.h"
#include "Poco/Net/WebSocket.h"
class StreamChannel
{
public:
    StreamChannel(const std::string &name);
    ~StreamChannel();

    int push(Poco::UInt8* data, int size,Poco::Int64 pts=0);
    int flush();
    void run();


    void onWebSocketConnected(const void *pSender, Poco::Net::WebSocket*& socket);
private:
    int open();
    int close();
    FlvMuxer _muxer;
    std::string _name;
    Poco::Activity<StreamChannel> _activity;

    bool sendFrame(Poco::Net::WebSocket *socket, const char *buffer, int sz);
};

#endif // STREAMCHANNEL_H
