#include <stdint.h>
#include <ostream>
extern "C"
{
#include "flv_bytestream.h"
}
#include "FlvEncoder.h"

typedef void* flv_io_handle;


FlvEncoder::FlvEncoder(BaseWriter *pWriter)
{
	m_pWriter = pWriter;
	m_pFlvBuffer = malloc(sizeof(flv_buffer));
	memset(m_pFlvBuffer, 0, sizeof(flv_buffer));

	m_bSentHeader = false;
	m_bSentParameters = false;
}

FlvEncoder::~FlvEncoder()
{
	if(m_pFlvBuffer!=NULL) free(m_pFlvBuffer);
}

int FlvEncoder::init(BaseWriter *pWriter)
{
	m_pWriter = pWriter;
	m_pFlvBuffer = malloc(sizeof(flv_buffer));
	memset(m_pFlvBuffer, 0, sizeof(flv_buffer));

	return 0;
}

int FlvEncoder::flush(int type)
{
	flv_buffer *pBuf = (flv_buffer*)m_pFlvBuffer;

    if( !pBuf->d_cur )
        return 0;

    m_pWriter->write(type,pBuf->data, pBuf->d_cur);
    pBuf->d_total += pBuf->d_cur;

    pBuf->d_cur = 0;

    return 0;
}

int FlvEncoder::write_header()
{
	flv_buffer *pBuf = (flv_buffer*)m_pFlvBuffer;

    flv_put_tag ( pBuf, "FLV" ); // Signature
    flv_put_byte( pBuf, 1 );    // Version
    flv_put_byte( pBuf, 1 );    // Video Only
    flv_put_be32( pBuf, 9 );    // DataOffset
    flv_put_be32( pBuf, 0 );    // PreviousTagSize0

    return this->flush(TAG_HEADER);
}



H264FlvEncoder::H264FlvEncoder(BaseWriter *pWriter):FlvEncoder(pWriter)
{
	m_pSPS = NULL;
	m_pPPS = NULL;
	m_pSEI = NULL;
	m_uiSEILen = 0;
	m_uiFrameNum = 0;
	m_uiDelayTime= 0;
	m_iDelayFrames = 0; 
	m_uiInitDelta  = 0;
	m_dTimeBase    = 1.0/1000.0;

	m_bCompressDTS = true;
}

H264FlvEncoder::~H264FlvEncoder()
{
	if(m_pSEI) free(m_pSEI);
	if(m_pSPS) free(m_pSPS);
	if(m_pPPS) free(m_pPPS);
}

int H264FlvEncoder::write_video(uint8_t *data, size_t len, int64_t pts, int64_t dts)
{
	int rc = 0;
	uint8_t *p = data;
	uint8_t *lastp = data;
	//uint8_t *q = NULL;
	unsigned int lastpos = 0;
	unsigned int count = 0;

	if(len<5) return 0; //invalid data: 00 00 00 01 

	//write_nalu(data,len,pts,dts);
	for (unsigned int i = 0; i < len; i++)
	{
		if ((p[i] == 0x0 && p[i + 1] == 0x0 && p[i + 2] == 0x0 && p[i + 3] == 0x01)/* ||
			(p[i] == 0x0 && p[i + 1] == 0x0 && p[i + 2] == 0x01)*/)
		{
			if (lastpos != i)
			{
				int size = i - lastpos;
				rc = write_nalu(p + lastpos, size, pts, dts);
				if (rc < 0)
					return rc;
				count += rc;

				lastpos = i;
			}
		}
		else
		{
			if (i > len - 5)//the last piece
			{
				rc = write_nalu(p + lastpos, len - lastpos, pts, dts);
				if (rc < 0)
					return rc;
				count += rc;
				break;
			}
		}
	}

 	return count;
}

int H264FlvEncoder::write_nalu(uint8_t *data, size_t len, int64_t pts, int64_t dts)
{
	uint8_t *p = data;
	p += 4; //skip start code or AVC length
	uint8_t type = p[0] & 0x1F;

	if(this->m_bSentParameters && (type == 0x06 || type==0x07 || type==0x08))
	{
		//printf("Duplicate SPS/PPS\n");
		return 0;
	}

	switch(type)
	{
	case 0x01: //non-IDR
	case 0x02: //slice
	case 0x03:
	case 0x04:
		return write_frame(data, len, pts, dts, false);

	case 0x05: //IDR
		return write_frame(data, len, pts, dts, true);

	//case 0x06: //SEI
	//	if(m_pSEI!=NULL) { free(m_pSEI); m_pSEI=NULL; }
	//	if(m_pSEI==NULL) m_pSEI = (uint8_t*)malloc(len);
	//	if(m_pSEI==NULL) return -1;
	//	memcpy(m_pSEI, data, len);
	//	m_uiSEILen = len;
	//	break;

	case 0x07: //SPS
		if(m_pSPS!=NULL) { free(m_pSPS); m_pSPS=NULL; }
		if(m_pSPS==NULL) m_pSPS = (uint8_t*)malloc(len);
		if(m_pSPS==NULL) return -1;
		memcpy(m_pSPS, p, len-4);
		m_uiSPSLen = len-4;
		return write_sps_pps();

	case 0x08: //PPS
		if(m_pPPS!=NULL) { free(m_pPPS); m_pPPS=NULL; }
		if(m_pPPS==NULL) m_pPPS = (uint8_t*)malloc(len);
		if(m_pPPS==NULL) return -1;
		memcpy(m_pPPS, p, len-4);
		m_uiPPSLen = len-4;
		return write_sps_pps();

	case 0x06: //SEI
	case  0x9:
		return 0;
	default:
		return 0;
	}

	return -1;
}

int H264FlvEncoder::write_sps_pps()
{
	uint8_t *sps = m_pSPS;
	size_t sps_len = m_uiSPSLen;
	uint8_t *pps = m_pPPS;
	size_t pps_len = m_uiPPSLen;

	if(sps==NULL || pps==NULL) return 0; //write until both SPS and PPS are ready
    if(m_bSentHeader==false)  //send header if not yet
    {
        this->write_header();
        m_bSentHeader = true;
    }
    if(this->m_bSentParameters) return 0;

	flv_buffer *c = (flv_buffer*)m_pFlvBuffer;

	//SPS
    flv_put_byte( c, FLV_TAG_TYPE_VIDEO );
    flv_put_be24( c, 0 ); // rewrite later
    flv_put_be24( c, 0 ); // timestamp
    flv_put_byte( c, 0 ); // timestamp extended
    flv_put_be24( c, 0 ); // StreamID - Always 0
    int start = c->d_cur; // needed for overwriting length

    flv_put_byte( c, 7 | FLV_FRAME_KEY ); // Frametype and CodecID
    flv_put_byte( c, 0 ); // AVC sequence header
    flv_put_be24( c, 0 ); // composition time

    flv_put_byte( c, 1 );      // version
    flv_put_byte( c, sps[1] ); // profile
    flv_put_byte( c, sps[2] ); // profile
    flv_put_byte( c, sps[3] ); // level
    flv_put_byte( c, 0xff );   // 6 bits reserved (111111) + 2 bits nal size length - 1 (11)
    flv_put_byte( c, 0xe1 );   // 3 bits reserved (111) + 5 bits number of sps (00001)

    flv_put_be16( c, sps_len );
    flv_append_data( c, sps, sps_len );

    // PPS
    flv_put_byte( c, 1 ); // number of pps
    flv_put_be16( c, pps_len );
    flv_append_data( c, pps, pps_len );

    // rewrite data length info
    int length = c->d_cur - start;
    flv_rewrite_amf_be24( c, length, start - 10 );
    flv_put_be32( c, length + 11 ); // Last tag size
    CHECK( this->flush(TAG_SPSPPS) );

    this->m_bSentParameters = true;
    return sps_len + pps_len;
}

#define convert_timebase_ms(timestamp, timebase) (int64_t)((timestamp) * (timebase) * 1000 + 0.5)
int H264FlvEncoder::write_frame(uint8_t *data, size_t len, int64_t ipts, int64_t idts, bool bKeyFrame)
{
	int64_t dts;
	int64_t cts;
	int64_t offset;
	flv_buffer *c = (flv_buffer*)m_pFlvBuffer;

    if(this->m_bSentParameters==false)
    {
        return 0;
    }

    if( !this->m_uiFrameNum )
    {
        this->m_uiDelayTime = idts * -1;
	}

    if( this->m_bCompressDTS )
    {
        if( this->m_uiFrameNum == 1 )
            this->m_uiInitDelta = convert_timebase_ms( idts + this->m_uiDelayTime, this->m_dTimeBase );

        dts = this->m_uiFrameNum > this->m_iDelayFrames
            ? convert_timebase_ms( idts, this->m_dTimeBase )
            : this->m_uiFrameNum * this->m_uiInitDelta / (this->m_iDelayFrames + 1);

        cts = convert_timebase_ms( ipts, this->m_dTimeBase );
    }
    else
    {
        dts = convert_timebase_ms( idts + this->m_uiDelayTime, this->m_dTimeBase );
        cts = convert_timebase_ms( ipts + this->m_uiDelayTime, this->m_dTimeBase );
    }
	offset = cts - dts;

    // A new frame - write packet header
    flv_put_byte( c, FLV_TAG_TYPE_VIDEO );
    flv_put_be24( c, 0 ); // calculated later
    flv_put_be24( c, dts );
    flv_put_byte( c, dts >> 24 );
    flv_put_be24( c, 0 );

    int start = c->d_cur;
    flv_put_byte( c, (bKeyFrame ? FLV_FRAME_KEY : FLV_FRAME_INTER) | 0x7 );
    flv_put_byte( c, 1 ); // AVC NALU
    flv_put_be24( c, offset ); //composition time offset

    if( this->m_pSEI )
    {
        flv_append_data( c, this->m_pSEI, this->m_uiSEILen );
        free( this->m_pSEI );
        this->m_pSEI = NULL;
    }
    flv_put_be32(c,len-4);
    flv_append_data( c, data+4, len-4 );

    int length = c->d_cur - start;
    flv_rewrite_amf_be24( c, length, start - 10 );
    flv_put_be32( c, 11 + length ); // Last tag size
    CHECK( this->flush(bKeyFrame? TAG_NALU_I:TAG_NALU_P) );

    this->m_uiFrameNum++;

    return len;

}
