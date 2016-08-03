// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "HostAPIAccess.h"

namespace XMP_PLUGIN
{

///////////////////////////////////////////////////////////////////////////////
//
// API handling 
//
	
static HostAPIRef sHostAPI = NULL;

bool SetHostAPI( HostAPIRef hostAPI )
{
	bool retVal = false;
	if( hostAPI &&
		hostAPI->mFileIOAPI &&
		hostAPI->mStrAPI		)
	{
		sHostAPI = hostAPI;
		retVal = true;
	}
	return retVal;
}

// ============================================================================

static HostAPIRef GetHostAPI()
{
	return sHostAPI;
}

// ============================================================================

inline void CheckError( WXMP_Error & error )
{
	if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// class IOAdapter
//

XMP_Uns32 IOAdapter::Read( void* buffer, XMP_Uns32 count, bool readAll ) const
{
	WXMP_Error error;
	XMP_Uns32 result;
	GetHostAPI()->mFileIOAPI->mReadProc( this->mFileRef, buffer, count, readAll, result, &error );
	CheckError( error );
	return result;
}

// ============================================================================

void IOAdapter::Write( void* buffer, XMP_Uns32 count ) const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mWriteProc( this->mFileRef, buffer, count, &error );
	CheckError( error );
}

// ============================================================================

void IOAdapter::Seek( XMP_Int64& offset, SeekMode mode ) const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mSeekProc( this->mFileRef, offset, mode, &error );
	CheckError( error );
}

// ============================================================================

XMP_Int64 IOAdapter::Length() const
{
	WXMP_Error error;
	XMP_Int64 length = 0;
	GetHostAPI()->mFileIOAPI->mLengthProc( this->mFileRef, length, &error );
	CheckError( error );
	return length;
}

// ============================================================================

void IOAdapter::Truncate( XMP_Int64 length ) const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mTruncateProc( this->mFileRef, length, &error );
	CheckError( error );
}

// ============================================================================

XMP_IORef IOAdapter::DeriveTemp() const
{
	WXMP_Error error;
	XMP_IORef tempIO;
	GetHostAPI()->mFileIOAPI->mDeriveTempProc( this->mFileRef, tempIO, &error );
	CheckError( error );
	return tempIO;
}

// ============================================================================

void IOAdapter::AbsorbTemp() const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mAbsorbTempProc( this->mFileRef, &error );
	CheckError( error );
}

// ============================================================================

void IOAdapter::DeleteTemp() const
{
	WXMP_Error error;
	GetHostAPI()->mFileIOAPI->mDeleteTempProc( this->mFileRef, &error );
	CheckError( error );
}

///////////////////////////////////////////////////////////////////////////////
//
// Host Strings
//

StringPtr HostStringCreateBuffer( XMP_Uns32 size )
{
	WXMP_Error error;
	StringPtr buffer = NULL;
	GetHostAPI()->mStrAPI->mCreateBufferProc( &buffer, size, &error );
	CheckError( error );
	return buffer;
}

// ============================================================================

void HostStringReleaseBuffer( StringPtr buffer )
{
	WXMP_Error error;
	GetHostAPI()->mStrAPI->mReleaseBufferProc( buffer, &error );
	CheckError( error );
}

///////////////////////////////////////////////////////////////////////////////
//
// Abort functionality
//

bool CheckAbort( SessionRef session )
{
	WXMP_Error error;
	bool abort = false;
	GetHostAPI()->mAbortAPI->mCheckAbort( session, &abort, &error );

	if( error.mErrorID == kXMPErr_Unavailable )
	{
		abort = false;
	}
	else if( error.mErrorID != kXMPErr_NoError )
	{
		throw XMP_Error( error.mErrorID, error.mErrorMsg );
	}

	return abort;
}

///////////////////////////////////////////////////////////////////////////////
//
// Standard file handler access
//

} //namespace XMP_PLUGIN
