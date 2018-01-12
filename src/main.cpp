#include <iostream>
#include "pushermanager.h"
#include "opencv2/opencv.hpp"
#include "ffconfig.h"
#include "streamchannel.h"
#include "h5liveserver.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Path.h"
#include "CWLogger.h"
using namespace std;
using namespace cv;

static bool _bExit = false;

BOOL __stdcall ConsoleCtrlHandler(DWORD ctrlType)
{
    switch (ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
        _bExit = true;
        printf("ctrl+c pressed!\n");
        return TRUE;
    default:
        return FALSE;
    }
}

void waitForTerminationRequest()
{
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
}
int main()
{

    std::string live = "12345";
    std::string root = Poco::Path::current() + "/web";
    H5liveServer::get().start(root, 8000);
    H5liveServer::get().addStream(live);
    PusherManager::PusherPara para;
    para._redirect = false;
    para._bitrate  = 1000000;
    para._gop = 5;
    PusherManager::get().addStream(live,"url",para);
    VideoCapture capture;
    capture.open(0);
    int h   = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    int w   = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int fps = capture.get(CV_CAP_PROP_FPS);
    printf("h5live demo start!\n");
    printf("captrue w=%d,h=%d fps=%d\n",w,h,fps);
    _bExit = false;
    CvFont font_face;
    cvInitFont(&font_face, CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC,1.0,1.0,0,1);
    while(!_bExit)
    {
        Mat frame;

        if(capture.read(frame))
        {
            VideoBuf vb;
            vb.buffer = frame.data;
            vb.w = frame.cols;
            vb.h = frame.rows;
            vb.size = vb.w*vb.h*3;
            strcpy(vb.id,live.c_str());
            vb.format = SS_PT_BGR;
            Poco::LocalDateTime ldt;
            std::string date = Poco::format("%04d-%02d-%02d %02d:%02d:%02d.%03d",ldt.year(),ldt.month(),ldt.day(),ldt.hour(),ldt.minute(),ldt.second(),ldt.millisecond());
            IplImage img(frame);
            cvPutText(&img, date.c_str(), cvPoint(10,30), &font_face, cv::Scalar(0, 255, 0));

            PusherManager::get().pushFrame(&vb);
        }
        else
        {
            cw_error("grab frame failed!\n");
        }
    }
    return 0;
}

