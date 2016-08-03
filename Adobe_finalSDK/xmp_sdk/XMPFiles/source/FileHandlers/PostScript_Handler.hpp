#ifndef __PostScript_Handler_hpp__
#define __PostScript_Handler_hpp__	1

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

#include "XMPFiles/source/FileHandlers/Trivial_Handler.hpp"

// =================================================================================================
/// \file PostScript_Handler.hpp
/// \brief File format handler for PostScript and EPS files.
///
/// This header ...
///
// =================================================================================================

// *** This probably could be derived from Basic_Handler, buffer the file tail in a temp file.

extern XMPFileHandler * PostScript_MetaHandlerCTor ( XMPFiles * parent );

extern bool PostScript_CheckFormat ( XMP_FileFormat format,
									 XMP_StringPtr  filePath,
			                         XMP_IO *       fileRef,
			                         XMPFiles *     parent );

static const XMP_OptionBits kPostScript_HandlerFlags = kTrivial_HandlerFlags;

enum {
	kPSHint_NoMarker  = 0,
	kPSHint_NoMain    = 1,
	kPSHint_MainFirst = 2,
	kPSHint_MainLast  = 3
};

class PostScript_MetaHandler : public Trivial_MetaHandler
{
public:

	PostScript_MetaHandler ( XMPFiles * parent );
	~PostScript_MetaHandler();

	void CacheFileData();

	int psHint;

protected:

	int FindPostScriptHint();

	bool FindFirstPacket();
	bool FindLastPacket();

};	// PostScript_MetaHandler

// =================================================================================================

#endif /* __PostScript_Handler_hpp__ */
