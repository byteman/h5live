#ifndef PUSHERPAIR_H
#define PUSHERPAIR_H

#include "rtsppusher.h"
#include "ffmpegencoder.h"
#include "common.h"
class PusherPair
{
public:
    PusherPair(std::string id);

    int init(std::string url,int codec, int dw, int dh, int fps,int bitrate, int gop,bool redirect);
    bool equal(std::string url,int codec, int dw, int dh, int fps,int bitrate, int gop,bool redirect);
    int init(int sw,int sh, int sformat);
    /**
     * @brief start 启动编码和推流
     * @return 返回结果.
     */
    int start();
    /**
     * @brief stop 停止编码和推流.
     * @return
     */
    int stop();
    void notify(const void* pSender,int &event);
    void onEncodeComplete(const void* pSender,AVPacket& pkt);
    int  push(VideoBuf& frame);
    bool isStarted();
    bool redirect(bool enable);
    bool isRedirect(){return _redirect;}
    //bool addWriter(std::string key,IWriter* writer);
    void  getStatisticsInfo(Poco::UInt64& encodeFrames, Poco::UInt64& encodeLostFrames);
    bool addReader(std::string key);
    bool  waitPacket(std::string key,Packet& pkt, int timeout);

private:
    FFMpegEncoder _encoder;
    RtspPusher    _pusher;
    std::string   _id;
    std::string   _host;
    std::string   _url;
    bool    _redirect;
    bool    _started;
};

PusherPair* createPusherPair(std::string id);
bool        releasePusherPair(std::string id);
PusherPair* getPusherPair(std::string id);
bool        existPusherPair(std::string id);
int listChannels(std::vector<std::string> &channels);
#endif // PUSHERPAIR_H
