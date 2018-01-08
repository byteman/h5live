#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include "Poco/Mutex.h"
#include "Poco/Condition.h"
#include <queue>
#include "ffconfig.h"
class FrameQueue{
public:
    FrameQueue(int size = 30);
    ~FrameQueue();
    int setMaxSize(int size);
    void        wait();
    int         push(AVFrame* frame);
    AVFrame*    pop();
    int         put(AVFrame* frame);
    AVFrame*    take(long timeout=-1);
    size_t      size();


private:

    Poco::Mutex _mutex;
    Poco::Condition  _notEmpty;
	std::queue<AVFrame*> video_queue;
    int _maxSize;
    AVFrame* _frame;


};
#endif
