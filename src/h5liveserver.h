#ifndef H5LIVESERVER_H
#define H5LIVESERVER_H

#include <string>

class H5liveServer
{
public:
    H5liveServer();
    static H5liveServer& get();
    int start(const std::string webroot, int port);
    int push264(const std::string &name, unsigned char* data, int size,long long pts=0);
    int addStream(const std::string &name);
    int removeStream(const std::string &name);
private:


};

#endif // H5LIVESERVER_H
