#include "pusherpair.h"
#include "Poco/Delegate.h"
#include "Poco/FunctionDelegate.h"
#include "CWLogger.h"

PusherPair::PusherPair(std::string id):
    _id(id),
    _started(false)
{
    _pusher.setId(id);
}

int PusherPair::init(std::string url,int codec, int dw, int dh, int fps,int bitrate, int gop,bool redirect)
{

    _redirect = redirect;

    _encoder.setEncoderPara(codec,dw, dh, fps,  bitrate,gop);


    //_encoder.enableDump(_id+".264");

    if(!_pusher.setURL(url))
    {
        cw_error("set url failed %s",url.c_str());
        return -1;
    }
    return 0;

}

bool PusherPair::equal(std::string url, int codec, int dw, int dh, int fps, int bitrate, int gop, bool redirect)
{
    return (  _pusher.equal(url) && _encoder.equal(codec,dw,dh,fps,bitrate,gop) && redirect==_redirect);
}

int PusherPair::init(int sw, int sh, int sformat)
{
    if(_redirect)
    {
        return 0;
    }
    return _encoder.updateEncoder(sw,sh, sformat);

}

int PusherPair::start()
{
    if(_started)
    {
        cw_warn("id=%s has started",_id.c_str());
        return 0;
    }
    if(!_redirect && _encoder.isStopped())
    {

        std::vector<unsigned char> sps;
        std::vector<unsigned char> pps;

        pps = _encoder.getPps();
        sps = _encoder.getSps();
        if(!_pusher.setPpsSps(sps,pps))
        {
            cw_error("get spspps failed");
            return -1;
        }

        _pusher.event += Poco::delegate(this, &PusherPair::notify);
        _encoder.start();
        _encoder.onEncodeComplete += Poco::delegate(this, &PusherPair::onEncodeComplete);
    }
    int err = _pusher.connect();
    if(err == 0)
    {
         _started = true;
    }
    return err;
}

int PusherPair::stop()
{
    if(!_started) return 0;
    if(!_redirect)
    {
        _pusher.event -= Poco::delegate(this, &PusherPair::notify);
        _encoder.onEncodeComplete -= Poco::delegate(this, &PusherPair::onEncodeComplete);
        _encoder.stop();
    }

    int err = _pusher.disconect();
    _started = false;
    return err;
}
void PusherPair::notify(const void* pSender,int &event)
{
    if(event == 0) //流已经连接
    {
        cw_info("stream %s has connected\n",_id.c_str());
    }
    else if(event == 1) //流断开
    {
        cw_error("stream %s has connected\n",_id.c_str());
    }
}

void PusherPair::onEncodeComplete(const void* pSender,AVPacket& pkt)
{

    //printf("pts= %lld dts=%lld\n",pkt.pts,pkt.dts);
    Poco::Timestamp ts;

    _pusher.pushVideo(pkt.data,pkt.size,ts.epochMicroseconds() /1000, pkt.flags==AV_PKT_FLAG_KEY?true:false);
}

int PusherPair::push(VideoBuf& frame)
{
    if(!_redirect)
    {
        if(frame.format == SS_PT_H264)
            return 0;
        _encoder.pushFrame(frame.buffer,frame.w,frame.h,frame.pts);
    }
    else
    {
        if(frame.format != SS_PT_H264) return 0;
        _pusher.pushVideo(( (unsigned char*)frame.buffer), frame.size, frame.pts,frame.iskey);
    }
    return 0;
}

bool PusherPair::isStarted()
{
    return _started;
}

bool PusherPair::redirect(bool enable)
{
    if(enable==_redirect) return true;

    return _redirect;
}

//bool PusherPair::addWriter(std::string key,IWriter *writer)
//{
//    return _pusher.addWriter(key,writer);
//}
bool PusherPair::addReader(std::string key)
{
    return _pusher.addReader(key);
}
bool PusherPair::waitPacket(std::string key, Packet& pkt,int timeout)
{
    return _pusher.waitPacket(key,pkt,timeout);
}
void  PusherPair::getStatisticsInfo(Poco::UInt64& encodeFrames, Poco::UInt64& encodeLostFrames)
{
    if(!_redirect)
    {
        _encoder.getStatisticsInfo(encodeFrames, encodeLostFrames);
    }
}

#include <map>
#include "Poco/RWLock.h"
typedef std::map<std::string ,PusherPair*> PusherMap;
static  PusherMap pusherList;
static  Poco::RWLock _rwlock;

PusherPair *createPusherPair(std::string id)
{
    Poco::ScopedWriteRWLock lock(_rwlock);
    PusherPair* pair = pusherList[id];

    if(pair == NULL)
    {
        pair = new PusherPair(id);
        pusherList[id] = pair;
    }
    return pair;
}


bool releasePusherPair(std::string id)
{
    Poco::ScopedWriteRWLock lock(_rwlock);
    PusherPair* pair = pusherList[id];

    if(pair == NULL)
    {
        return true;
    }
    pair->stop();
    delete pair;
    pusherList[id] = NULL;
    return true;
}


PusherPair *getPusherPair(std::string id)
{
    Poco::ScopedReadRWLock lock(_rwlock);
    PusherPair* pair = pusherList[id];
    return pair;
}


bool existPusherPair(std::string id)
{
    return getPusherPair(id)?true:false;
}
int listChannels(std::vector<std::string> &channels)
{
    Poco::ScopedReadRWLock lock(_rwlock);
    std::map<std::string ,PusherPair*>::iterator it;
   for(it=pusherList.begin();it!=pusherList.end();++it)
   {
       channels.push_back(it->first);
   }
   return channels.size();

}
