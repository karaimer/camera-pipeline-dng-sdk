// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2004 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.

#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "source/XMPFiles_IO.hpp"
#include "source/XIO.hpp"

#include "XMPFiles/source/FileHandlers/PostScript_Handler.hpp"

#include "XMPFiles/source/FormatSupport/XMPScanner.hpp"
#include "XMPFiles/source/FileHandlers/Scanner_Handler.hpp"

using namespace std;

// =================================================================================================
/// \file PostScript_Handler.cpp
/// \brief File format handler for PostScript and EPS files.
///
/// This header ...
///
// =================================================================================================

static const char * kPSFileTag    = "%!PS-Adobe-";
static const size_t kPSFileTagLen = strlen ( kPSFileTag );

// =================================================================================================
// PostScript_MetaHandlerCTor
// ==========================

XMPFileHandler * PostScript_MetaHandlerCTor ( XMPFiles * parent )
{
	XMPFileHandler * newHandler = new PostScript_MetaHandler ( parent );

	return newHandler;

}	// PostScript_MetaHandlerCTor

// =================================================================================================
// PostScript_CheckFormat
// ======================

bool PostScript_CheckFormat ( XMP_FileFormat format,
	                          XMP_StringPtr  filePath,
	                          XMP_IO*    fileRef,
	                          XMPFiles *     parent )
{
	IgnoreParam(filePath); IgnoreParam(parent);
	XMP_Assert ( (format == kXMP_EPSFile) || (format == kXMP_PostScriptFile) );

	IOBuffer ioBuf;

	XMP_Int64 psOffset;
	size_t    psLength;
	XMP_Uns32 temp1, temp2;

	// Check for the binary EPSF preview header.

	fileRef->Rewind();
	if ( ! CheckFileSpace ( fileRef, &ioBuf, 4 ) ) return false;
	temp1 = GetUns32BE ( ioBuf.ptr );

	if ( temp1 == 0xC5D0D3C6 ) {

		if ( ! CheckFileSpace ( fileRef, &ioBuf, 30 ) ) return false;

		psOffset = GetUns32LE ( ioBuf.ptr+4 );	// PostScript offset.
		psLength = GetUns32LE ( ioBuf.ptr+8 );	// PostScript length.

		FillBuffer ( fileRef, psOffset, &ioBuf );	// Make sure buffer starts at psOffset for length check.
		if ( (ioBuf.len < kIOBufferSize) && (ioBuf.len < psLength) ) return false;	// Not enough PostScript.

	}

	// Check the start of the PostScript DSC header comment.

	if ( ! CheckFileSpace ( fileRef, &ioBuf, (kPSFileTagLen + 3 + 1) ) ) return false;
	if ( ! CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSFileTag), kPSFileTagLen ) ) return false;
	ioBuf.ptr += kPSFileTagLen;

	// Check the PostScript DSC major version number.

	temp1 = 0;
	while ( (ioBuf.ptr < ioBuf.limit) && ('0' <= *ioBuf.ptr) && (*ioBuf.ptr <= '9') ) {
		temp1 = (temp1 * 10) + (*ioBuf.ptr - '0');
		if ( temp1 > 1000 ) return false;	// Overflow.
		ioBuf.ptr += 1;
	}
	if ( temp1 < 3 ) return false;	// The version must be at least 3.0.

	if ( ! CheckFileSpace ( fileRef, &ioBuf, 3 ) ) return false;
	if ( *ioBuf.ptr != '.' ) return false;	// No minor number.
	ioBuf.ptr += 1;

	// Check the PostScript DSC minor version number.

	temp2 = 0;
	while ( (ioBuf.ptr < ioBuf.limit) && ('0' <= *ioBuf.ptr) && (*ioBuf.ptr <= '9') ) {
		temp2 = (temp2 * 10) + (*ioBuf.ptr - '0');
		if ( temp2 > 1000 ) return false;	// Overflow.
		ioBuf.ptr += 1;
	}
	// We don't care about the actual minor version number.

	if ( format == kXMP_PostScriptFile ) {

		// Almost done for plain PostScript, check for whitespace.

		if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return false;
		if ( (*ioBuf.ptr != ' ') && (*ioBuf.ptr != kLF) && (*ioBuf.ptr != kCR) ) return false;
		ioBuf.ptr += 1;

	} else {

		// Check for the EPSF keyword on the header comment.

		if ( ! CheckFileSpace ( fileRef, &ioBuf, 6+3+1 ) ) return false;
		if ( ! CheckBytes ( ioBuf.ptr, Uns8Ptr(" EPSF-"), 6 ) ) return false;
		ioBuf.ptr += 6;

		// Check the EPS major version number.

		temp1 = 0;
		while ( (ioBuf.ptr < ioBuf.limit) && ('0' <= *ioBuf.ptr) && (*ioBuf.ptr <= '9') ) {
			temp1 = (temp1 * 10) + (*ioBuf.ptr - '0');
			if ( temp1 > 1000 ) return false;	// Overflow.
			ioBuf.ptr += 1;
		}
		if ( temp1 < 3 ) return false;	// The version must be at least 3.0.

		if ( ! CheckFileSpace ( fileRef, &ioBuf, 3 ) ) return false;
		if ( *ioBuf.ptr != '.' ) return false;	// No minor number.
		ioBuf.ptr += 1;

		// Check the EPS minor version number.

		temp2 = 0;
		while ( (ioBuf.ptr < ioBuf.limit) && ('0' <= *ioBuf.ptr) && (*ioBuf.ptr <= '9') ) {
			temp2 = (temp2 * 10) + (*ioBuf.ptr - '0');
			if ( temp2 > 1000 ) return false;	// Overflow.
			ioBuf.ptr += 1;
		}
		// We don't care about the actual minor version number.

		if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return false;
		if ( (*ioBuf.ptr != kLF) && (*ioBuf.ptr != kCR) ) return false;
		ioBuf.ptr += 1;

	}

	return true;

}	// PostScript_CheckFormat

// =================================================================================================
// PostScript_MetaHandler::PostScript_MetaHandler
// ==============================================

PostScript_MetaHandler::PostScript_MetaHandler ( XMPFiles * _parent )
{
	this->parent = _parent;
	this->handlerFlags = kPostScript_HandlerFlags;
	this->stdCharForm = kXMP_Char8Bit;
	this->psHint = kPSHint_NoMarker;

}	// PostScript_MetaHandler::PostScript_MetaHandler

// =================================================================================================
// PostScript_MetaHandler::~PostScript_MetaHandler
// ===============================================

PostScript_MetaHandler::~PostScript_MetaHandler()
{
	// ! Inherit the base cleanup.

}	// PostScript_MetaHandler::~PostScript_MetaHandler

// =================================================================================================
// PostScript_MetaHandler::FindPostScriptHint
// ==========================================
//
// Search for "%ADO_ContainsXMP:" at the beginning of a line, it must be before "%%EndComments". If
// the XMP marker is found, look for the MainFirst/MainLast/NoMain options.

static const char * kPSContainsXMPString = "%ADO_ContainsXMP:";
static const size_t kPSContainsXMPLength = strlen ( kPSContainsXMPString );

static const char * kPSEndCommentString  = "%%EndComments";	// ! Assumed shorter than kPSContainsXMPString.
static const size_t kPSEndCommentLength  = strlen ( kPSEndCommentString );

int PostScript_MetaHandler::FindPostScriptHint()
{
	bool     found = false;
	IOBuffer ioBuf;
	XMP_Uns8 ch;

	XMP_IO* fileRef = this->parent->ioRef;

	XMP_AbortProc abortProc  = this->parent->abortProc;
	void *        abortArg   = this->parent->abortArg;
	const bool    checkAbort = (abortProc != 0);

	// Check for the binary EPSF preview header.

	fileRef->Rewind();
	if ( ! CheckFileSpace ( fileRef, &ioBuf, 4 ) ) return false;
	XMP_Uns32 temp1 = GetUns32BE ( ioBuf.ptr );

	if ( temp1 == 0xC5D0D3C6 ) {

		if ( ! CheckFileSpace ( fileRef, &ioBuf, 30 ) ) return false;

		XMP_Uns32 psOffset = GetUns32LE ( ioBuf.ptr+4 );	// PostScript offset.
		XMP_Uns32 psLength = GetUns32LE ( ioBuf.ptr+8 );	// PostScript length.

		MoveToOffset ( fileRef, psOffset, &ioBuf );

	}

	// Look for the ContainsXMP comment.

	while ( true ) {

		if ( checkAbort && abortProc(abortArg) ) {
			XMP_Throw ( "PostScript_MetaHandler::FindPostScriptHint - User abort", kXMPErr_UserAbort );
		}

		if ( ! CheckFileSpace ( fileRef, &ioBuf, kPSContainsXMPLength ) ) return kPSHint_NoMarker;

		if ( CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSEndCommentString), kPSEndCommentLength ) ) {

			// Found "%%EndComments", don't look any further.
			return kPSHint_NoMarker;

		} else if ( ! CheckBytes ( ioBuf.ptr, Uns8Ptr(kPSContainsXMPString), kPSContainsXMPLength ) ) {

			// Not "%%EndComments" or "%ADO_ContainsXMP:", skip past the end of this line.
			do {
				if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return kPSHint_NoMarker;
				ch = *ioBuf.ptr;
				++ioBuf.ptr;
			} while ( ! IsNewline ( ch ) );

		} else {

			// Found "%ADO_ContainsXMP:", look for the main packet location option.

			ioBuf.ptr += kPSContainsXMPLength;
			int xmpHint = kPSHint_NoMain;	// ! From here on, a failure means "no main", not "no marker".
			if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return kPSHint_NoMain;
			if ( ! IsSpaceOrTab ( *ioBuf.ptr ) ) return kPSHint_NoMain;

			while ( true ) {

				while ( true ) {	// Skip leading spaces and tabs.
					if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return kPSHint_NoMain;
					if ( ! IsSpaceOrTab ( *ioBuf.ptr ) ) break;
					++ioBuf.ptr;
				}
				if ( IsNewline ( *ioBuf.ptr ) ) return kPSHint_NoMain;	// Reached the end of the ContainsXMP comment.

				if ( ! CheckFileSpace ( fileRef, &ioBuf, 6 ) ) return kPSHint_NoMain;

				if ( CheckBytes ( ioBuf.ptr, Uns8Ptr("NoMain"), 6 ) ) {

					ioBuf.ptr += 6;
					xmpHint = kPSHint_NoMain;
					break;

				} else if ( CheckBytes ( ioBuf.ptr, Uns8Ptr("MainFi"), 6 ) ) {

					ioBuf.ptr += 6;
					if ( ! CheckFileSpace ( fileRef, &ioBuf, 3 ) ) return kPSHint_NoMain;
					if ( CheckBytes ( ioBuf.ptr, Uns8Ptr("rst"), 3 ) ) {
						ioBuf.ptr += 3;
						xmpHint = kPSHint_MainFirst;
					}
					break;

				} else if ( CheckBytes ( ioBuf.ptr, Uns8Ptr("MainLa"), 6 ) ) {

					ioBuf.ptr += 6;
					if ( ! CheckFileSpace ( fileRef, &ioBuf, 2 ) ) return kPSHint_NoMain;
					if ( CheckBytes ( ioBuf.ptr, Uns8Ptr("st"), 2 ) ) {
						ioBuf.ptr += 2;
						xmpHint = kPSHint_MainLast;
					}
					break;

				} else {

					while ( true ) {	// Skip until whitespace.
						if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return kPSHint_NoMain;
						if ( IsWhitespace ( *ioBuf.ptr ) ) break;
						++ioBuf.ptr;
					}

				}

			}	// Look for the main packet location option.

			// Make sure we found exactly a known option.
			if ( ! CheckFileSpace ( fileRef, &ioBuf, 1 ) ) return kPSHint_NoMain;
			if ( ! IsWhitespace ( *ioBuf.ptr ) ) return kPSHint_NoMain;
			return xmpHint;

		}	// Found "%ADO_ContainsXMP:".

	}	// Outer marker loop.

	return kPSHint_NoMarker;	// Should never reach here.

}	// PostScript_MetaHandler::FindPostScriptHint


// =================================================================================================
// PostScript_MetaHandler::FindFirstPacket
// =======================================
//
// Run the packet scanner until we find a valid packet. The first one is the main. For simplicity,
// the state of all snips is checked after each buffer is read. In theory only the last of the
// previous snips might change from partial to valid, but then we would have to special case the
// first pass when there is no previous set of snips. Since we have to get a full report to look at
// the last snip anyway, it costs virtually nothing extra to recheck all of the snips.

bool PostScript_MetaHandler::FindFirstPacket()
{
	int		snipCount;
	bool 	found	= false;
	size_t	bufPos, bufLen;

	XMP_IO* fileRef = this->parent->ioRef;
	XMP_Int64   fileLen = fileRef->Length();
	XMP_PacketInfo & packetInfo = this->packetInfo;

	XMPScanner scanner ( fileLen );
	XMPScanner::SnipInfoVector snips;

	enum { kBufferSize = 64*1024 };
	XMP_Uns8	buffer [kBufferSize];

	XMP_AbortProc abortProc  = this->parent->abortProc;
	void *        abortArg   = this->parent->abortArg;
	const bool    checkAbort = (abortProc != 0);

	bufPos = 0;
	bufLen = 0;

	fileRef->Rewind();	// Seek back to the beginning of the file.

	while ( true ) {

		if ( checkAbort && abortProc(abortArg) ) {
			XMP_Throw ( "PostScript_MetaHandler::FindFirstPacket - User abort", kXMPErr_UserAbort );
		}

		bufPos += bufLen;
		bufLen = fileRef->Read ( buffer, kBufferSize );
		if ( bufLen == 0 ) return false;	// Must be at EoF, no packets found.

		scanner.Scan ( buffer, bufPos, bufLen );
		snipCount = scanner.GetSnipCount();
		scanner.Report ( snips );

		for ( int i = 0; i < snipCount; ++i ) {
			if ( snips[i].fState == XMPScanner::eValidPacketSnip ) {
				if ( snips[i].fLength > 0x7FFFFFFF ) XMP_Throw ( "PostScript_MetaHandler::FindFirstPacket: Oversize packet", kXMPErr_BadXMP );
				packetInfo.offset = snips[i].fOffset;
				packetInfo.length = (XMP_Int32)snips[i].fLength;
				packetInfo.charForm  = snips[i].fCharForm;
				packetInfo.writeable = (snips[i].fAccess == 'w');
				return true;
			}
		}

	}

	return false;

}	// FindFirstPacket


// =================================================================================================
// PostScript_MetaHandler::FindLastPacket
// ======================================
//
// Run the packet scanner backwards until we find the start of a packet, or a valid packet. If we
// found a packet start, resume forward scanning to see if it is a valid packet. For simplicity, all
// of the snips are checked on each pass, for much the same reasons as in FindFirstPacket.

#if 1

// *** Doing this right (as described above) requires out of order scanning support which isn't
// *** implemented yet. For now we scan the whole file and pick the last valid packet.

bool PostScript_MetaHandler::FindLastPacket()
{
	int		pkt;
	size_t	bufPos, bufLen;

	XMP_IO* fileRef = this->parent->ioRef;
	XMP_Int64   fileLen = fileRef->Length();
	XMP_PacketInfo & packetInfo = this->packetInfo;

	// ------------------------------------------------------
	// Scan the entire file to find all of the valid packets.

	XMPScanner	scanner ( fileLen );

	enum { kBufferSize = 64*1024 };
	XMP_Uns8	buffer [kBufferSize];

	XMP_AbortProc abortProc  = this->parent->abortProc;
	void *        abortArg   = this->parent->abortArg;
	const bool    checkAbort = (abortProc != 0);

	fileRef->Rewind();	// Seek back to the beginning of the file.

	for ( bufPos = 0; bufPos < (size_t)fileLen; bufPos += bufLen ) {
		if ( checkAbort && abortProc(abortArg) ) {
			XMP_Throw ( "PostScript_MetaHandler::FindLastPacket - User abort", kXMPErr_UserAbort );
		}
		bufLen = fileRef->Read ( buffer, kBufferSize );
		if ( bufLen == 0 ) XMP_Throw ( "PostScript_MetaHandler::FindLastPacket: Read failure", kXMPErr_ExternalFailure );
		scanner.Scan ( buffer, bufPos, bufLen );
	}

	// -------------------------------
	// Pick the last the valid packet.

	int snipCount = scanner.GetSnipCount();

	XMPScanner::SnipInfoVector snips ( snipCount );
	scanner.Report ( snips );

	for ( pkt = snipCount-1; pkt >= 0; --pkt ) {
		if ( snips[pkt].fState == XMPScanner::eValidPacketSnip ) break;
	}

	if ( pkt >= 0 ) {
		if ( snips[pkt].fLength > 0x7FFFFFFF ) XMP_Throw ( "PostScript_MetaHandler::FindLastPacket: Oversize packet", kXMPErr_BadXMP );
		packetInfo.offset = snips[pkt].fOffset;
		packetInfo.length = (XMP_Int32)snips[pkt].fLength;
		packetInfo.charForm  = snips[pkt].fCharForm;
		packetInfo.writeable = (snips[pkt].fAccess == 'w');
		return true;
	}

	return false;

}	// PostScript_MetaHandler::FindLastPacket

#else

bool PostScript_MetaHandler::FindLastPacket()
{
	int		  err, snipCount;
	bool 	  found	= false;
	XMP_Int64 backPos, backLen;
	size_t	  ioCount;

	XMP_IO* fileRef = this->parent->fileRef;
	XMP_Int64   fileLen = fileRef->Length();
	XMP_PacketInfo & packetInfo = this->packetInfo;

	XMPScanner scanner ( fileLen );
	XMPScanner::SnipInfoVector snips;

	enum { kBufferSize = 64*1024 };
	XMP_Uns8	buffer [kBufferSize];

	XMP_AbortProc abortProc  = this->parent->abortProc;
	void *            abortArg   = this->parent->abortArg;
	const bool        checkAbort = (abortProc != 0);

	backPos = fileLen;
	backLen = 0;

	while ( true ) {

		if ( checkAbort && abortProc(abortArg) ) {
			XMP_Throw ( "PostScript_MetaHandler::FindLastPacket - User abort", kXMPErr_UserAbort );
		}

		backLen = kBufferSize;
		if ( backPos < kBufferSize ) backLen = backPos;
		if ( backLen == 0 ) return false;	// Must be at BoF, no packets found.

		backPos -= backLen;
		fileRef->Seek ( backPos, kXMP_SeekFromStart );	// Seek back to the start of the next buffer.

		#error "ioCount is 32 bits, backLen is 64"
		ioCount = fileRef->Read ( buffer, backLen );
		if ( ioCount != backLen ) XMP_Throw ( "PostScript_MetaHandler::FindLastPacket: Read failure", kXMPErr_ExternalFailure );

		scanner.Scan ( buffer, backPos, backLen );
		snipCount = scanner.GetSnipCount();
		scanner.Report ( snips );

		for ( int i = snipCount-1; i >= 0; --i ) {

			if ( snips[i].fState == XMPScanner::eValidPacketSnip ) {

				return VerifyMainPacket ( fileRef, snips[i].fOffset, snips[i].fLength, format, beLenient, mainInfo );

			} else if ( snips[i].fState == XMPScanner::ePartialPacketSnip ) {

				// This part is a tad tricky. We have a partial packet, so we need to scan
				// forward from its ending to see if it is a valid packet. Snips won't recombine,
				// the partial snip will change state. Be careful with the I/O to not clobber the
				// backward scan positions, so that it can be resumed if necessary.

				size_t fwdPos = snips[i].fOffset + snips[i].fLength;
				fileRef->Seek ( fwdPos, kXMP_SeekFromStart );	// Seek to the end of the partial snip.

				while ( (fwdPos < fileLen) && (snips[i].fState == XMPScanner::ePartialPacketSnip) ) {
					ioCount = fileRef->Read ( buffer, kBufferSize );
					if ( ioCount == 0 ) XMP_Throw ( "PostScript_MetaHandler::FindLastPacket: Read failure", kXMPErr_ExternalFailure );
					scanner.Scan ( buffer, fwdPos, ioCount );
					scanner.Report ( snips );
					fwdPos += ioCount;
				}

				if ( snips[i].fState == XMPScanner::eValidPacketSnip ) {
					if ( snips[i].fLength > 0x7FFFFFFF ) XMP_Throw ( "PostScript_MetaHandler::FindLastPacket: Oversize packet", kXMPErr_BadXMP );
					packetInfo.offset = snips[i].fOffset;
					packetInfo.length = (XMP_Int32)snips[i].fLength;
					packetInfo.charForm  = snips[i].fCharForm;
					packetInfo.writeable = (snips[i].fAccess == 'w');
					return true;
				}

			}

		}	//  Backwards snip loop.

	}	// Backwards read loop.

	return false;	// Should never get here.

}	// PostScript_MetaHandler::FindLastPacket

#endif

// =================================================================================================
// PostScript_MetaHandler::CacheFileData
// =====================================

void PostScript_MetaHandler::CacheFileData()
{
	this->containsXMP = false;
	this->psHint = FindPostScriptHint();

	if ( this->psHint == kPSHint_MainFirst ) {
		this->containsXMP = FindFirstPacket();
	} else if ( this->psHint == kPSHint_MainLast ) {
		this->containsXMP = FindLastPacket();
	}

	if ( this->containsXMP ) ReadXMPPacket ( this );

}	// PostScript_MetaHandler::CacheFileData

// =================================================================================================
