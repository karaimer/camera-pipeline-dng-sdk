// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "PluginBase.h"
#include "PluginRegistry.h"

namespace XMP_PLUGIN
{

#define PLUGIN_VERSION	1 
#define XMP_MIN(a, b) (((a) < (b)) ? (a) : (b))

///////////////////////////////////////////////////////////////////////////////
//
//		Exception handler
//

static void HandleException( WXMP_Error* wError ) 
{
	try
	{
		throw;
	}
	catch(  XMP_Error& xmpErr )
	{
		wError->mErrorMsg = xmpErr.GetErrMsg();
		wError->mErrorID = xmpErr.GetID();
	}
	catch( std::exception& stdErr )
	{
		wError->mErrorMsg = stdErr.what();
	}
	catch( ... )
	{
		wError->mErrorMsg = "Caught unknown exception";
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//		Plugin API
//

static XMPErrorID Static_TerminatePlugin( WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginTerminate;

	try 
	{
		PluginRegistry::terminate();
		wError->mErrorID = kXMPErr_NoError;
	}
	catch( ... )
	{
		HandleException( wError );
	}
	
	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_SetHostAPI( HostAPIRef hostAPI, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_SetHostAPI;

	if( hostAPI && hostAPI->mVersion == PLUGIN_VERSION )
	{
		SetHostAPI( hostAPI );
		wError->mErrorID = kXMPErr_NoError;
	}
	
	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_InitializeSession( XMP_StringPtr uid, XMP_StringPtr filePath, XMP_Uns32 format, XMP_Uns32 handlerFlags, XMP_Uns32 openFlags, SessionRef * session, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginSessionInit;

	try
	{
		*session = PluginRegistry::create(uid, filePath, openFlags, format, handlerFlags );

		if( *session != NULL )
		{
			wError->mErrorID = kXMPErr_NoError;
		}
	}
	catch( ... )
	{
		HandleException( wError );
	}
	
	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_TerminateSession( SessionRef session, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginSessionTerm;
	PluginBase* thiz = (PluginBase*) session;
	
	try 
	{
		delete thiz;
		wError->mErrorID = kXMPErr_NoError;
	}
	catch( ... )
	{
		HandleException( wError );
	}

	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_CheckFileFormat( XMP_StringPtr uid, XMP_StringPtr filePath, XMP_IORef fileRef, bool * value, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginCheckFileFormat;

	try 
	{
		IOAdapter file( fileRef );
		*value = PluginRegistry::checkFileFormat( uid, filePath, file );
		wError->mErrorID = kXMPErr_NoError;
	}
	catch( ... )
	{
		HandleException( wError );
	}

	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_CheckFolderFormat( XMP_StringPtr uid, XMP_StringPtr rootPath, XMP_StringPtr gpName, XMP_StringPtr parentName, XMP_StringPtr leafName, bool * value, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginCheckFolderFormat;
	
	try
	{
		*value = PluginRegistry::checkFolderFormat( uid, rootPath, gpName, parentName, leafName );
		wError->mErrorID = kXMPErr_NoError;
	}
	catch( ... )
	{
		HandleException( wError );
	}

	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_GetFileModDate ( SessionRef session, bool * ok, XMP_DateTime * modDate, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginGetFileModDate;
	PluginBase* thiz = (PluginBase*) session;
	if ( (thiz == 0) || (ok == 0) || (modDate == 0) ) return kXMPErr_PluginGetFileModDate;

	try 	
	{
		*ok = thiz->getFileModDate ( modDate );
		wError->mErrorID = kXMPErr_NoError;
	} 
	catch ( ... ) 
	{
		HandleException( wError );
	}

	return wError->mErrorID;
}

static XMPErrorID Static_CacheFileData( SessionRef session, XMP_IORef fileRef, XMP_StringPtr* xmpStr, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginCacheFileData;
	
	PluginBase* thiz = (PluginBase*) session;
	try
	{
		if(thiz)
		{
			thiz->cacheFileData( fileRef, xmpStr );
			wError->mErrorID = kXMPErr_NoError;
		}
	}
	catch( ... )
	{
		HandleException( wError );
	}

	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_UpdateFile( SessionRef session, XMP_IORef fileRef, bool doSafeUpdate, XMP_StringPtr xmpStr, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginUpdateFile;
	
	PluginBase* thiz = (PluginBase*) session;
	try
	{
		if(thiz)
		{
			thiz->updateFile( fileRef, doSafeUpdate, xmpStr );
			wError->mErrorID = kXMPErr_NoError;
		}
	}
	catch( ... )
	{
		HandleException( wError );
	}

	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_WriteTempFile( SessionRef session, XMP_IORef srcFileRef, XMP_IORef fileRef, XMP_StringPtr xmpStr, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginWriteTempFile;
	
	PluginBase* thiz = (PluginBase*) session;
	try
	{
		if(thiz)
		{
			thiz->writeTempFile( srcFileRef, fileRef, xmpStr );
			wError->mErrorID = kXMPErr_NoError;
		}
	}
	catch( ... )
	{
		HandleException( wError );
	}

	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_ImportToXMP( SessionRef session, XMPMetaRef xmp, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;


	wError->mErrorID = kXMPErr_NoError;

	return wError->mErrorID;
}

// ============================================================================

static XMPErrorID Static_ExportFromXMP( SessionRef session, XMPMetaRef xmp, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;


	wError->mErrorID = kXMPErr_NoError;

	return wError->mErrorID;
}

// ============================================================================

XMPErrorID InitializePlugin( XMP_StringPtr moduleID, PluginAPIRef pluginAPI, WXMP_Error * wError )
{
	if( wError == NULL )	return kXMPErr_BadParam;

	wError->mErrorID = kXMPErr_PluginInitialized;
	
	if( !pluginAPI || moduleID == NULL )
	{
		wError->mErrorMsg = "pluginAPI or moduleID is NULL";
		return wError->mErrorID;
	}

	try
	{
		// Test if module identifier is same as found in the resource file MODULE_IDENTIFIER.txt
		bool identical = (0 == memcmp(GetModuleIdentifier(), moduleID, XMP_MIN(strlen(GetModuleIdentifier()), strlen(moduleID))));
		if ( !identical )
		{
			wError->mErrorMsg = "Module identifier doesn't match";
			return wError->mErrorID;
		}
	
		RegisterFileHandlers(); // Register all file handlers

		// Initialize all the registered file handlers
		if( PluginRegistry::initialize() )
		{
			pluginAPI->mVersion = PLUGIN_VERSION;
			pluginAPI->mSize = sizeof(PluginAPI);
			
			pluginAPI->mTerminatePluginProc = Static_TerminatePlugin;
			pluginAPI->mSetHostAPIProc      = Static_SetHostAPI;

			pluginAPI->mInitializeSessionProc = Static_InitializeSession;
			pluginAPI->mTerminateSessionProc = Static_TerminateSession;

			pluginAPI->mCheckFileFormatProc = Static_CheckFileFormat;
			pluginAPI->mCheckFolderFormatProc = Static_CheckFolderFormat;
			pluginAPI->mGetFileModDateProc = Static_GetFileModDate;
			pluginAPI->mCacheFileDataProc = Static_CacheFileData;
			pluginAPI->mUpdateFileProc = Static_UpdateFile;
			pluginAPI->mWriteTempFileProc = Static_WriteTempFile;

			pluginAPI->mImportToXMPProc = Static_ImportToXMP;
			pluginAPI->mExportFromXMPProc = Static_ExportFromXMP;

			wError->mErrorID = kXMPErr_NoError;
		}
	}
	catch( ... )
	{
		HandleException( wError );
	}
	
	return wError->mErrorID;
}

} //namespace XMP_PLUGIN
