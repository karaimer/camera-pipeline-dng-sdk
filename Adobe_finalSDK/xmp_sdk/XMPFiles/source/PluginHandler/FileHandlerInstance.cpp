// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "FileHandlerInstance.h"

namespace XMP_PLUGIN
{

FileHandlerInstance::FileHandlerInstance ( SessionRef object, FileHandlerSharedPtr handler, XMPFiles * _parent ):
XMPFileHandler( _parent ), mObject( object ), mHandler( handler )
{
	this->handlerFlags = mHandler->getHandlerFlags();
	this->stdCharForm  = kXMP_Char8Bit;
	PluginManager::addHandlerInstance( this->mObject, this );
}

FileHandlerInstance::~FileHandlerInstance()
{
	WXMP_Error error;
	mHandler->getModule()->getPluginAPIs()->mTerminateSessionProc( this->mObject, &error );
	PluginManager::removeHandlerInstance( this->mObject );
	CheckError( error );
}

bool FileHandlerInstance::GetFileModDate ( XMP_DateTime * modDate )
{
	bool ok;
	WXMP_Error error;
	GetSessionFileModDateProc wGetFileModDate = mHandler->getModule()->getPluginAPIs()->mGetFileModDateProc;
	wGetFileModDate ( this->mObject, &ok, modDate, &error );
	CheckError ( error );
	return ok;
}

void FileHandlerInstance::CacheFileData()
{
	if( this->containsXMP ) return;
	
	WXMP_Error error;
	XMP_StringPtr xmpStr = NULL;
	mHandler->getModule()->getPluginAPIs()->mCacheFileDataProc( this->mObject, this->parent->ioRef, &xmpStr, &error );
	
	if( error.mErrorID != kXMPErr_NoError )
	{
		if ( xmpStr != 0 ) free( (void*) xmpStr );
		throw XMP_Error( kXMPErr_InternalFailure, error.mErrorMsg );
	}

	if( xmpStr != NULL )
	{
		this->xmpPacket.assign( xmpStr );
		free( (void*) xmpStr ); // It should be freed as documentation of mCacheFileDataProc says so.
	}
	this->containsXMP = true;
}

void FileHandlerInstance::ProcessXMP()
{
	if( !this->containsXMP || this->processedXMP ) return;
	this->processedXMP = true;

	SXMPUtils::RemoveProperties ( &this->xmpObj, 0, 0, kXMPUtil_DoAllProperties );
	this->xmpObj.ParseFromBuffer ( this->xmpPacket.c_str(), (XMP_StringLen)this->xmpPacket.size() );

	WXMP_Error error;
	if( mHandler->getModule()->getPluginAPIs()->mImportToXMPProc )
		mHandler->getModule()->getPluginAPIs()->mImportToXMPProc( this->mObject, this->xmpObj.GetInternalRef(), &error );
	CheckError( error );
}

void FileHandlerInstance::UpdateFile ( bool doSafeUpdate )
{
	if ( !this->needsUpdate || this->xmpPacket.size() == 0 ) return;

	WXMP_Error error;
	if( mHandler->getModule()->getPluginAPIs()->mExportFromXMPProc )
		mHandler->getModule()->getPluginAPIs()->mExportFromXMPProc( this->mObject, this->xmpObj.GetInternalRef(), &error );
	CheckError( error );

	this->xmpObj.SerializeToBuffer ( &this->xmpPacket, mHandler->getSerializeOption() );
	
	mHandler->getModule()->getPluginAPIs()->mUpdateFileProc( this->mObject, this->parent->ioRef, doSafeUpdate, this->xmpPacket.c_str(), &error );
	CheckError( error );
	this->needsUpdate = false;
}

void FileHandlerInstance::WriteTempFile( XMP_IO* tempRef )
{
	WXMP_Error error;
	if( mHandler->getModule()->getPluginAPIs()->mExportFromXMPProc )
		mHandler->getModule()->getPluginAPIs()->mExportFromXMPProc( this->mObject, this->xmpObj.GetInternalRef(), &error );
	CheckError( error );

	this->xmpObj.SerializeToBuffer ( &this->xmpPacket, mHandler->getSerializeOption() );

	mHandler->getModule()->getPluginAPIs()->mWriteTempFileProc( this->mObject, this->parent->ioRef, tempRef, this->xmpPacket.c_str(), &error );
	CheckError( error );
}

} //namespace XMP_PLUGIN
