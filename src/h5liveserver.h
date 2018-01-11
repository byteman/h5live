#ifndef H5LIVESERVER_H
#define H5LIVESERVER_H

#include <string>

class H5liveServer
{
public:
    H5liveServer();
    static H5liveServer& get();
    bool start(const std::string webroot, int port);
    bool push264(const std::string &name, unsigned char* data, int size,long long pts=0);
    void addStream(const std::string &name);
    void removeStream(const std::string &name);
private:


};

#endif // H5LIVESERVER_H
