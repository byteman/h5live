#include "flvmuxer.h"
#include "CWLogger.h"

FlvMuxer::FlvMuxer():
    _flv_encoder(NULL)
{

    _fos = new Poco::FileOutputStream("test.flv");

    //_flv_encoder = new H264FlvEncoder(new StreamWriter(*_fos.get()));
    _flv_encoder = new H264FlvEncoder(this);
}
//�ҵ�������I֡������֡
//�ҵ�GOP
//��װnaluΪһ��flv tag
void FlvMuxer::push_h264(Poco::UInt8 *nalu, int size,Poco::Int64 pts)
{
    //printf("%d->%02x %02x %02x %02x %02x %02x \n",size,nalu[0],nalu[1],nalu[2],nalu[3],nalu[4],nalu[5] );
    _flv_encoder->write_video(nalu,size,pts,pts);
}
bool FlvMuxer::setPpsSps(std::vector<unsigned char> &sps, std::vector<unsigned char> &pps)
{
    return true;
}
//һ����������Ҫ���� flvͷ��,ppssps��tag,gop.
bool FlvMuxer::pop_tag(FlvTag &tag, int timeout)
{
    return _flv_queue.take(timeout,tag);
}
//��ȡһ����������Ҫ���͵�flvͷ����ppssps��tag,gop.
bool FlvMuxer::get_header(std::vector<unsigned char> &header)
{
    header = _header;
    return true;
}

bool FlvMuxer::get_ppssps(std::vector<unsigned char> &ppssps)
{
    ppssps = _ppssps;
    return true;
}

bool FlvMuxer::get_gop(std::vector<FlvTag> &gop)
{
    gop = _gop_queue;
    return true;
}
//����д�����ݶ���flv tag.
int FlvMuxer::write(int type,uint8_t *buf, size_t len)
{
    FlvTag tag;
    if(type == TAG_HEADER)
    {
        _header.clear();
        _header.assign(buf, buf+len);
    }
    else if(type == TAG_SPSPPS)
    {
        _ppssps.clear();
        _ppssps.assign(buf, buf+len);
    }
    else if(type == TAG_NALU_I)
    {
        _gop_queue.clear();
        tag.push(buf,len);
        _gop_queue.push_back(tag);
    }
    else
    {
        tag.push(buf,len);
        _gop_queue.push_back(tag);
    }
    if(tag.data.size() > 0)
    {
        if(_flv_queue.size() > 5)
        {
            //����̫������
            _flv_queue.clear();
            printf("reset queue\n");
        }
        else
        {
            _flv_queue.put(tag);
        }
        //printf("queue size=%d\n",_flv_queue.size());

    }

    return 0;
}

