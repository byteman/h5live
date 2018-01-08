#ifndef FFCONFIG_H
#define FFCONFIG_H
#ifdef	__cplusplus
extern "C"
{
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/time.h"
#include "libavutil/audio_fifo.h"
#include "libswresample/swresample.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/imgutils.h"
#include "libavutil/error.h"

#ifdef __cplusplus
};
#endif


typedef enum
{
    SS_PT_VOID			 = -1, // 未知数据 void*
    SS_PT_PCMU          = 0,
    SS_PT_1016          = 1,
    SS_PT_G721          = 2,
    SS_PT_GSM           = 3,
    SS_PT_G723          = 4,
    SS_PT_DVI4_8K       = 5,
    SS_PT_DVI4_16K      = 6,
    SS_PT_LPC           = 7,
    SS_PT_PCMA          = 8,
    SS_PT_G722          = 9,
    SS_PT_S16BE_STEREO  = 10,
    SS_PT_S16BE_MONO    = 11,
    SS_PT_QCELP         = 12,
    SS_PT_CN            = 13,
    SS_PT_MPEGAUDIO     = 14,
    SS_PT_G728          = 15,
    SS_PT_DVI4_3        = 16,
    SS_PT_DVI4_4        = 17,
    SS_PT_G729          = 18,
    SS_PT_G711A         = 19,
    SS_PT_G711U         = 20,
    SS_PT_G726          = 21,
    SS_PT_G729A         = 22,
    SS_PT_LPCM          = 23,
    SS_PT_CelB          = 25,
    SS_PT_JPEG          = 26,
    SS_PT_CUSM          = 27,
    SS_PT_NV            = 28,
    SS_PT_PICW          = 29,
    SS_PT_CPV           = 30,
    SS_PT_H261          = 31,
    SS_PT_MPEGVIDEO     = 32,
    SS_PT_MPEG2TS       = 33,
    SS_PT_H263          = 34,
    SS_PT_SPEG          = 35,
    SS_PT_MPEG2VIDEO    = 36,
    SS_PT_AAC           = 37,
    SS_PT_WMA9STD       = 38,
    SS_PT_HEAAC         = 39,
    SS_PT_PCM_VOICE     = 40,
    SS_PT_PCM_AUDIO     = 41,
    SS_PT_AACLC         = 42,
    SS_PT_MP3           = 43,
    SS_PT_ADPCMA        = 49,
    SS_PT_AEC           = 50,
    SS_PT_X_LD          = 95,
    SS_PT_H264          = 96,
    SS_PT_D_GSM_HR      = 200,
    SS_PT_D_GSM_EFR     = 201,
    SS_PT_D_L8          = 202,
    SS_PT_D_RED         = 203,
    SS_PT_D_VDVI        = 204,
    SS_PT_D_BT656       = 220,
    SS_PT_D_H263_1998   = 221,
    SS_PT_D_MP1S        = 222,
    SS_PT_D_MP2P        = 223,
    SS_PT_D_BMPEG       = 224,
    SS_PT_MP4VIDEO      = 230,
    SS_PT_MP4AUDIO      = 237,
    SS_PT_VC1           = 238,
    SS_PT_JVC_ASF       = 255,
    SS_PT_D_AVI         = 256,
    SS_PT_MAX           = 257,

    /* add by hisilicon */
    SS_PT_AMR           = 1001,
    SS_PT_MJPEG         = 1002,
    /* ps stream */
    SS_PT_PS			 = 1003,
    SS_PT_PO_SLICE		 = 1004,
    SS_PT_SPEEX		 = 1005,
    SS_PT_HIK			 = 1006,
    SS_PT_DAHUA		 = 1007,

    SS_PT_YUV420		 = 1100,
    SS_PT_YV12			 = 1101,
    SS_PT_BGR			 = 1102,
    SS_PT_FLV			 = 1103

} PAYLOAD_TYPE;

#endif // FFCONFIG_H

