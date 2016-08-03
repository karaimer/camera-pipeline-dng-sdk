// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "PluginBase.h"
#include "XMP.incl_cpp"

#if XMP_WinBuild
#include <Windows.h>
#endif

#if ( XMP_MacBuild || XMP_UNIXBuild )
#include <sys/stat.h>
#endif

namespace XMP_PLUGIN
{
static bool GetModifyDate ( const char* filePath, XMP_DateTime* modifyDate );
	
void PluginBase::cacheFileData( XMP_IORef fileRef, XMP_StringPtr* xmpStr )
{
	std::string buffer;
	IOAdapter file( fileRef );
	
	this->cacheFileData( file, buffer );

	if( !buffer.empty() )
	{
		XMP_Uns32 length = (XMP_Uns32)buffer.size() + 1 ;
		StringPtr bufferPtr = HostStringCreateBuffer( length );
		memcpy( bufferPtr, buffer.c_str(), length );
		*xmpStr = bufferPtr; // callee function should free the memory.
	}
}

// ============================================================================

void PluginBase::updateFile( XMP_IORef fileRef, bool doSafeUpdate, XMP_StringPtr xmpStr )
{
	IOAdapter file( fileRef );
	std::string buffer( xmpStr );

	this->updateFile( file, doSafeUpdate, buffer );
}

// ============================================================================

void PluginBase::writeTempFile( XMP_IORef srcFileRef, XMP_IORef tmpFileRef, XMP_StringPtr xmpStr )
{
	IOAdapter srcFile( srcFileRef );
	IOAdapter tmpFile( tmpFileRef );
	std::string buffer( xmpStr );

	this->writeTempFile( srcFile, tmpFile, buffer );
}

// ============================================================================
// ============================================================================
// ============================================================================

bool PluginBase::getFileModDate ( XMP_DateTime * modDate )
{
	XMP_OptionBits flags = this->getHandlerFlags();
	const std::string & filePath = this->getPath();
	
	if ( (flags & kXMPFiles_HandlerOwnsFile)   ||
		 (flags & kXMPFiles_UsesSidecarXMP)    ||
		 (flags & kXMPFiles_FolderBasedFormat) ||
		 filePath.empty() )
	{
		return false;
	}

	return GetModifyDate ( filePath.c_str(), modDate );
}

// ============================================================================

bool PluginBase::checkAbort( bool doAbort /*= false*/)
{
	bool abort = CheckAbort( this );

	if( abort && doAbort )
	{
		throw XMP_Error( kXMPErr_UserAbort, "User abort" );
	}

	return abort;
}

// ============================================================================
// ============================================================================
// ============================================================================

#if XMP_WinBuild
	
static bool GetModifyDate ( const char* filePath, XMP_DateTime* modifyDate )
{
	bool ret = false;
	
	//
	// open file
	//
	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	DWORD access = GENERIC_READ;	// read only
	DWORD share  = FILE_SHARE_READ;

	std::string wideName;
	const size_t utf8Len = strlen(filePath);
	const size_t maxLen = 2 * (utf8Len+1);
	
	wideName.reserve ( maxLen );
	wideName.assign ( maxLen, ' ' );
	
	if( MultiByteToWideChar ( CP_UTF8, 0, filePath, -1, (LPWSTR)wideName.data(), (int)maxLen ) != 0 )
	{
		if ( GetFileAttributes ( (LPCWSTR)wideName.data() ) != INVALID_FILE_ATTRIBUTES )
		{
			fileHandle = CreateFileW ( (LPCWSTR)wideName.data(), access, share, 0, OPEN_EXISTING,
											  (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS), 0 );
		}
	}

	//
	// get the file modification time
	//
	if( fileHandle != INVALID_HANDLE_VALUE )
	{
		FILETIME binTime;
		
		if ( GetFileTime ( fileHandle, 0, 0, &binTime ) )
		{
			SYSTEMTIME utcTime;
			
			if ( FileTimeToSystemTime ( &binTime, &utcTime ) )
			{
				if ( modifyDate != NULL )
				{
					// Ignore the fractional seconds for consistency with UNIX and to avoid false newness even on
					// Windows. Some other sources of time only resolve to seconds, we don't want 25.3 looking
					// newer than 25.
					
					modifyDate->year		= utcTime.wYear;
					modifyDate->month		= utcTime.wMonth;
					modifyDate->day			= utcTime.wDay;
					modifyDate->hasDate		= true;
					
					modifyDate->hour		= utcTime.wHour;
					modifyDate->minute		= utcTime.wMinute;
					modifyDate->second		= utcTime.wSecond;
					modifyDate->nanoSecond	= 0;	// See note above; winTime.wMilliseconds * 1000*1000;
					modifyDate->hasTime		= true;
					
					modifyDate->tzSign		= kXMP_TimeIsUTC;
					modifyDate->tzHour		= 0;
					modifyDate->tzMinute	= 0;
					modifyDate->hasTimeZone	= true;
				}
				
				ret = true;
			}
		}

		CloseHandle ( fileHandle );
	}
	
	return ret;
}

#endif
	
#if ( XMP_MacBuild || XMP_UNIXBuild )

static bool GetModifyDate ( const char* filePath, XMP_DateTime* modifyDate )
{
	bool ret = false;
	struct stat info;
	
	if ( stat ( filePath, &info ) == 0 )
	{
		if ( S_ISREG(info.st_mode) || S_ISDIR(info.st_mode) )
		{
			ret = true;
			
			if( modifyDate != NULL )
			{
				struct tm posixUTC;
				gmtime_r ( &(info.st_mtime), &posixUTC );
				
				modifyDate->year		= posixUTC.tm_year + 1900;
				modifyDate->month		= posixUTC.tm_mon + 1;
				modifyDate->day			= posixUTC.tm_mday;
				modifyDate->hasDate		= true;
				
				modifyDate->hour		= posixUTC.tm_hour;
				modifyDate->minute		= posixUTC.tm_min;
				modifyDate->second		= posixUTC.tm_sec;
				modifyDate->nanoSecond	= 0;	// The time_t resolution is only to seconds.
				modifyDate->hasTime		= true;
				
				modifyDate->tzSign		= kXMP_TimeIsUTC;
				modifyDate->tzHour		= 0;
				modifyDate->tzMinute	= 0;
				modifyDate->hasTimeZone = true;
			}
		}
	}
	
	return ret;
}

#endif	

} //namespace XMP_PLUGIN
