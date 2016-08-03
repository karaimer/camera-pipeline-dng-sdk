// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! This must be the first include.

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "XMPFiles/source/FormatSupport/ID3_Support.hpp"
#include "XMPFiles/source/FormatSupport/Reconcile_Impl.hpp"

#include "source/UnicodeConversions.hpp"
#include "source/XIO.hpp"

#include <vector>

// =================================================================================================

#define MIN(a,b)	((a) < (b) ? (a) : (b))

ID3GenreMap* kMapID3GenreCodeToName = 0;	// Map from a code like "21" or "RX" to the full name.
ID3GenreMap* kMapID3GenreNameToCode = 0;	// Map from the full name to a code like "21" or "RX".

// =================================================================================================

bool ID3_Support::InitializeGlobals()
{
	return true;
}

// =================================================================================================

void ID3_Support::TerminateGlobals()
{
	// nothing yet
}

// =================================================================================================
// ID3Header
// =================================================================================================

bool ID3_Support::ID3Header::read ( XMP_IO* file )
{

	XMP_Assert ( sizeof(fields) == kID3_TagHeaderSize );
	file->ReadAll ( this->fields, kID3_TagHeaderSize );

	if ( ! CheckBytes ( &this->fields[ID3Header::o_id], "ID3", 3 ) ) {
		// chuck in default contents:
		const static char defaultHeader[kID3_TagHeaderSize] = { 'I', 'D', '3', 3, 0, 0, 0, 0, 0, 0 };
		memcpy ( this->fields, defaultHeader, kID3_TagHeaderSize );
		return false; // no header found (o.k.) thus stick with new, default header constructed above
	}

	XMP_Uns8 major = this->fields[o_vMajor];
	XMP_Uns8 minor = this->fields[o_vMinor];
	XMP_Validate ( ((2 <= major) && (major <= 4)), "Invalid ID3 major version", kXMPErr_BadFileFormat );

	return true;

}

// =================================================================================================

void ID3_Support::ID3Header::write ( XMP_IO* file, XMP_Int64 tagSize )
{

	XMP_Assert ( (kID3_TagHeaderSize <= tagSize) && (tagSize < 256*1024*1024) );	// 256 MB limit due to synching.

	XMP_Uns32 synchSize = int32ToSynch ( (XMP_Uns32)tagSize - kID3_TagHeaderSize );
	PutUns32BE ( synchSize, &this->fields[ID3Header::o_size] );
	file->Write ( this->fields, kID3_TagHeaderSize );

}

// =================================================================================================
// ID3v2Frame
// =================================================================================================

#define frameDefaults	id(0), flags(0), content(0), contentSize(0), active(true), changed(false)

ID3_Support::ID3v2Frame::ID3v2Frame() : frameDefaults
{
	XMP_Assert ( sizeof(fields) == kV23_FrameHeaderSize );	// Only need to do this in one place.
	memset ( this->fields, 0, kV23_FrameHeaderSize );
}

// =================================================================================================

ID3_Support::ID3v2Frame::ID3v2Frame ( XMP_Uns32 id ) : frameDefaults
{
	memset ( this->fields, 0, kV23_FrameHeaderSize );
	this->id = id;
	PutUns32BE ( id, &this->fields[o_id] );
}

// =================================================================================================

void ID3_Support::ID3v2Frame::release()
{
	if ( this->content != 0 ) delete this->content;
	this->content = 0;
	this->contentSize = 0;
}

// =================================================================================================

void ID3_Support::ID3v2Frame::setFrameValue ( const std::string& rawvalue, bool needDescriptor,
											  bool utf16, bool isXMPPRIVFrame, bool needEncodingByte )
{

	std::string value;

	if ( isXMPPRIVFrame ) {

		XMP_Assert ( (! needDescriptor) && (! utf16) );

		value.append ( "XMP\0", 4 );
		value.append ( rawvalue );
		value.append ( "\0", 1  ); // final zero byte

	} else {

		if ( needEncodingByte ) {
			if ( utf16 ) {
				value.append ( "\x1", 1  );
			} else {
				value.append ( "\x0", 1  );
			}
		}

		if ( needDescriptor ) value.append ( "eng", 3 );

		if ( utf16 ) {

			if ( needDescriptor ) value.append ( "\xFF\xFE\0\0", 4 );

			value.append ( "\xFF\xFE", 2 );
			std::string utf16str;
			ToUTF16 ( (XMP_Uns8*) rawvalue.c_str(), rawvalue.size(), &utf16str, false );
			value.append ( utf16str );
			value.append ( "\0\0", 2 );

		} else {

			std::string convertedValue;
			ReconcileUtils::UTF8ToLatin1 ( rawvalue.c_str(), rawvalue.size(), &convertedValue );

			if ( needDescriptor ) value.append ( "\0", 1 );
			value.append ( convertedValue );
			value.append ( "\0", 1  );

		}

	}

	this->changed = true;
	this->release();

	this->contentSize = (XMP_Int32) value.size();
	XMP_Validate ( (this->contentSize < 20*1024*1024), "XMP Property exceeds 20MB in size", kXMPErr_InternalFailure );
	this->content = new char [ this->contentSize ];
	memcpy ( this->content, value.c_str(), this->contentSize );

}	// ID3v2Frame::setFrameValue

// =================================================================================================

XMP_Int64 ID3_Support::ID3v2Frame::read ( XMP_IO* file, XMP_Uns8 majorVersion )
{
	XMP_Assert ( (2 <= majorVersion) && (majorVersion <= 4) );

	this->release(); // ensures/allows reuse of 'curFrame'
	XMP_Int64 start = file->Offset();
	
	if ( majorVersion > 2 ) {
		file->ReadAll ( this->fields, kV23_FrameHeaderSize );
	} else {
		// Read the 6 byte v2.2 header into the 10 byte form.
		memset ( this->fields, 0, kV23_FrameHeaderSize );	// Clear all of the bytes.
		file->ReadAll ( &this->fields[o_id], 3 );		// Leave the low order byte as zero.
		file->ReadAll ( &this->fields[o_size+1], 3 );	// Read big endian UInt24.
	}

	this->id = GetUns32BE ( &this->fields[o_id] );

	if ( this->id == 0 ) {
		file->Seek ( start, kXMP_SeekFromStart );	// Zero ID must mean nothing but padding.
		return 0;
	}

	this->flags = GetUns16BE ( &this->fields[o_flags] );
	XMP_Validate ( (0 == (this->flags & 0xEE)), "invalid lower bits in frame flags", kXMPErr_BadFileFormat );

	//*** flag handling, spec :429ff aka line 431ff  (i.e. Frame should be discarded)
	//  compression and all of that..., unsynchronisation
	this->contentSize = GetUns32BE ( &this->fields[o_size] );
	if ( majorVersion == 4 ) this->contentSize = synchToInt32 ( this->contentSize );

	XMP_Validate ( (this->contentSize >= 0), "negative frame size", kXMPErr_BadFileFormat );
	XMP_Validate ( (this->contentSize < 20*1024*1024), "single frame exceeds 20MB", kXMPErr_BadFileFormat );

	this->content = new char [ this->contentSize ];

	file->ReadAll ( this->content, this->contentSize );
	return file->Offset() - start;

}	// ID3v2Frame::read

// =================================================================================================

void ID3_Support::ID3v2Frame::write ( XMP_IO* file, XMP_Uns8 majorVersion )
{
	XMP_Assert ( (2 <= majorVersion) && (majorVersion <= 4) );

	if ( majorVersion < 4 ) {
		PutUns32BE ( this->contentSize, &this->fields[o_size] );
	} else {
		PutUns32BE ( int32ToSynch ( this->contentSize ), &this->fields[o_size] );
	}

	if ( majorVersion > 2 ) {
		file->Write ( this->fields, kV23_FrameHeaderSize );
	} else {
		file->Write ( &this->fields[o_id], 3 );
		file->Write ( &this->fields[o_size+1], 3 );
	}

	file->Write ( this->content, this->contentSize );

}	// ID3v2Frame::write

// =================================================================================================
		
bool ID3_Support::ID3v2Frame::advancePastCOMMDescriptor ( XMP_Int32& pos )
{

		if ( (this->contentSize - pos) <= 3 ) return false; // silent error, no room left behing language tag
		if ( ! CheckBytes ( &this->content[pos], "eng", 3 ) ) return false; // not an error, but leave all non-eng tags alone...

		pos += 3; // skip lang tag
		if ( pos >= this->contentSize ) return false; // silent error

		while ( pos < this->contentSize ) {
			if ( this->content[pos++] == 0x00 ) break;
		}
		if ( (pos < this->contentSize) && (this->content[pos] == 0x00) ) pos++;

		if ( (pos == 5) && (this->contentSize == 6) && (GetUns16BE(&this->content[4]) == 0x0031) ) {
			return false;
		}

		if ( pos > 4 ) {
			std::string descriptor = std::string ( &this->content[4], pos-1 );
			if ( 0 == descriptor.substr(0,4).compare( "iTun" ) ) {	// begins with engiTun ?
				return false; // leave alone, then
			}
		}

		return true; //o.k., descriptor skipped, time for the real thing.

}	// ID3v2Frame::advancePastCOMMDescriptor

// =================================================================================================

bool ID3_Support::ID3v2Frame::getFrameValue ( XMP_Uns8 majorVersion, XMP_Uns32 logicalID, std::string* utf8string )
{

	XMP_Assert ( (this->content != 0) && (this->contentSize >= 0) && (this->contentSize < 20*1024*1024) );

	if ( this->contentSize == 0 ) {
		utf8string->erase();
		return true; // ...it is "of interest", even if empty contents.
	}

	XMP_Int32 pos = 0;
	XMP_Uns8 encByte = 0;
	// WCOP does not have an encoding byte, for all others: use [0] as EncByte, advance pos
	if ( logicalID != 0x57434F50 ) {
		encByte = this->content[0];
		pos++;
	}

	// mode specific forks, COMM or USLT
	bool commMode = ( (logicalID == 0x434F4D4D) || (logicalID == 0x55534C54) );

	switch ( encByte ) {

		case 0: //ISO-8859-1, 0-terminated
		{
			if ( commMode && (! advancePastCOMMDescriptor ( pos )) ) return false; // not a frame of interest!

			char* localPtr  = &this->content[pos];
			size_t localLen = this->contentSize - pos;
			ReconcileUtils::Latin1ToUTF8 ( localPtr, localLen, utf8string );
			break;

		}

		case 1: // Unicode, v2.4: UTF-16 (undetermined Endianess), with BOM, terminated 0x00 00
		case 2: // UTF-16BE without BOM, terminated 0x00 00
		{

			if ( commMode && (! advancePastCOMMDescriptor ( pos )) ) return false; // not a frame of interest!

			std::string tmp ( this->content, this->contentSize );
			bool bigEndian = true;	// assume for now (if no BOM follows)

			if ( GetUns16BE ( &this->content[pos] ) == 0xFEFF ) {
				pos += 2;
				bigEndian = true;
			} else if ( GetUns16BE ( &this->content[pos] ) == 0xFFFE ) {
				pos += 2;
				bigEndian = false;
			}

			FromUTF16 ( (UTF16Unit*)&this->content[pos], ((this->contentSize - pos)) / 2, utf8string, bigEndian );
			break;

		}

		case 3: // UTF-8 unicode, terminated \0
		{
			if ( commMode && (! advancePastCOMMDescriptor ( pos )) ) return false; // not a frame of interest!
		
			if ( (GetUns32BE ( &this->content[pos]) & 0xFFFFFF00 ) == 0xEFBBBF00 ) {
				pos += 3;	// swallow any BOM, just in case
			}

			utf8string->assign ( &this->content[pos], (this->contentSize - pos) );
			break;
		}

		default:
			XMP_Throw ( "unknown text encoding", kXMPErr_BadFileFormat ); //COULDDO assume latin-1 or utf-8 as best-effort
			break;

	}

	return true;

}	// ID3v2Frame::getFrameValue

// =================================================================================================
// ID3v1Tag
// =================================================================================================

bool ID3_Support::ID3v1Tag::read ( XMP_IO* file, SXMPMeta* meta )
{
	// returns returns true, if ID3v1 (or v1.1) exists, otherwise false, sets XMP properties en route

	if ( file->Length() <= 128 ) return false;  // ensure sufficient room
	file->Seek ( -128, kXMP_SeekFromEnd );

	XMP_Uns32 tagID = XIO::ReadInt32_BE ( file );
	tagID = tagID & 0xFFFFFF00; // wipe 4th byte
	if ( tagID != 0x54414700 ) return false; // must be "TAG"
	file->Seek ( -1, kXMP_SeekFromCurrent  ); //rewind 1

	XMP_Uns8 buffer[31]; // nothing is bigger here, than 30 bytes (offsets [0]-[29])
	buffer[30] = 0;		 // wipe last byte
	std::string utf8string;

	file->ReadAll ( buffer, 30 );
	std::string title ( (char*) buffer ); //security: guaranteed to 0-terminate after 30 bytes
	if ( ! title.empty() ) {
		ReconcileUtils::Latin1ToUTF8 ( title.c_str(), title.size(), &utf8string );
		meta->SetLocalizedText ( kXMP_NS_DC, "title", "", "x-default", utf8string.c_str() );
	}

	file->ReadAll ( buffer, 30 );
	std::string artist( (char*) buffer );
	if ( ! artist.empty() ) {
		ReconcileUtils::Latin1ToUTF8 ( artist.c_str(), artist.size(), &utf8string );
		meta->SetProperty ( kXMP_NS_DM, "artist", utf8string.c_str() );
	}

	file->ReadAll ( buffer, 30 );
	std::string album( (char*) buffer );
	if ( ! album.empty() ) {
		ReconcileUtils::Latin1ToUTF8 ( album.c_str(), album.size(), &utf8string );
		meta->SetProperty ( kXMP_NS_DM, "album", utf8string.c_str() );
	}

	file->ReadAll ( buffer, 4 );
	buffer[4]=0; // ensure 0-term
	std::string year( (char*) buffer );
	if ( ! year.empty() ) {	// should be moot for a year, but let's be safe:
		ReconcileUtils::Latin1ToUTF8 ( year.c_str(), year.size(), &utf8string );
		meta->SetProperty ( kXMP_NS_XMP, "CreateDate",  utf8string.c_str() );
	}

	file->ReadAll ( buffer, 30 );
	std::string comment( (char*) buffer );
	if ( ! comment.empty() ) {
		ReconcileUtils::Latin1ToUTF8 ( comment.c_str(), comment.size(), &utf8string );
		meta->SetProperty ( kXMP_NS_DM, "logComment", utf8string.c_str() );
	}

	if ( buffer[28] == 0 ) {
		XMP_Uns8 trackNo = buffer[29];
		if ( trackNo > 0 ) {
			std::string trackStr;
			meta->SetProperty_Int ( kXMP_NS_DM, "trackNumber", trackNo );
		}
	}

	XMP_Uns8 genreNo = XIO::ReadUns8 ( file );
	if ( genreNo < 127 ) {
		meta->SetProperty ( kXMP_NS_DM, "genre", Genres[genreNo] );
	}

	return true; // ID3Tag found

}	// ID3v1Tag::read

// =================================================================================================

void ID3_Support::ID3v1Tag::write ( XMP_IO* file, SXMPMeta* meta )
{

	std::string zeros ( 128, '\0' );
	std::string utf8, latin1;

	file->Seek ( -128, kXMP_SeekFromEnd );
	file->Write ( zeros.data(), 128 );

	file->Seek ( -128, kXMP_SeekFromEnd );
	XIO::WriteUns8 ( file, 'T' );
	XIO::WriteUns8 ( file, 'A' );
	XIO::WriteUns8 ( file, 'G' );

	if ( meta->GetLocalizedText ( kXMP_NS_DC, "title", "", "x-default", 0, &utf8, 0 ) ) {
		file->Seek ( (-128 + 3), kXMP_SeekFromEnd );
		ReconcileUtils::UTF8ToLatin1 ( utf8.c_str(), utf8.size(), &latin1 );
		file->Write ( latin1.c_str(), MIN ( 30, (XMP_Int32)latin1.size() ) );
	}

	if ( meta->GetProperty ( kXMP_NS_DM, "artist", &utf8, 0 ) ) {
		file->Seek ( (-128 + 33), kXMP_SeekFromEnd );
		ReconcileUtils::UTF8ToLatin1 ( utf8.c_str(), utf8.size(), &latin1 );
		file->Write ( latin1.c_str(), MIN ( 30, (XMP_Int32)latin1.size() ) );
	}

	if ( meta->GetProperty ( kXMP_NS_DM, "album", &utf8, 0 ) ) {
		file->Seek ( (-128 + 63), kXMP_SeekFromEnd );
		ReconcileUtils::UTF8ToLatin1 ( utf8.c_str(), utf8.size(), &latin1 );
		file->Write ( latin1.c_str(), MIN ( 30, (XMP_Int32)latin1.size() ) );
	}

	if ( meta->GetProperty ( kXMP_NS_XMP, "CreateDate", &utf8, 0 ) ) {
		XMP_DateTime dateTime;
		SXMPUtils::ConvertToDate( utf8, &dateTime );
		if ( dateTime.hasDate ) {
			SXMPUtils::ConvertFromInt ( dateTime.year, "", &latin1 );
			file->Seek ( (-128 + 93), kXMP_SeekFromEnd );
			file->Write ( latin1.c_str(), MIN ( 4, (XMP_Int32)latin1.size() ) );
		}
	}

	if ( meta->GetProperty ( kXMP_NS_DM, "logComment", &utf8, 0 ) ) {
		file->Seek ( (-128 + 97), kXMP_SeekFromEnd );
		ReconcileUtils::UTF8ToLatin1 ( utf8.c_str(), utf8.size(), &latin1 );
		file->Write ( latin1.c_str(), MIN ( 30, (XMP_Int32)latin1.size() ) );
	}

	if ( meta->GetProperty ( kXMP_NS_DM, "genre", &utf8, 0 ) ) {

		XMP_Uns8 genreNo = 0;

		int i;
		const char* genreCString = utf8.c_str();
		for ( i = 0; i < 127; ++i ) {
			if ( (strlen(genreCString) == strlen(Genres[i])) &&  //fixing buggy stricmp behaviour on PPC
				 (stricmp ( genreCString, Genres[i] ) == 0 ) ) {
				genreNo = i; // found
				break;
			}
		}

		file->Seek ( (-128 + 127), kXMP_SeekFromEnd );
		XIO::WriteUns8 ( file, genreNo );

	}

	if ( meta->GetProperty ( kXMP_NS_DM, "trackNumber", &utf8, kXMP_NoOptions ) ) {

		XMP_Uns8 trackNo = 0;
		try {
			trackNo = (XMP_Uns8) SXMPUtils::ConvertToInt ( utf8.c_str() );
			file->Seek ( (-128 + 125), kXMP_SeekFromEnd );
			XIO::WriteUns8 ( file, 0 ); // ID3v1.1 extension
			XIO::WriteUns8 ( file, trackNo );
		} catch ( ... ) {
			// forgive, just don't set this one.
		}

	}

}	// ID3v1Tag::write
