//
// MediaTypeMapper.cpp
//
// $Id: //poco/1.6/OSP/Web/src/MediaTypeMapper.cpp#1 $
//
// Library: OSP/Web
// Package: Web
// Module:  MediaTypeMapper
//
// Copyright (c) 2007-2014, Applied Informatics Software Engineering GmbH.
// All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//


#include "MediaTypeMapper.h"
#include "Poco/String.h"
#include <cctype>




HttpMediaTypeMapper::HttpMediaTypeMapper():
	_default("application/binary")
{
}


HttpMediaTypeMapper::~HttpMediaTypeMapper()
{
}


void HttpMediaTypeMapper::load(std::istream& istr)
{
	static const int eof = std::char_traits<char>::eof();

	int ch = istr.get();
	while (ch != eof)
	{
		while (std::isspace(ch)) ch = istr.get();
		if (ch == '#')
		{
			ch = istr.get();
			while (ch != eof && ch != '\n') ch = istr.get();
		}
		else
		{
			std::string mediaType;
			while (ch != eof && !std::isspace(ch))
			{
				mediaType += (char) ch;
				ch = istr.get();
			}
			while (ch != eof && ch != '\n')
			{
				while (ch != eof && std::isspace(ch) && ch != '\n') ch = istr.get();
				std::string ext;
				while (ch != eof && !std::isspace(ch))
				{
					ext += (char) ch;
					ch = istr.get();
				}
				if (!ext.empty())
					add(ext, mediaType);
			}
		}
	}
}


HttpMediaTypeMapper::ConstIterator HttpMediaTypeMapper::find(const std::string& suffix) const
{
	return _map.find(Poco::toLower(suffix));
}


void HttpMediaTypeMapper::add(const std::string& suffix, const std::string& mediaType)
{
	_map[Poco::toLower(suffix)] = mediaType;
}


const std::string& HttpMediaTypeMapper::map(const std::string& suffix) const
{
	ConstIterator it = find(Poco::toLower(suffix));
	if (it != end())
		return it->second;
	else
		return _default;
}


void HttpMediaTypeMapper::setDefault(const std::string& mediaType)
{
	_default = mediaType;
}

