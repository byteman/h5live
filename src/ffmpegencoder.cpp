#include "ffmpegencoder.h"
#include "Poco/Stopwatch.h"
#include "CWLogger.h"
#include "ffutils.h"
//#include "encoderparser.h"

FFMpegEncoder::FFMpegEncoder():
    _activity(this, &FFMpegEncoder::runActivity),
    _opened(false),
    _enable_dump(false)
    , m_totalEncodeFrames(0)
    , m_totalEncodeLostFrames(0)
{
    av_register_all();

    _pts = 0;
    codec = NULL;
    c = NULL;
}

FFMpegEncoder::~FFMpegEncoder()
{
    uninit();
}
bool FFMpegEncoder::isStopped()
{
    return _activity.isStopped();
}

void FFMpegEncoder::start()
{
    if(_activity.isRunning())
    {
       return;
    }
    _activity.start();
    _ready.wait();
}

void FFMpegEncoder::stop()
{
    if(_activity.isStopped())
    {
        return;
    }
    _activity.stop();
    _exit.wait();
}
static void fill_yuv_image(AVFrame *pict, int frame_index,
                           int width, int height)
{
    int x, y, i, ret;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally;
     * make sure we do not overwrite it here
     */
    ret = av_frame_make_writable(pict);
    if (ret < 0)
        exit(1);

    i = frame_index;

    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}


int FFMpegEncoder::openEncoder(int codec_id, int sw,int sh, int sformat,
                int dw, int dh, int fps,  int gop,int bitrate)
{


    Poco::Mutex::ScopedLock _lock(_mutex);
    if(sformat == SS_PT_YUV420) _sfmt = AV_PIX_FMT_YUV420P;
    else if(sformat==SS_PT_BGR) _sfmt = AV_PIX_FMT_BGR24;
    else
    {
        cw_error("format %d not support",sformat);
        return -4;
    }
    if(_opened)
    {
        closeEncoder();
    }

//EncoderParser p;
//p.isSupportQsv();
    AVPixelFormat format = AV_PIX_FMT_YUV420P; //编码器需要的视频像素格式.

    if(codec_id == 0) codec = avcodec_find_encoder_by_name("libx264");
    else if(codec_id == 1)
    {
        cw_warn("open encoder with qsv");
        codec = avcodec_find_encoder_by_name("h264_nvenc"); //qsv nv12
        format = AV_PIX_FMT_NV12;
    }
    else
    {
        codec = avcodec_find_encoder_by_name("libx264"); //libx264 yuv420p
        cw_warn("open encoder with x264");
    }

    if (!codec) {
        cw_error( "codec_id[%d] not found ",codec_id);
        return -1;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        cw_error("Could not allocate video codec context");
        return -2;
    }
    _codec_id = codec_id;
    /* put sample parameters */
    c->bit_rate = bitrate;
    /* resolution must be a multiple of two */
    c->width    = dw;
    c->height   = dh;
    /* frames per second */
    c->time_base.num = 1;
    c->time_base.den = fps;
    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = gop;
    c->max_b_frames = 1;
    c->pix_fmt = format;
    c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (codec->id == AV_CODEC_ID_H264)
    {
        if(codec_id == 0)
        {
            av_opt_set(c->priv_data, "preset", "ultrafast", 0);
            av_opt_set(c->priv_data, "tune", "zerolatency", 0);
            av_opt_set(c->priv_data, "profile", "baseline", 0);
            av_opt_set(c->priv_data, "level", "3.0", 0);
        }
        else if(codec_id == 1)
        {
            av_opt_set(c->priv_data, "preset", "veryfast", 0);
            av_opt_set_int(c->priv_data, "async_depth", 2, 0);
            av_opt_set(c->priv_data, "profile", "baseline", 0);  
        }

    }
    /* open it */
    int err = avcodec_open2(c, codec, NULL);
    if (err < 0) {
        if( c != NULL )
        {
            avcodec_close(c);
            av_free(c);
            c = NULL;
        }
        cw_error("Could not open codec %s\n",FFUtils::getErrorDesc(err).c_str());
        return -3;
    }
    _vpp.init(_sfmt,sw,sh,format,dw,dh);
    updateSpsPps(c);
    _dst_w = dw;
    _dst_h = dh;
    _gop = gop;
    _dst_bitrate = bitrate;
    _dst_fps = fps;
    _opened = true;
    //enableDump("out.264");
    return 0;
}

int FFMpegEncoder::setEncoderPara(int codec_id,int dw, int dh, int fps, int gop, int bitrate)
{
    _dst_w = dw;
    _dst_h = dh;
    _gop = gop;
    _dst_bitrate = bitrate;
    _dst_fps = fps;
    _codec_id = codec_id;
    return 0;
}

bool FFMpegEncoder::equal(int codec_id, int dw, int dh, int fps, int gop, int bitrate)
{
    return !( (codec_id != _codec_id) ||
             (dw!=_dst_w) ||
             (dh != _dst_h) ||
             (fps != _dst_fps) ||
             (gop != _gop) ||
             (bitrate != _dst_bitrate));

}

int FFMpegEncoder::updateEncoder(int sw, int sh, int sformat)
{

    if(_opened)
    {
        cw_warn("encoder has opened\n");
    }
    return openEncoder(_codec_id,sw,sh,sformat,_dst_w,_dst_h,_dst_fps, _gop, _dst_bitrate);
}

int FFMpegEncoder::uninit()
{
    _vpp.uninit();
    return closeEncoder();
}

void FFMpegEncoder::enableDump(std::string file)
{


    try
    {

        _fos.open(file,std::ios::out|std::ios::binary);
        _enable_dump = true;
    }
    catch(Poco::Exception& e)
    {

    }


}
static AVFrame* allocFrame(void* data,  int width, int height,AVPixelFormat format)
{
    AVFrame* frame = FFUtils::alloc_picture(format, width,height);
#if 0
    av_image_fill_arrays(frame->data,frame->linesize,(const uint8_t*)data,format,width,height,32);
    av_frame_make_writable(frame);
#else

    if(frame == NULL) return NULL;

    int t_w = FFALIGN(width,16);
    int t_h = FFALIGN(height,16);

    //AVFrame* tmpframe = FFUtils::alloc_picture(format, t_w,t_h);
    AVFrame* tmpframe = FFUtils::alloc_picture(format, width,height);
    if(tmpframe == NULL)
    {
        cw_error("alloc_picture failed");
        av_frame_free(&frame);
        return NULL;
    }
    //这里align不能使用32或者16，在图片的分辨率不是16的倍数的时候，会image_copy会报错。
    av_image_fill_arrays(tmpframe->data,tmpframe->linesize,(const uint8_t*)data,format,width,height,1);

    //av_frame_copy_props(frame,tmpframe);

    av_image_copy(frame->data,frame->linesize,(const uint8_t**)tmpframe->data,tmpframe->linesize,format,width,height);
    //av_frame_copy(frame,tmpframe);

    av_frame_free(&tmpframe);
#endif
    return frame;
}
AVFrame* FFMpegEncoder::getTempFrame()
{
    static int i = 0;
    if(_frame == NULL)
    {
        _frame = FFUtils::alloc_picture(AV_PIX_FMT_YUV420P, c->width,c->height);

    }
    fill_yuv_image(_frame,i++,c->width,c->height);

    return allocFrame(_frame->data,_frame->width,_frame->height,(AVPixelFormat)_frame->format);

}

bool FFMpegEncoder::updateSpsPps(AVCodecContext *ctx)
{

    int i = 0;
    int pps_i = 0;
    int sps_i = 0;
    for(i = 0 ; i < ctx->extradata_size; i++)
    {
        if( (i + 4 ) >= ctx->extradata_size)
            break;
        if( (ctx->extradata[i+0]==0) && (ctx->extradata[i+1]==0) && (ctx->extradata[i+2]==0) && (ctx->extradata[i+3]==1) )
        {
            unsigned char naltype = ctx->extradata[i+4]&0x1F;
            if(naltype == 8)
            {
                                pps_i = i;
                                break;
            }

            else if(naltype == 7)
            {
                sps_i = i;
            }
        }
        printf(" %02x ",ctx->extradata[i]);

    }

    printf("\n");
    sps.assign(ctx->extradata+sps_i , ctx->extradata + pps_i );
    pps.assign(ctx->extradata+pps_i , ctx->extradata+ ctx->extradata_size);

    printf("sps - ");
    for(int i = 0; i < sps.size(); i++)
    {
        printf(" %02x ", sps[i]);
    }
     printf("\n");
      printf("pps - ");
     for(int i = 0; i < pps.size(); i++)
     {
         printf(" %02x ", pps[i]);
     }
      printf("\n");
      return true;
}

int FFMpegEncoder::pushFrame(void* data,  int width, int height,int64_t pts)
{
    m_totalEncodeFrames++;
    /* encode 1 second of video */
    if(!_opened) return -1;

    //return _queue.push(getTempFrame());
    if(_queue.size() > 30)
    {
        m_totalEncodeLostFrames++;
        cw_error("encoder queue full");
        return -3;
    }
    AVFrame* frame = allocFrame(data,width,height,_sfmt);
    if(frame == NULL)
    {
        m_totalEncodeLostFrames++;
        return -2;
    }

    return _queue.put(frame);

    return 0;
}
#include "Poco/Format.h"
void dump_packet(AVPacket* pkt)
{
    static int i = 0;
    std::string name = Poco::format("%d.pkt",i++);

    Poco::FileOutputStream fos;
    fos.open(name,std::ios::out|std::ios::binary);
    fos.write((const char*)pkt->data,pkt->size);
    fos.close();
}
int FFMpegEncoder::parse(AVPacket* pkt)
{
    static bool first = false;
    pkt->flags = 0;
    while(pkt->size > 6)
    {
        if( ( pkt->data[0] == 0 ) &&
            ( pkt->data[1] == 0 ) &&
            ( pkt->data[2] == 0 ) &&
            ( pkt->data[3] == 1 )    )
        {
            unsigned char naltype = ( (unsigned char)pkt->data[4] & 0x1F);


                if( naltype == 7 )
                {
                    if(!first)
                    {
                        pkt->flags = 1;

                        first = true;
                    break;
                }

            }

        }

        if( ( pkt->data[0] == 0 ) &&
            ( pkt->data[1] == 0 ) &&
            ( pkt->data[2] == 1 ))
        {
            unsigned char naltype = ( (unsigned char)pkt->data[3] & 0x1F);
            if( naltype == 1 || naltype == 5)
            {
                if(naltype == 5)
                    pkt->flags = 1;
                pkt->size++;
                pkt->data--;
                pkt->data[0] = 0;
                break;
            }

        }
        pkt->size--;
        pkt->data++;
    }
    return 0;
}
void FFMpegEncoder::flush()
{
    AVPacket pkt;
    int ret = 0,got_output = 0;
    av_init_packet(&pkt);
    pkt.data = NULL;    // packet data will be allocated by the encoder
    pkt.size = 0;

    for (got_output = 1; got_output; ) {

           ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
           if (ret < 0) {
               fprintf(stderr, "Error encoding frame\n");
               return;
           }

           if (got_output) {
                   if( (pkt.data[0] == 0) &&
                       (pkt.data[1] == 0) &&
                       (pkt.data[2] == 0) &&
                       (pkt.data[3] == 1) &&
                       (pkt.data[4] == 9))
                   {
                       pkt.data += 6;
                       pkt.size -= 6;
                   }
                    //parse(&pkt);
                    //qDebug() << "got ";
                    //dump_packet(&pkt);
                   onEncodeComplete.notify(this,pkt);
                   if(_enable_dump)
                   {
                       _fos.write((const char*)pkt.data,pkt.size);
                   }
               av_packet_unref(&pkt);
               flush();
           }
       }
}
int FFMpegEncoder::encode(AVFrame* pFrame)
{
    AVPacket pkt;
    if(pFrame == NULL)
    {
        return -2;
    }
    av_init_packet(&pkt);
    pkt.data = NULL;    // packet data will be allocated by the encoder
    pkt.size = 0;


    pFrame->pts = _pts++;
    int got_output = 0;
    /* encode the image */
    //cw_error("begin encode");
    int ret = avcodec_encode_video2(c, &pkt, pFrame, &got_output);
    //cw_error("end encode");
    if (ret < 0) {
        m_totalEncodeLostFrames++;
        cw_error("encode error");
        return -1;
    }

    if (got_output) {

        if( (pkt.data[0] == 0) &&
            (pkt.data[1] == 0) &&
            (pkt.data[2] == 0) &&
            (pkt.data[3] == 1) &&
            (pkt.data[4] == 9))
        {
            pkt.data += 6;
            pkt.size -= 6;
        }
         //parse(&pkt);
         //qDebug() << "got ";
         //dump_packet(&pkt);
        onEncodeComplete.notify(this,pkt);
        if(_enable_dump)
        {
            _fos.write((const char*)pkt.data,pkt.size);
        }

        av_packet_unref(&pkt);
        if(_codec_id == 1)
            flush();
    }

    return 0;
}

int FFMpegEncoder::closeEncoder()
{
    if(!_opened) return 0;
     avcodec_close(c);
     av_free(c);
     c = NULL;
      _opened = false;
     return 0;
}

void FFMpegEncoder::runActivity()
{
    _ready.set();

    while(!_activity.isStopped())
    {

        //cw_error("taken one frame");
        AVFrame* pFrame = _queue.take(100);
        if(pFrame==NULL)
        {
            continue;
        }
        //Poco::Stopwatch  sw;
        //sw.start();
        AVFrame* frame =_vpp.filter(pFrame);
        encode(frame);
        //sw.stop();
        //cw_info("encode elipsed %lld",sw.elapsed());

        av_frame_free(&pFrame);

    }
    _exit.set();
}

std::vector<unsigned char> &FFMpegEncoder::getPps()
{
    return pps;
}

std::vector<unsigned char> &FFMpegEncoder::getSps()
{
    return sps;
}

void  FFMpegEncoder::getStatisticsInfo(Poco::UInt64& encodeFrames, Poco::UInt64& encodeLostFrames)
{
    encodeFrames = m_totalEncodeFrames;
    encodeLostFrames = m_totalEncodeFrames;
}
