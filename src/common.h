#ifndef COMMON_H
#define COMMON_H
#include "Poco/Types.h"
struct VideoBuf
{
	char id[255];
	void* buffer;
	int size;
	int w;
	int h;
	Poco::Int64	pts;
	Poco::Int64     recvTime;
	Poco::UInt64	frameSeq;
	bool iskey;
	int format;
};

#endif
