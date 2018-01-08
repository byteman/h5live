#include "pushermanager.h"
#include "Poco/SingletonHolder.h"
#include "pusherpair.h"
#include "Poco/Mutex.h"
#include "CWLogger.h"
static Poco::Mutex _mutex;
PusherManager::PusherManager()
{

}

PusherManager::~PusherManager()
{

}

PusherManager &PusherManager::get()
{
    static Poco::SingletonHolder<PusherManager> sh;
    return *sh.get();
}

PusherManager::PusherErrCode PusherManager::start()
{
    Poco::Mutex::ScopedLock _lock(_mutex);

    return PUSHER_ERROR_OK;
}

PusherManager::PusherErrCode PusherManager::stop()
{
    Poco::Mutex::ScopedLock _lock(_mutex);

    return PUSHER_ERROR_OK;
}

PusherManager::PusherErrCode PusherManager::addStream(std::string id, std::string url,PusherPara para)
{
    Poco::Mutex::ScopedLock _lock(_mutex);

    PusherPair* ppair = getPusherPair(id);
    if(ppair != NULL)
    {
        if(!ppair->equal(url,para._hwAccelType,para._dw,para._dh,para._fps,
                        para._gop,para._bitrate,para._redirect))
        {
           cw_warn("add stream[%s] param not equal ,restart it",id.c_str());
            if( removeStream(id) != PUSHER_ERROR_OK )
            {
                cw_warn("remove stream[%s] failed",id.c_str());
                return PUSHER_REMOVE_STREAM_FAILED;
            }
        }
        else
        {
            cw_warn("add stream[%s] param equal",id.c_str());
            return PUSHER_ERROR_OK;
        }

    }
    ppair = createPusherPair(id);
    if(ppair == NULL) return PUSHER_ADD_STREAM_FAILED;

    if(ppair->init(url,para._hwAccelType,para._dw,para._dh,para._fps,
                    para._gop,para._bitrate,para._redirect) != 0)
    {
        return PUSHER_INIT_STREAM_FAILED;
    }

    return PUSHER_ERROR_OK;

}

//PusherManager::PusherErrCode PusherManager::addStreamWriter(std::string id, IWriter *writer)
//{
//    PusherPair* ppair = getPusherPair(id);
//    if(ppair == NULL)
//    {
//        return PUSHER_NOT_EXIST_STREAM;
//    }
//    ppair->addWriter(id,writer);
//}

PusherManager::PusherErrCode PusherManager::addStreamReader(std::string id,std::string key)
{
    PusherPair* ppair = getPusherPair(id);
    if(ppair == NULL)
    {
        return PUSHER_NOT_EXIST_STREAM;
    }
    ppair->addReader(key);
}

bool PusherManager::waitPacket(std::string id, std::string key, Packet &pkt,int timeout)
{
    PusherPair* ppair = getPusherPair(id);
    if(ppair == NULL)
    {
        return false;
    }
    return ppair->waitPacket(key,pkt,timeout);
}

PusherManager::PusherErrCode PusherManager::removeStream(std::string id)
{
    Poco::Mutex::ScopedLock _lock(_mutex);
    if(!releasePusherPair(id))
    {
        return PUSHER_REMOVE_STREAM_FAILED;
    }
    PusherEvent evt(id,3);

    event.notify(this,evt);
    return PUSHER_ERROR_OK;
}

PusherManager::PusherErrCode PusherManager::startPush(std::string id, bool start)
{
    PusherPair* ppair = getPusherPair(id);
    if(ppair == NULL)
    {
        return PUSHER_NOT_EXIST_STREAM;
    }
    int err = start?ppair->start():ppair->stop();
    if(err != 0) return PUSHER_START_STREAM_FAILED;
    return PUSHER_ERROR_OK;
}

PusherManager::PusherErrCode PusherManager::pushFrame(VideoBuf* video)
{
    Poco::Mutex::ScopedLock _lock(_mutex);
    PusherPair* ppair = getPusherPair(video->id);
    if(ppair == NULL )
    {
        return PUSHER_NOT_EXIST_STREAM;
    }
    if(!ppair->isStarted())
    {
        if(ppair->init(video->w, video->h, video->format) != 0)
        {
            return PUSHER_INIT_STREAM_FAILED;
        }

        PusherManager::PusherErrCode err = startPush(video->id, true);
        if(err != PUSHER_ERROR_OK) return err;
    }

    ppair->push(*video);
    return PUSHER_ERROR_OK;
}

std::string PusherManager::getErrorDesc(PusherManager::PusherErrCode &code)
{
    Poco::Mutex::ScopedLock _lock(_mutex);
    static std::string errdesc[] ={"ok","stream has exist","stream hasnot exist","add stream failed","remove stream failed","start stream failed","init stream failed"};
    if( (code>=PUHSER_ERR_MAX) || (int)code < 0)
        return "unknown";
    return errdesc[code];
}

bool PusherManager::existStream(std::string id)
{
    Poco::Mutex::ScopedLock _lock(_mutex);
    if(!existPusherPair(id)) return false;
    return streamStarted(id);
}

void PusherManager::listStreams(std::vector<std::string> &streams)
{
    Poco::Mutex::ScopedLock _lock(_mutex);
    listChannels(streams);
}

bool PusherManager::streamStarted(std::string id)
{
    PusherPair* pusher = getPusherPair(id);
    if(pusher == NULL)
    {
        return false;
    }
    return pusher->isStarted();
}

void  PusherManager::getStatisticsInfo(std::string strId, Poco::UInt64& encodeFrames, Poco::UInt64& encodeLostFrames)
{
    PusherPair* pusher = getPusherPair(strId);
    if(pusher == NULL)
    {
        return;
    }
    pusher->getStatisticsInfo(encodeFrames, encodeLostFrames);
}
