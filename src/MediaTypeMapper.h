//
// MediaTypeMapper.h
//
// $Id: //poco/1.6/OSP/Web/include/Poco/OSP/Web/MediaTypeMapper.h#1 $
//
// Library: OSP/Web
// Package: Web
// Module:  MediaTypeMapper
//
// Definition of the MediaTypeMapper class.
//
// Copyright (c) 2007-2014, Applied Informatics Software Engineering GmbH.
// All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//


#ifndef OSP_Web_MediaTypeMapper_INCLUDED
#define OSP_Web_MediaTypeMapper_INCLUDED



//#include "Poco/OSP/Service.h"
#include "Poco/AutoPtr.h"
#include <map>
#include <istream>



class  HttpMediaTypeMapper
	/// The MediaTypeMapper service maps file extensions to MIME media types.
	///
	/// For more information on MIME media types, please see RFC 2045 and RFC 2046.
	///
	/// The MediaTypeMapper can load extension to media type associations from
	/// a configuration file. The configuration file has the same format as the
	/// Apache mime.types file, used by the Apache mod_mime module (see
	/// http://httpd.apache.org/docs/1.3/mod/mod_mime.html).
	///
	/// The mime.types file consists of lines, each containing an association
	/// between a media type and zero or more extensions. Media type and
	/// extensions are delimited by whitespace. Lines starting with a hash
	/// character ('#') are treated as comments and ignored.
	///
	/// The name of the MediaTypeMapper service is "osp.web.mediatype".
{
public:
	typedef Poco::AutoPtr<HttpMediaTypeMapper> Ptr;
	typedef std::map<std::string, std::string> SuffixToMediaTypeMap;
	typedef SuffixToMediaTypeMap::const_iterator ConstIterator;

	HttpMediaTypeMapper();
		/// Creates an empty MediaTypeMapper.

	~HttpMediaTypeMapper();
		/// Destroys the MediaTypeMapper.

	void load(std::istream& str);
		/// Loads mappings from a stream or file conforming to the
		/// Apache mime.types file format. The mappings from the file
		/// are added to the existing mappings.

	void add(const std::string& suffix, const std::string& mediaType);
		/// Adds the suffix->MIME media type mapping if no entry exists for the suffix yet.
		/// Suffix is not case sensitive.

	ConstIterator find(const std::string& suffix) const;
		/// Searches a MIME media type for the given suffix. Returns end() for unknown mappings. 
		/// Note that the search is not case sensitive!

	ConstIterator begin() const;
		/// Returns the begin iterator for the suffix to MIME media type map.

	ConstIterator end() const;
		/// Returns the end iterator for the suffix to MIME media type map.

	const std::string& map(const std::string& suffix) const;
		/// Returns a MIME media type for the given suffix. If no media type has been
		/// registered for the given suffix, the default media type
		/// (application/binary, unless anothe one has been set) is returned.
		/// Note that the search is not case sensitive!

	void setDefault(const std::string& mediaType);
		/// Sets a default MIME media type for unknown suffixes.
		
	const std::string& getDefault() const;
		/// Returns the default MIME media type.
		///
		/// Unless another default type has been set with setDefault(),
		/// the default type is application/binary.

	

private:
	SuffixToMediaTypeMap _map;
	std::string          _default;
};


//
// inlines
//
inline HttpMediaTypeMapper::ConstIterator HttpMediaTypeMapper::begin() const
{
	return _map.begin();
}


inline HttpMediaTypeMapper::ConstIterator HttpMediaTypeMapper::end() const
{
	return _map.end();
}


inline const std::string& HttpMediaTypeMapper::getDefault() const
{
	return _default;
}





#endif // OSP_Web_MediaTypeMapper_INCLUDED
