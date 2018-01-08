
#include "videoqueue.h"
#include "CWLogger.h"
FrameQueue::FrameQueue(int size):
    _maxSize(size)
{

	
}
FrameQueue::~FrameQueue()
{
//FIXME 需要释放其中存放的AVFrame.
}

int FrameQueue::setMaxSize(int size)
{
    _maxSize = size;
    return _maxSize;
}

void FrameQueue::wait()
{
    _notEmpty.wait(_mutex);
}

int FrameQueue::push(AVFrame* frame)
{

    Poco::Mutex::ScopedLock lock(_mutex);
    size_t size = video_queue.size();
    if(size > _maxSize)
    {
        cw_error("frame queue full");
        return -1;
    }

    video_queue.push(frame);
    //_notEmpty.signal();

    return 0;
}



AVFrame* FrameQueue::pop()
{
    Poco::Mutex::ScopedLock lock(_mutex);
    if(video_queue.size() == 0) return NULL;

    AVFrame* frame  = video_queue.front();
    video_queue.pop();
    return frame;
}

int FrameQueue::put(AVFrame *frame)
{
    Poco::Mutex::ScopedLock lock(_mutex);
    size_t size = video_queue.size();
    if(size > _maxSize)
    {
       cw_error("frame queue full");
    }

    video_queue.push(frame);
    _notEmpty.signal();

    return 0;
}


AVFrame *FrameQueue::take(long timeout)
{
    //qDebug() << "begin taken";
    Poco::Mutex::ScopedLock lock(_mutex);

    while(video_queue.size() == 0)
    {
        if(timeout <= -1)_notEmpty.wait(_mutex);
        else
        {
            _notEmpty.tryWait(_mutex,timeout);
            if(video_queue.size() == 0) return NULL;
        }
    }

    AVFrame* frame  = video_queue.front();
    video_queue.pop();
    return frame;
}
size_t FrameQueue::size()
{
    Poco::Mutex::ScopedLock lock(_mutex);

	return video_queue.size();
}


