#ifndef PUSHERMANAGER_H
#define PUSHERMANAGER_H

#include <string>

#include "Poco/BasicEvent.h"
#include "common.h"
#include "dataqueue.h"
#ifdef PUSHER_EXPORTS
#define PUSHERMODULE_API __declspec(dllexport)
#else
#define PUSHERMODULE_API
#endif


#define ENCODER_LIBX264 0
#define ENCODER_QSV     1


#define FORMAT_YUV420P  0
#define FORMAT_BGR24    1

class PUSHERMODULE_API PusherManager
{
public:
    struct PusherPara
    {
        PusherPara():
        _dw(640),
        _dh(480),
        _sw(640),
        _sh(480),
        _sfmt(FORMAT_BGR24),
        _fps(25),
        _bitrate(200000),
        _gop(10),
        _hwAccelType(ENCODER_LIBX264),
        _redirect(true)

        {

        }
        PusherPara(int sw,int sh, int sf, int dw, int dh, int fps, int gop,int bitrate,int codec,bool redirect):
            _dw(dw),
            _dh(dh),
            _sw(sw),
            _sh(sh),
            _sfmt(sf),
            _fps(fps),
            _bitrate(bitrate),
            _gop(gop),
            _hwAccelType(codec),
            _redirect(redirect)
        {

        }
        PusherPara(int sw,int sh, int sf, int fps, int gop,int bitrate,int codec,bool redirect):
            _dw(sw),
            _dh(sh),
            _sw(sw),
            _sh(sh),
            _sfmt(sf),
            _fps(fps),
            _bitrate(bitrate),
            _gop(gop),
            _hwAccelType(codec),
            _redirect(redirect)
        {

        }
        int _gop; //编码后gop
        int _bitrate; //目标码率
        int _fps; //目标帧率
        int _hwAccelType; //硬件加速类型 0->x264 1->quicksync 2->auto
        int _sfmt; //源视频格式.
        int _sw; //原视频宽度.
        int _sh; //原视频高度
        int _dw; //目标视频的宽度
        int _dh;//目标视频高度
        bool _redirect;//转发视频.
    };
    struct PusherEvent
    {
        PusherEvent(std::string _channel,int _type):
            channel(_channel),
            type(_type)
        {

        }
        std::string channel; //视频ID.
        int type; //事件类型. //回调通知事件, 1.连接超时 2.参数错误 3.设备断开
    };
    enum PusherErrCode
    {
        PUSHER_ERROR_OK=0, //成功
        PUSHER_EXIST_STREAM = 3000,//pusher  已经存在了.
        PUSHER_NOT_EXIST_STREAM,//pusher  不存在.
        PUSHER_ADD_STREAM_FAILED, //创建流失败
        PUSHER_REMOVE_STREAM_FAILED, //删除流失败
        PUSHER_START_STREAM_FAILED,
        PUSHER_INIT_STREAM_FAILED,
        PUHSER_ERR_MAX,
    };
    PusherManager();
    ~PusherManager();
    //管理模块是单件类
    static PusherManager &get();
    //启动模块
    PusherErrCode   start();
    //停止模块
    PusherErrCode   stop();

    /**
     * @brief startPush 启动推流
     * @param id 视频编号
     * @param url 流媒体服务器上的
     * @return
     */
    PusherErrCode   addStream(std::string id, std::string url,PusherPara para);
    //PusherErrCode   addStreamWriter(std::string id, IWriter* writer);

    bool        waitPacket(std::string id, std::string key,Packet& pkt, int timeout);

    /**
     * @brief stopPush 停止推流
     * @param id 视频编号
     * @return
     */
    PusherErrCode   removeStream(std::string id);




    /**
     * @brief 编码推流 yuv420p->缩放->编码h264->push
     * @param id 视频编号
     * @param yuv420p_frame raw视频帧yuv420p格式
     * @param src_w 视频帧源宽度
     * @param src_h 视频帧源高度
     * @param format 像素格式 0->yuv420p 1->RGB888 2->NV21
     * @return
     */
    PusherErrCode   pushFrame(VideoBuf* video);

    /**
     * @brief getErrorDesc
     * @param code
     * @return
     */
    std::string     getErrorDesc(PusherErrCode &code);

    /**
     * @brief enumHwAccelTypes
     * @param types
     * @return 支持的编码器
     */
    int             enumEncoderTypes(std::vector<int> &types);

    void            getStatisticsInfo(std::string strId, Poco::UInt64& encodeFrames, Poco::UInt64& encodeLostFrames);


    bool    existStream(std::string id);
    void    listStreams(std::vector<std::string> streams);
    Poco::BasicEvent<PusherEvent> event;
    void listStreams(std::vector<std::string> &streams);
    PusherErrCode addStreamReader(std::string id, std::string key);
private:
    PusherErrCode   startPush(std::string id,bool start=true);
    bool    streamStarted(std::string id);
    PusherManager(const PusherManager&);
    PusherManager& operator =(const PusherManager&);
    PusherPara _pushPara;
};


#endif // PUSHERMANAGER_H
