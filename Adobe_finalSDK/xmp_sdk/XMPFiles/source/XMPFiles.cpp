// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2004 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! Must be the first #include!
#include "public/include/XMP_Const.h"
#include "public/include/XMP_IO.hpp"

#include <vector>
#include <string.h>

#include "source/UnicodeConversions.hpp"
#include "source/XMPFiles_IO.hpp"
#include "source/XIO.hpp"

#include "XMPFiles/source/XMPFiles_Impl.hpp"
#include "XMPFiles/source/HandlerRegistry.h"
#include "XMPFiles/source/PluginHandler/PluginManager.h"

#include "XMPFiles/source/FormatSupport/ID3_Support.hpp"

#if EnablePacketScanning
	#include "XMPFiles/source/FileHandlers/Scanner_Handler.hpp"
#endif

// =================================================================================================
/// \file XMPFiles.cpp
/// \brief High level support to access metadata in files of interest to Adobe applications.
///
/// This header ...
///
// =================================================================================================

using namespace Common;
using namespace XMP_PLUGIN;

// =================================================================================================

XMP_Int32 sXMPFilesInitCount = 0;

#if GatherPerformanceData
	APIPerfCollection* sAPIPerf = 0;
#endif

// These are embedded version strings.

#if XMP_DebugBuild
	#define kXMPFiles_DebugFlag 1
#else
	#define kXMPFiles_DebugFlag 0
#endif

#define kXMPFiles_VersionNumber	( (kXMPFiles_DebugFlag << 31)    |	\
                                  (XMP_API_VERSION_MAJOR << 24) |	\
						          (XMP_API_VERSION_MINOR << 16) |	\
						          (XMP_API_VERSION_MICRO << 8) )

	#define kXMPFilesName "XMP Files"
	#define kXMPFiles_VersionMessage kXMPFilesName " " XMPFILES_API_VERSION_STRING
const char * kXMPFiles_EmbeddedVersion   = kXMPFiles_VersionMessage;
const char * kXMPFiles_EmbeddedCopyright = kXMPFilesName " " kXMP_CopyrightStr;

// =================================================================================================

#if EnablePacketScanning
	static XMPFileHandlerInfo kScannerHandlerInfo ( kXMP_UnknownFile, kScanner_HandlerFlags,
													(CheckFileFormatProc)0, Scanner_MetaHandlerCTor );
#endif

// =================================================================================================

/* class-static */
void
XMPFiles::GetVersionInfo ( XMP_VersionInfo * info )
{

	memset ( info, 0, sizeof(XMP_VersionInfo) );

	info->major   = XMPFILES_API_VERSION_MAJOR;
	info->minor   = XMPFILES_API_VERSION_MINOR;
	info->micro   = 0; //no longer used
	info->isDebug = kXMPFiles_DebugFlag;
	info->flags   = 0;	// ! None defined yet.
	info->message = kXMPFiles_VersionMessage;

}	// XMPFiles::GetVersionInfo

// =================================================================================================

#if XMP_TraceFilesCalls
	FILE * xmpFilesLog = stderr;
#endif

#if UseGlobalLibraryLock & (! XMP_StaticBuild )
	XMP_BasicMutex sLibraryLock;	// ! Handled in XMPMeta for static builds.
#endif

/* class static */
bool
XMPFiles::Initialize( XMP_OptionBits options, const char* pluginFolder, const char* plugins /* = NULL */ )
{
	++sXMPFilesInitCount;
	if ( sXMPFilesInitCount > 1 ) return true;

	#if XMP_TraceFilesCallsToFile
		xmpFilesLog = fopen ( "XMPFilesLog.txt", "w" );
		if ( xmpFilesLog == 0 ) xmpFilesLog = stderr;
	#endif

	#if UseGlobalLibraryLock & (! XMP_StaticBuild )
		InitializeBasicMutex ( sLibraryLock );	// ! Handled in XMPMeta for static builds.
	#endif

	SXMPMeta::Initialize();	// Just in case the client does not.

	if ( ! Initialize_LibUtils() ) return false;
	if ( ! ID3_Support::InitializeGlobals() ) return false;

	#if GatherPerformanceData
		sAPIPerf = new APIPerfCollection;
	#endif

	XMP_Uns16 endianInt  = 0x00FF;
	XMP_Uns8  endianByte = *((XMP_Uns8*)&endianInt);
	if ( kBigEndianHost ) {
		if ( endianByte != 0 ) XMP_Throw ( "Big endian flag mismatch", kXMPErr_InternalFailure );
	} else {
		if ( endianByte != 0xFF ) XMP_Throw ( "Little endian flag mismatch", kXMPErr_InternalFailure );
	}

	XMP_Assert ( kUTF8_PacketHeaderLen == strlen ( "<?xpacket begin='xxx' id='W5M0MpCehiHzreSzNTczkc9d'" ) );
	XMP_Assert ( kUTF8_PacketTrailerLen == strlen ( (const char *) kUTF8_PacketTrailer ) );

	HandlerRegistry::getInstance().initialize();

	InitializeUnicodeConversions();

	ignoreLocalText = XMP_OptionIsSet ( options, kXMPFiles_IgnoreLocalText );
	#if XMP_UNIXBuild
		if ( ! ignoreLocalText ) XMP_Throw ( "Generic UNIX clients must pass kXMPFiles_IgnoreLocalText", kXMPErr_EnforceFailure );
	#endif

	if ( pluginFolder != 0 ) {
		std::string pluginList;
		if ( plugins != 0 ) pluginList.assign ( plugins );
		PluginManager::initialize ( pluginFolder, pluginList );  // Load file handler plugins.
	}

	// Make sure the embedded info strings are referenced and kept.
	if ( (kXMPFiles_EmbeddedVersion[0] == 0) || (kXMPFiles_EmbeddedCopyright[0] == 0) ) return false;
	// Verify critical type sizes.
	XMP_Assert ( sizeof(XMP_Int8) == 1 );
	XMP_Assert ( sizeof(XMP_Int16) == 2 );
	XMP_Assert ( sizeof(XMP_Int32) == 4 );
	XMP_Assert ( sizeof(XMP_Int64) == 8 );
	XMP_Assert ( sizeof(XMP_Uns8) == 1 );
	XMP_Assert ( sizeof(XMP_Uns16) == 2 );
	XMP_Assert ( sizeof(XMP_Uns32) == 4 );
	XMP_Assert ( sizeof(XMP_Uns64) == 8 );

	return true;

}	// XMPFiles::Initialize

// =================================================================================================

#if GatherPerformanceData

#if XMP_WinBuild
	#pragma warning ( disable : 4996 )	// '...' was declared deprecated
#endif

#include "PerfUtils.cpp"

static void ReportPerformanceData()
{
	struct SummaryInfo {
		size_t callCount;
		double totalTime;
		SummaryInfo() : callCount(0), totalTime(0.0) {};
	};

	SummaryInfo perfSummary [kAPIPerfProcCount];

	XMP_DateTime now;
	SXMPUtils::CurrentDateTime ( &now );
	std::string nowStr;
	SXMPUtils::ConvertFromDate ( now, &nowStr );

	#if XMP_WinBuild
		#define kPerfLogPath "C:\\XMPFilesPerformanceLog.txt"
	#else
		#define kPerfLogPath "/XMPFilesPerformanceLog.txt"
	#endif
	FILE * perfLog = fopen ( kPerfLogPath, "ab" );
	if ( perfLog == 0 ) return;

	fprintf ( perfLog, "\n\n// =================================================================================================\n\n" );
	fprintf ( perfLog, "XMPFiles performance data\n" );
	fprintf ( perfLog, "   %s\n", kXMPFiles_VersionMessage );
	fprintf ( perfLog, "   Reported at %s\n", nowStr.c_str() );
	fprintf ( perfLog, "   %s\n", PerfUtils::GetTimerInfo() );

	// Gather and report the summary info.

	for ( size_t i = 0; i < sAPIPerf->size(); ++i ) {
		SummaryInfo& summaryItem = perfSummary [(*sAPIPerf)[i].whichProc];
		++summaryItem.callCount;
		summaryItem.totalTime += (*sAPIPerf)[i].elapsedTime;
	}

	fprintf ( perfLog, "\nSummary data:\n" );

	for ( size_t i = 0; i < kAPIPerfProcCount; ++i ) {
		long averageTime = 0;
		if ( perfSummary[i].callCount != 0 ) {
			averageTime = (long) (((perfSummary[i].totalTime/perfSummary[i].callCount) * 1000.0*1000.0) + 0.5);
		}
		fprintf ( perfLog, "   %s, %d total calls, %d average microseconds per call\n",
				  kAPIPerfNames[i], perfSummary[i].callCount, averageTime );
	}

	fprintf ( perfLog, "\nPer-call data:\n" );

	// Report the info for each call.

	for ( size_t i = 0; i < sAPIPerf->size(); ++i ) {
		long time = (long) (((*sAPIPerf)[i].elapsedTime * 1000.0*1000.0) + 0.5);
		fprintf ( perfLog, "   %s, %d microSec, ref %.8X, \"%s\"\n",
				  kAPIPerfNames[(*sAPIPerf)[i].whichProc], time,
				  (*sAPIPerf)[i].xmpFilesRef, (*sAPIPerf)[i].extraInfo.c_str() );
	}

	fclose ( perfLog );

}	// ReportAReportPerformanceDataPIPerformance

#endif

// =================================================================================================

/* class static */
void
XMPFiles::Terminate()
{
	--sXMPFilesInitCount;
	if ( sXMPFilesInitCount != 0 ) return;	// Not ready to terminate, or already terminated.

	#if GatherPerformanceData
		ReportPerformanceData();
		EliminateGlobal ( sAPIPerf );
	#endif

	PluginManager::terminate();
	HandlerRegistry::terminate();

	SXMPMeta::Terminate();	// Just in case the client does not.

	ID3_Support::TerminateGlobals();
	Terminate_LibUtils();

	#if UseGlobalLibraryLock & (! XMP_StaticBuild )
		TerminateBasicMutex ( sLibraryLock );	// ! Handled in XMPMeta for static builds.
	#endif

	#if XMP_TraceFilesCallsToFile
		if ( xmpFilesLog != stderr ) fclose ( xmpFilesLog );
		xmpFilesLog = stderr;
	#endif

}	// XMPFiles::Terminate

// =================================================================================================

XMPFiles::XMPFiles() :
	clientRefs(0),
	format(kXMP_UnknownFile),
	ioRef(0),
	openFlags(0),
	handler(0),
	tempPtr(0),
	tempUI32(0),
	abortProc(0),
	abortArg(0)
{
	// Nothing more to do, clientRefs is incremented in wrapper.

}	// XMPFiles::XMPFiles

// =================================================================================================

static inline void CloseLocalFile ( XMPFiles* thiz )
{
	if ( thiz->UsesLocalIO() ) {

		XMPFiles_IO* localFile = (XMPFiles_IO*)thiz->ioRef;

		if ( localFile != 0 ) {
			localFile->Close();
			delete localFile;
			thiz->ioRef = 0;
		}

	}

}	// CloseLocalFile

// =================================================================================================

XMPFiles::~XMPFiles()
{
	XMP_Assert ( this->clientRefs <= 0 );

	if ( this->handler != 0 ) {
		delete this->handler;
		this->handler = 0;
	}

	CloseLocalFile ( this );

	if ( this->tempPtr != 0 ) free ( this->tempPtr );	// ! Must have been malloc-ed!

}	// XMPFiles::~XMPFiles

// =================================================================================================

/* class static */
bool
XMPFiles::GetFormatInfo ( XMP_FileFormat   format,
                          XMP_OptionBits * flags /* = 0 */ )
{

	return HandlerRegistry::getInstance().getFormatInfo ( format, flags );

}	// XMPFiles::GetFormatInfo

// =================================================================================================

/* class static */
XMP_FileFormat
XMPFiles::CheckFileFormat ( XMP_StringPtr clientPath )
{
	if ( (clientPath == 0) || (*clientPath == 0) ) return kXMP_UnknownFile;

	XMPFiles bogus;	// Needed to provide context to SelectSmartHandler.
	bogus.filePath = clientPath;	// So that XMPFiles destructor cleans up the XMPFiles_IO object.
	XMPFileHandlerInfo * handlerInfo = HandlerRegistry::getInstance().selectSmartHandler( &bogus, clientPath, kXMP_UnknownFile, kXMPFiles_OpenForRead );

	if ( handlerInfo == 0 ) return kXMP_UnknownFile;
	return handlerInfo->format;

}	// XMPFiles::CheckFileFormat

// =================================================================================================

/* class static */
XMP_FileFormat
XMPFiles::CheckPackageFormat ( XMP_StringPtr folderPath )
{
	// This is called with a path to a folder, and checks to see if that folder is the top level of
	// a "package" that should be recognized by one of the folder-oriented handlers. The checks here
	// are not overly extensive, but hopefully enough to weed out false positives.
	//
	// Since we don't have many folder handlers, this is simple hardwired code.

	#if ! EnableDynamicMediaHandlers
		return kXMP_UnknownFile;
	#else
		Host_IO::FileMode folderMode = Host_IO::GetFileMode ( folderPath );
		if ( folderMode != Host_IO::kFMode_IsFolder ) return kXMP_UnknownFile;
		return HandlerRegistry::checkTopFolderName ( std::string ( folderPath ) );
	#endif

}	// XMPFiles::CheckPackageFormat

// =================================================================================================

static bool FileIsExcluded ( XMP_StringPtr clientPath, std::string * fileExt, Host_IO::FileMode * clientMode )
{
	// ! Return true for excluded files, false for OK files.
	
	*clientMode = Host_IO::GetFileMode ( clientPath );
	if ( (*clientMode == Host_IO::kFMode_IsFolder) || (*clientMode == Host_IO::kFMode_IsOther) ) return true;
	XMP_Assert ( (*clientMode == Host_IO::kFMode_IsFile) || (*clientMode == Host_IO::kFMode_DoesNotExist) );

	if ( *clientMode == Host_IO::kFMode_IsFile ) {

		// Find the file extension. OK to be "wrong" for something like "C:\My.dir\file". Any
		// filtering looks for matches with real extensions, "dir\file" won't match any of these.
		XMP_StringPtr extPos = clientPath + strlen ( clientPath );
		for ( ; (extPos != clientPath) && (*extPos != '.'); --extPos ) {}
		if ( *extPos == '.' ) {
			fileExt->assign ( extPos+1 );
			MakeLowerCase ( fileExt );
		}

		// See if this file is one that XMPFiles should never process.
		for ( size_t i = 0; kKnownRejectedFiles[i] != 0; ++i ) {
			if ( *fileExt == kKnownRejectedFiles[i] ) return true;
		}

	}
	
	return false;

}	// FileIsExcluded

// =================================================================================================

/* class static */
bool
XMPFiles::GetFileModDate ( XMP_StringPtr    clientPath,
						   XMP_DateTime *   modDate,
						   XMP_FileFormat * format, /* = 0 */
						   XMP_OptionBits   options /* = 0 */ )
{
	
	// ---------------------------------------------------------------
	// First try to select a smart handler. Return false if not found.
	
	XMPFiles dummyParent;	// GetFileModDate is static, but the handler needs a parent.
	dummyParent.filePath = clientPath;

	Host_IO::FileMode clientMode;
	std::string fileExt;	// Used to check for excluded files.
	bool excluded = FileIsExcluded ( clientPath, &fileExt, &clientMode );	// ! Fills in fileExt and clientMode.
	if ( excluded ) return false;

	XMPFileHandlerInfo * handlerInfo  = 0;
	XMPFileHandlerCTor   handlerCTor  = 0;
	XMP_OptionBits       handlerFlags = 0;
	
	XMP_FileFormat dummyFormat = kXMP_UnknownFile;
	if ( format == 0 ) format = &dummyFormat;

	options |= kXMPFiles_OpenForRead;
	handlerInfo = HandlerRegistry::getInstance().selectSmartHandler ( &dummyParent, clientPath, *format, options );
	if ( handlerInfo == 0 ) return false;
	
	// -------------------------------------------------------------------------
	// Fill in the format output. Call the handler to get the modification date.
	
	dummyParent.format = handlerInfo->format;
	*format = handlerInfo->format;
	
	dummyParent.handler = handlerInfo->handlerCTor ( &dummyParent );
	
	bool ok = dummyParent.handler->GetFileModDate ( modDate );
	
	delete dummyParent.handler;
	dummyParent.handler = 0;
	
	return ok;
	
}	// XMPFiles::GetFileModDate

// =================================================================================================

static bool
DoOpenFile ( XMPFiles *     thiz,
			 XMP_IO *       clientIO,
			 XMP_StringPtr  clientPath,
	         XMP_FileFormat format /* = kXMP_UnknownFile */,
	         XMP_OptionBits openFlags /* = 0 */ )
{
	XMP_Assert ( (clientIO == 0) ? (clientPath[0] != 0) : (clientPath[0] == 0) );
	
	openFlags &= ~kXMPFiles_ForceGivenHandler;	// Don't allow this flag for OpenFile.

	if ( thiz->handler != 0 ) XMP_Throw ( "File already open", kXMPErr_BadParam );
	CloseLocalFile ( thiz );	// Sanity checks if prior call failed.

	thiz->ioRef = clientIO;
	thiz->filePath = clientPath;

	thiz->format = kXMP_UnknownFile;	// Make sure it is preset for later check.
	thiz->openFlags = openFlags;

	bool readOnly = XMP_OptionIsClear ( openFlags, kXMPFiles_OpenForUpdate );

	Host_IO::FileMode clientMode;
	std::string fileExt;	// Used to check for camera raw files and OK to scan files.

	if ( thiz->UsesClientIO() ) {
		clientMode = Host_IO::kFMode_IsFile;
	} else {
		bool excluded = FileIsExcluded ( clientPath, &fileExt, &clientMode );	// ! Fills in fileExt and clientMode.
		if ( excluded ) return false;
	}

	// Find the handler, fill in the XMPFiles member variables, cache the desired file data.

	XMPFileHandlerInfo * handlerInfo  = 0;
	XMPFileHandlerCTor   handlerCTor  = 0;
	XMP_OptionBits       handlerFlags = 0;

	if ( ! (openFlags & kXMPFiles_OpenUsePacketScanning) ) {
		handlerInfo = HandlerRegistry::getInstance().selectSmartHandler( thiz, clientPath, format, openFlags );
	}

	#if ! EnablePacketScanning

		if ( handlerInfo == 0 ) return false;

	#else

		if ( handlerInfo == 0 ) {

			// No smart handler, packet scan if appropriate.

			if ( clientMode != Host_IO::kFMode_IsFile ) return false;
			if ( openFlags & kXMPFiles_OpenUseSmartHandler ) return false;

			if ( openFlags & kXMPFiles_OpenLimitedScanning ) {
				bool scanningOK = false;
				for ( size_t i = 0; kKnownScannedFiles[i] != 0; ++i ) {
					if ( fileExt == kKnownScannedFiles[i] ) { scanningOK = true; break; }
				}
				if ( ! scanningOK ) return false;
			}

			handlerInfo = &kScannerHandlerInfo;
			if ( thiz->ioRef == 0 ) {	// Normally opened in SelectSmartHandler, but might not be open yet.
				thiz->ioRef = XMPFiles_IO::New_XMPFiles_IO ( clientPath, readOnly );
				if ( thiz->ioRef == 0 ) return false;
			}

		}

	#endif

	XMP_Assert ( handlerInfo != 0 );
	handlerCTor  = handlerInfo->handlerCTor;
	handlerFlags = handlerInfo->flags;

	XMP_Assert ( (thiz->ioRef != 0) ||
				 (handlerFlags & kXMPFiles_UsesSidecarXMP) ||
				 (handlerFlags & kXMPFiles_HandlerOwnsFile) ||
				 (handlerFlags & kXMPFiles_FolderBasedFormat) );

	if ( thiz->format == kXMP_UnknownFile ) thiz->format = handlerInfo->format;	// ! The CheckProc might have set it.
	XMPFileHandler* handler = (*handlerCTor) ( thiz );
	XMP_Assert ( handlerFlags == handler->handlerFlags );

	thiz->handler = handler;

	try {
		handler->CacheFileData();
	} catch ( ... ) {
		delete thiz->handler;
		thiz->handler = 0;
		if ( ! (handlerFlags & kXMPFiles_HandlerOwnsFile) ) CloseLocalFile ( thiz );
		throw;
	}

	if ( handler->containsXMP ) FillPacketInfo ( handler->xmpPacket, &handler->packetInfo );

	if ( (! (openFlags & kXMPFiles_OpenForUpdate)) && (! (handlerFlags & kXMPFiles_HandlerOwnsFile)) ) {
		// Close the disk file now if opened for read-only access.
		CloseLocalFile ( thiz );
	}

	return true;

}	// DoOpenFile

// =================================================================================================

static bool DoOpenFile( XMPFiles* thiz, 
						const Common::XMPFileHandlerInfo& hdlInfo, 
						XMP_IO* clientIO, 
						XMP_StringPtr clientPath, 
						XMP_OptionBits openFlags )
{
	XMP_Assert ( (clientIO == 0) ? (clientPath[0] != 0) : (clientPath[0] == 0) );
	
	openFlags &= ~kXMPFiles_ForceGivenHandler;	// Don't allow this flag for OpenFile.


	if ( thiz->handler != 0 ) XMP_Throw ( "File already open", kXMPErr_BadParam );

	//
	// setup members
	//
	thiz->ioRef		= clientIO;
	thiz->filePath	= std::string( clientPath );
	thiz->format	= hdlInfo.format;
	thiz->openFlags = openFlags;

	//
	// create file handler instance
	//
	XMPFileHandlerCTor handlerCTor	= hdlInfo.handlerCTor;
	XMP_OptionBits handlerFlags		= hdlInfo.flags;

	XMPFileHandler* handler			= (*handlerCTor) ( thiz );
	XMP_Assert ( handlerFlags == handler->handlerFlags );

	thiz->handler = handler;

	//
	// try to read metadata
	//
	try 
	{
		handler->CacheFileData();

		if( handler->containsXMP ) 
		{
			FillPacketInfo( handler->xmpPacket, &handler->packetInfo );
		}

		if( (! (openFlags & kXMPFiles_OpenForUpdate)) && (! (handlerFlags & kXMPFiles_HandlerOwnsFile)) ) 
		{
			// Close the disk file now if opened for read-only access.
			CloseLocalFile ( thiz );
		}
	} 
	catch( ... ) 
	{
		delete thiz->handler;
		thiz->handler = NULL;

		if( ! (handlerFlags & kXMPFiles_HandlerOwnsFile) ) 
		{
			CloseLocalFile( thiz );
		}

		throw;
	}

	return true;
}

// =================================================================================================

bool
XMPFiles::OpenFile ( XMP_StringPtr  clientPath,
	                 XMP_FileFormat format /* = kXMP_UnknownFile */,
	                 XMP_OptionBits openFlags /* = 0 */ )
{

	return DoOpenFile ( this, 0, clientPath, format, openFlags );

}

// =================================================================================================

#if XMP_StaticBuild	// ! Client XMP_IO objects can only be used in static builds.

bool
XMPFiles::OpenFile ( XMP_IO*        clientIO,
	                 XMP_FileFormat format /* = kXMP_UnknownFile */,
	                 XMP_OptionBits openFlags /* = 0 */ )
{

	return DoOpenFile ( this, clientIO, "", format, openFlags );

}	// XMPFiles::OpenFile

#endif	// XMP_StaticBuild

// =================================================================================================

#if XMP_StaticBuild	// ! Client XMP_IO objects can only be used in static builds.

bool XMPFiles::OpenFile( const Common::XMPFileHandlerInfo&	hdlInfo,
						 XMP_IO*							clientIO,
						 XMP_OptionBits						openFlags /*= 0*/ )
{

	return DoOpenFile( this, hdlInfo, clientIO, NULL, openFlags );
}

#endif	// XMP_StaticBuild

// =================================================================================================

bool XMPFiles::OpenFile( const Common::XMPFileHandlerInfo&	hdlInfo,
						 XMP_StringPtr						filePath,
						 XMP_OptionBits						openFlags /*= 0*/ )
{

	return DoOpenFile( this, hdlInfo, NULL, filePath, openFlags );
}

// =================================================================================================

void
XMPFiles::CloseFile ( XMP_OptionBits closeFlags /* = 0 */ )
{
	if ( this->handler == 0 ) return;	// Return if there is no open file (not an error).

	bool needsUpdate = this->handler->needsUpdate;
	XMP_OptionBits handlerFlags = this->handler->handlerFlags;

	// Decide if we're doing a safe update. If so, make sure the handler supports it. All handlers
	// that don't own the file tolerate safe update using common code below.

	bool doSafeUpdate = XMP_OptionIsSet ( closeFlags, kXMPFiles_UpdateSafely );
	#if GatherPerformanceData
		if ( doSafeUpdate ) sAPIPerf->back().extraInfo += ", safe update";	// Client wants safe update.
	#endif

	if ( ! (this->openFlags & kXMPFiles_OpenForUpdate) ) doSafeUpdate = false;
	if ( ! needsUpdate ) doSafeUpdate = false;

	bool safeUpdateOK = ( (handlerFlags & kXMPFiles_AllowsSafeUpdate) ||
						  (! (handlerFlags & kXMPFiles_HandlerOwnsFile)) );
	if ( doSafeUpdate && (! safeUpdateOK) ) {
		XMP_Throw ( "XMPFiles::CloseFile - Safe update not supported", kXMPErr_Unavailable );
	}

	// Try really hard to make sure the file is closed and the handler is deleted.

	try {

		if ( (! doSafeUpdate) || (handlerFlags & kXMPFiles_HandlerOwnsFile) ) {	// ! Includes no update case.

			// Close the file without doing common crash-safe writing. The handler might do it.

			if ( needsUpdate ) {
				#if GatherPerformanceData
					sAPIPerf->back().extraInfo += ", direct update";
				#endif
				this->handler->UpdateFile ( doSafeUpdate );
			}

			delete this->handler;
			this->handler = 0;
			CloseLocalFile ( this );

		} else {

			// Update the file in a crash-safe manner using common control of a temp file.

			XMP_IO* tempFileRef = this->ioRef->DeriveTemp();
			if ( tempFileRef == 0 ) XMP_Throw ( "XMPFiles::CloseFile, cannot create temp", kXMPErr_InternalFailure );

			if ( handlerFlags & kXMPFiles_CanRewrite ) {

				// The handler can rewrite an entire file based on the original.

				#if GatherPerformanceData
					sAPIPerf->back().extraInfo += ", file rewrite";
				#endif

				this->handler->WriteTempFile ( tempFileRef );

			} else {

				// The handler can only update an existing file. Copy to the temp then update.


				#if GatherPerformanceData
					sAPIPerf->back().extraInfo += ", copy update";
				#endif


				XMP_IO* origFileRef = this->ioRef;

				origFileRef->Rewind();
				XIO::Copy ( origFileRef, tempFileRef, origFileRef->Length(), abortProc, abortArg );

				try {

					this->ioRef = tempFileRef;
					this->handler->UpdateFile ( false );	// We're doing the safe update, not the handler.
					this->ioRef = origFileRef;

				} catch ( ... ) {

					this->ioRef->DeleteTemp();
					this->ioRef = origFileRef;
					throw;

				}

			}

			this->ioRef->AbsorbTemp();
			CloseLocalFile ( this );

			delete this->handler;
			this->handler = 0;

		}

	} catch ( ... ) {

		// *** Don't delete the temp or copy files, not sure which is best.

		try {
			if ( this->handler != 0 ) delete this->handler;
		} catch ( ... ) { /* Do nothing, throw the outer exception later. */ }

		if( this->ioRef ) this->ioRef->DeleteTemp();
		CloseLocalFile ( this );
		this->filePath.clear();

		this->handler   = 0;
		this->format    = kXMP_UnknownFile;
		this->ioRef     = 0;
		this->openFlags = 0;

		if ( this->tempPtr != 0 ) free ( this->tempPtr );	// ! Must have been malloc-ed!
		this->tempPtr  = 0;
		this->tempUI32 = 0;

		throw;

	}

	// Clear the XMPFiles member variables.

	CloseLocalFile ( this );
	this->filePath.clear();

	this->handler   = 0;
	this->format    = kXMP_UnknownFile;
	this->ioRef     = 0;
	this->openFlags = 0;

	if ( this->tempPtr != 0 ) free ( this->tempPtr );	// ! Must have been malloc-ed!
	this->tempPtr  = 0;
	this->tempUI32 = 0;

}	// XMPFiles::CloseFile

// =================================================================================================

bool
XMPFiles::GetFileInfo ( XMP_StringPtr *  filePath /* = 0 */,
                        XMP_StringLen *  pathLen /* = 0 */,
	                    XMP_OptionBits * openFlags /* = 0 */,
	                    XMP_FileFormat * format /* = 0 */,
	                    XMP_OptionBits * handlerFlags /* = 0 */ ) const
{
	if ( this->handler == 0 ) return false;
	XMPFileHandler * handler = this->handler;

	if ( filePath == 0 ) filePath = &voidStringPtr;
	if ( pathLen == 0 ) pathLen = &voidStringLen;
	if ( openFlags == 0 ) openFlags = &voidOptionBits;
	if ( format == 0 ) format = &voidFileFormat;
	if ( handlerFlags == 0 ) handlerFlags = &voidOptionBits;

	*filePath     = this->filePath.c_str();
	*pathLen      = (XMP_StringLen) this->filePath.size();
	*openFlags    = this->openFlags;
	*format       = this->format;
	*handlerFlags = this->handler->handlerFlags;

	return true;

}	// XMPFiles::GetFileInfo

// =================================================================================================

void
XMPFiles::SetAbortProc ( XMP_AbortProc abortProc,
						 void *        abortArg )
{
	this->abortProc = abortProc;
	this->abortArg  = abortArg;

	XMP_Assert ( (abortProc != (XMP_AbortProc)0) || (abortArg != (void*)(unsigned long long)0xDEADBEEFULL) );	// Hack to test the assert callback.
}	// XMPFiles::SetAbortProc

// =================================================================================================
// SetClientPacketInfo
// ===================
//
// Set the packet info returned to the client. This is the internal packet info at first, which
// tells what is in the file. But once the file needs update (PutXMP has been called), we return
// info about the latest XMP. The internal packet info is left unchanged since it is needed when
// the file is updated to locate the old packet in the file.

static void
SetClientPacketInfo ( XMP_PacketInfo * clientInfo, const XMP_PacketInfo & handlerInfo,
					  const std::string & xmpPacket, bool needsUpdate )
{

	if ( clientInfo == 0 ) return;

	if ( ! needsUpdate ) {
		*clientInfo = handlerInfo;
	} else {
		clientInfo->offset = kXMPFiles_UnknownOffset;
		clientInfo->length = (XMP_Int32) xmpPacket.size();
		FillPacketInfo ( xmpPacket, clientInfo );
	}

}	// SetClientPacketInfo

// =================================================================================================

bool
XMPFiles::GetXMP ( SXMPMeta *       xmpObj /* = 0 */,
                   XMP_StringPtr *  xmpPacket /* = 0 */,
                   XMP_StringLen *  xmpPacketLen /* = 0 */,
                   XMP_PacketInfo * packetInfo /* = 0 */ )
{
	if ( this->handler == 0 ) XMP_Throw ( "XMPFiles::GetXMP - No open file", kXMPErr_BadObject );

	XMP_OptionBits applyTemplateFlags = kXMPTemplate_AddNewProperties | kXMPTemplate_IncludeInternalProperties;

	if ( ! this->handler->processedXMP ) {
		try {
			this->handler->ProcessXMP();
		} catch ( ... ) {
			// Return the outputs then rethrow the exception.
			if ( xmpObj != 0 ) {
				// ! Don't use Clone, that replaces the internal ref in the local xmpObj, leaving the client unchanged!
				xmpObj->Erase();
				SXMPUtils::ApplyTemplate ( xmpObj, this->handler->xmpObj, applyTemplateFlags );
			}
			if ( xmpPacket != 0 ) *xmpPacket = this->handler->xmpPacket.c_str();
			if ( xmpPacketLen != 0 ) *xmpPacketLen = (XMP_StringLen) this->handler->xmpPacket.size();
			SetClientPacketInfo ( packetInfo, this->handler->packetInfo,
								  this->handler->xmpPacket, this->handler->needsUpdate );
			throw;
		}
	}

	if ( ! this->handler->containsXMP ) return false;

	#if 0	// *** See bug 1131815. A better way might be to pass the ref up from here.
		if ( xmpObj != 0 ) *xmpObj = this->handler->xmpObj.Clone();
	#else
		if ( xmpObj != 0 ) {
			// ! Don't use Clone, that replaces the internal ref in the local xmpObj, leaving the client unchanged!
			xmpObj->Erase();
			SXMPUtils::ApplyTemplate ( xmpObj, this->handler->xmpObj, applyTemplateFlags );
		}
	#endif

	if ( xmpPacket != 0 ) *xmpPacket = this->handler->xmpPacket.c_str();
	if ( xmpPacketLen != 0 ) *xmpPacketLen = (XMP_StringLen) this->handler->xmpPacket.size();
	SetClientPacketInfo ( packetInfo, this->handler->packetInfo,
						  this->handler->xmpPacket, this->handler->needsUpdate );

	return true;

}	// XMPFiles::GetXMP

// =================================================================================================

static bool
DoPutXMP ( XMPFiles * thiz, const SXMPMeta & xmpObj, const bool doIt )
{
	// Check some basic conditions to see if the Put should be attempted.

	if ( thiz->handler == 0 ) XMP_Throw ( "XMPFiles::PutXMP - No open file", kXMPErr_BadObject );
	if ( ! (thiz->openFlags & kXMPFiles_OpenForUpdate) ) {
		XMP_Throw ( "XMPFiles::PutXMP - Not open for update", kXMPErr_BadObject );
	}

	XMPFileHandler * handler      = thiz->handler;
	XMP_OptionBits   handlerFlags = handler->handlerFlags;
	XMP_PacketInfo & packetInfo   = handler->packetInfo;
	std::string &    xmpPacket    = handler->xmpPacket;

	if ( ! handler->processedXMP ) handler->ProcessXMP();	// Might have Open/Put with no GetXMP.

	size_t oldPacketOffset = (size_t)packetInfo.offset;
	size_t oldPacketLength = packetInfo.length;

	if ( oldPacketOffset == (size_t)kXMPFiles_UnknownOffset ) oldPacketOffset = 0;	// ! Simplify checks.
	if ( oldPacketLength == (size_t)kXMPFiles_UnknownLength ) oldPacketLength = 0;

	bool fileHasPacket = (oldPacketOffset != 0) && (oldPacketLength != 0);

	if ( ! fileHasPacket ) {
		if ( ! (handlerFlags & kXMPFiles_CanInjectXMP) ) {
			XMP_Throw ( "XMPFiles::PutXMP - Can't inject XMP", kXMPErr_Unavailable );
		}
		if ( handler->stdCharForm == kXMP_CharUnknown ) {
			XMP_Throw ( "XMPFiles::PutXMP - No standard character form", kXMPErr_InternalFailure );
		}
	}

	// Serialize the XMP and update the handler's info.

	XMP_Uns8 charForm = handler->stdCharForm;
	if ( charForm == kXMP_CharUnknown ) charForm = packetInfo.charForm;

	XMP_OptionBits options = handler->GetSerializeOptions() | XMP_CharToSerializeForm ( charForm );
	if ( handlerFlags & kXMPFiles_NeedsReadOnlyPacket ) options |= kXMP_ReadOnlyPacket;
	if ( fileHasPacket && (thiz->format == kXMP_UnknownFile) && (! packetInfo.writeable) ) options |= kXMP_ReadOnlyPacket;

	bool preferInPlace = ((handlerFlags & kXMPFiles_PrefersInPlace) != 0);
	bool tryInPlace    = (fileHasPacket & preferInPlace) || (! (handlerFlags & kXMPFiles_CanExpand));

	if ( handlerFlags & kXMPFiles_UsesSidecarXMP ) tryInPlace = false;

	if ( tryInPlace ) {
		try {
			xmpObj.SerializeToBuffer ( &xmpPacket, (options | kXMP_ExactPacketLength), (XMP_StringLen) oldPacketLength );
			XMP_Assert ( xmpPacket.size() == oldPacketLength );
		} catch ( ... ) {
			if ( preferInPlace ) {
				tryInPlace = false;	// ! Try again, out of place this time.
			} else {
				if ( ! doIt ) return false;
				throw;
			}
		}
	}

	if ( ! tryInPlace ) {
		try {
			xmpObj.SerializeToBuffer ( &xmpPacket, options );
		} catch ( ... ) {
			if ( ! doIt ) return false;
			throw;
		}
	}

	if ( doIt ) {
		handler->xmpObj = xmpObj.Clone();
		handler->containsXMP = true;
		handler->processedXMP = true;
		handler->needsUpdate = true;
	}

	return true;

}	// DoPutXMP

// =================================================================================================

void
XMPFiles::PutXMP ( const SXMPMeta & xmpObj )
{
	(void) DoPutXMP ( this, xmpObj, true );

}	// XMPFiles::PutXMP

// =================================================================================================

void
XMPFiles::PutXMP ( XMP_StringPtr xmpPacket,
                   XMP_StringLen xmpPacketLen /* = kXMP_UseNullTermination */ )
{
	SXMPMeta xmpObj ( xmpPacket, xmpPacketLen );
	this->PutXMP ( xmpObj );

}	// XMPFiles::PutXMP

// =================================================================================================

bool
XMPFiles::CanPutXMP ( const SXMPMeta & xmpObj )
{
	if ( this->handler == 0 ) XMP_Throw ( "XMPFiles::CanPutXMP - No open file", kXMPErr_BadObject );

	if ( ! (this->openFlags & kXMPFiles_OpenForUpdate) ) return false;

	if ( this->handler->handlerFlags & kXMPFiles_CanInjectXMP ) return true;
	if ( ! this->handler->containsXMP ) return false;
	if ( this->handler->handlerFlags & kXMPFiles_CanExpand ) return true;

	return DoPutXMP ( this, xmpObj, false );

}	// XMPFiles::CanPutXMP

// =================================================================================================

bool
XMPFiles::CanPutXMP ( XMP_StringPtr xmpPacket,
                      XMP_StringLen xmpPacketLen /* = kXMP_UseNullTermination */ )
{
	SXMPMeta xmpObj ( xmpPacket, xmpPacketLen );
	return this->CanPutXMP ( xmpObj );

}	// XMPFiles::CanPutXMP

// =================================================================================================
