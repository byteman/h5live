#ifndef MEDIATYPE_H
#define MEDIATYPE_H

#include <string>
class MediaType
{
public:
    MediaType();
    static MediaType& get();
    void   load(std::string path);
    const  std::string& getMime(std::string& suffix);
private:

    void loadDefaultMimes();
};

#endif // MEDIATYPE_H
