#include "mediatype.h"
#include "MediaTypeMapper.h"
#include "Poco/SingletonHolder.h"
#include "Poco/FileStream.h"
#include "CWLogger.h"
static HttpMediaTypeMapper m_mediaTypes;
MediaType::MediaType()
{

}

MediaType &MediaType::get()
{
    static Poco::SingletonHolder<MediaType> sh;
    return *sh.get();
}

void MediaType::load(std::string path)
{
    try
    {
        //std::string mime_path = Poco::Path::current ()+"mime.types";
        Poco::FileInputStream fis(path);
        m_mediaTypes.load(fis);
    }
    catch(...)
    {
        cw_warn("can not load mime.types\r\n");
        loadDefaultMimes();
    }
}
void MediaType::loadDefaultMimes()
{

    m_mediaTypes.add("html","text/html");
    m_mediaTypes.add("js","application/javascript");
    m_mediaTypes.add("png","image/png");
    m_mediaTypes.add("css","text/css");
    m_mediaTypes.add("gif","image/gif");
    m_mediaTypes.add("jpg","image/jpeg");

}


const std::string& getMime(std::string& suffix)
{
    return m_mediaTypes.map(suffix);
}
