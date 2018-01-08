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
        int _gop; //�����gop
        int _bitrate; //Ŀ������
        int _fps; //Ŀ��֡��
        int _hwAccelType; //Ӳ���������� 0->x264 1->quicksync 2->auto
        int _sfmt; //Դ��Ƶ��ʽ.
        int _sw; //ԭ��Ƶ���.
        int _sh; //ԭ��Ƶ�߶�
        int _dw; //Ŀ����Ƶ�Ŀ��
        int _dh;//Ŀ����Ƶ�߶�
        bool _redirect;//ת����Ƶ.
    };
    struct PusherEvent
    {
        PusherEvent(std::string _channel,int _type):
            channel(_channel),
            type(_type)
        {

        }
        std::string channel; //��ƵID.
        int type; //�¼�����. //�ص�֪ͨ�¼�, 1.���ӳ�ʱ 2.�������� 3.�豸�Ͽ�
    };
    enum PusherErrCode
    {
        PUSHER_ERROR_OK=0, //�ɹ�
        PUSHER_EXIST_STREAM = 3000,//pusher  �Ѿ�������.
        PUSHER_NOT_EXIST_STREAM,//pusher  ������.
        PUSHER_ADD_STREAM_FAILED, //������ʧ��
        PUSHER_REMOVE_STREAM_FAILED, //ɾ����ʧ��
        PUSHER_START_STREAM_FAILED,
        PUSHER_INIT_STREAM_FAILED,
        PUHSER_ERR_MAX,
    };
    PusherManager();
    ~PusherManager();
    //����ģ���ǵ�����
    static PusherManager &get();
    //����ģ��
    PusherErrCode   start();
    //ֹͣģ��
    PusherErrCode   stop();

    /**
     * @brief startPush ��������
     * @param id ��Ƶ���
     * @param url ��ý��������ϵ�
     * @return
     */
    PusherErrCode   addStream(std::string id, std::string url,PusherPara para);
    //PusherErrCode   addStreamWriter(std::string id, IWriter* writer);

    bool        waitPacket(std::string id, std::string key,Packet& pkt, int timeout);

    /**
     * @brief stopPush ֹͣ����
     * @param id ��Ƶ���
     * @return
     */
    PusherErrCode   removeStream(std::string id);




    /**
     * @brief �������� yuv420p->����->����h264->push
     * @param id ��Ƶ���
     * @param yuv420p_frame raw��Ƶ֡yuv420p��ʽ
     * @param src_w ��Ƶ֡Դ���
     * @param src_h ��Ƶ֡Դ�߶�
     * @param format ���ظ�ʽ 0->yuv420p 1->RGB888 2->NV21
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
     * @return ֧�ֵı�����
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
