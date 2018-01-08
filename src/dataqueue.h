#ifndef DATA_CAPTURE_H
#define DATA_CAPTURE_H

#include "Poco/Mutex.h"
#include "Poco/Condition.h"
#include <queue>
#include "CWLogger.h"
#include "datapacket.h"


template <class T>
class DataQueue{
public:
    DataQueue(int size = 30):
        _maxSize(size)
    {

    }
    ~DataQueue()
    {

    }
    int setMaxSize(int size)
    {
        _maxSize = size;
        return _maxSize;
    }

    int         put(T frame)
    {
        Poco::Mutex::ScopedLock lock(_mutex);
        size_t size = data_queue.size();
        if(size > _maxSize)
        {
           cw_error("frame queue full");
        }

        data_queue.push(frame);
        _notEmpty.signal();

        return 0;
    }
    bool        peek(long timeout,T& pkt)
    {
        Poco::Mutex::ScopedLock lock(_mutex);

        while(data_queue.size() == 0)
        {
            if(timeout <= -1)_notEmpty.wait(_mutex);
            else
            {
                _notEmpty.tryWait(_mutex,timeout);
                if(data_queue.size() == 0) return false;
            }
        }

        pkt = data_queue.front();

        return true;
    }
    bool        take(long timeout,T& pkt)
    {
        Poco::Mutex::ScopedLock lock(_mutex);

        while(data_queue.size() == 0)
        {
            if(timeout <= -1)_notEmpty.wait(_mutex);
            else
            {
                _notEmpty.tryWait(_mutex,timeout);
                if(data_queue.size() == 0) return false;
            }
        }

        pkt = data_queue.front();
        data_queue.pop();
        return true;
    }
    size_t      size()
    {
        Poco::Mutex::ScopedLock lock(_mutex);

        return data_queue.size();
    }
    void clear()
    {
        Poco::Mutex::ScopedLock lock(_mutex);
        while(data_queue.size() != 0)
        {
            data_queue.pop();
        }

    }


private:

    Poco::Mutex _mutex;
    Poco::Condition  _notEmpty;
    std::queue<T> data_queue;
    int _maxSize;


};
#endif
