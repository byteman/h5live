#ifndef FFMPEGENCODER_H
#define FFMPEGENCODER_H

#include "ffconfig.h"
#include "ffavframe.h"
#include "videoqueue.h"
#include "Poco/Activity.h"
#include "Poco/BasicEvent.h"
#include "Poco/EventArgs.h"
#include "videopp.h"
#include "Poco/FileStream.h"

class EncoderPacket:public Poco::EventArgs
{
public:
   AVPacket* pkt;

};
class FFMpegEncoder
{
public:
    FFMpegEncoder();
    ~FFMpegEncoder();

    int     setEncoderPara(int codec_id,int dw, int dh, int fps, int gop,int bitrate);
    int     updateEncoder(int sw,int sh, int sformat);
    bool    equal(int codec_id, int dw, int dh, int fps, int gop,int bitrate);
    void    start();
    void    stop();

    int     pushFrame(void* data,  int width, int height,int64_t pts);

    std::vector<unsigned char>&   getPps();
    std::vector<unsigned char>&   getSps();
    bool isStopped();
    void    enableDump(std::string file);
    void runActivity();
    Poco::BasicEvent<AVPacket> onEncodeComplete;

    void  getStatisticsInfo(Poco::UInt64& encodeFrames, Poco::UInt64& encodeLostFrames);

private:
    int     uninit();
    void    flush();
    int     encode(AVFrame* pFrame);
    AVFrame* getTempFrame();
    bool    updateSpsPps(AVCodecContext* ctx);
    int     closeEncoder();
    int     openEncoder(int codec_id, int sw,int sh, int sformat,
                    int dw, int dh, int fps, int gop,int bitrate);
    Poco::Event _ready;
    Poco::Event _exit;
    //AVFrame *frame;
    FrameQueue _queue;
    //QtAV::BlockingQueue<AVFrame*> _queue;
    AVCodec *codec;
    AVCodecContext *c;
    int _pts;

    bool _opened;
    VideoPP _vpp;
    AVFrame* _frame;
    Poco::Activity<FFMpegEncoder> _activity;
    Poco::FileOutputStream _fos;
    bool _enable_dump;
    AVPixelFormat _sfmt;
    int parse(AVPacket *pkt);
    int _codec_id;
    int _dst_w;
    int _dst_h;
    int _dst_fps;
    int _dst_bitrate;
    int _gop;
    std::vector<unsigned char> sps;
    std::vector<unsigned char> pps;
    Poco::Mutex _mutex;


    Poco::UInt64		m_totalEncodeFrames;
    Poco::UInt64		m_totalEncodeLostFrames;
};

#endif // FFMPEGENCODER_H
