// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "Module.h"

namespace XMP_PLUGIN
{

PluginAPIRef Module::getPluginAPIs() 
{ 
	//
	// return ref. to Plugin API, load module if not yet loaded
	//
	if( !mPluginAPIs && !load() )
	{
		XMP_Throw ( "Plugin API not available.", kXMPErr_Unavailable );
	}

	return mPluginAPIs;
}

bool Module::load()
{
	if( mLoaded == kModuleNotLoaded )
	{
		std::string errorMsg;

		//
		// load module
		//
		mLoaded = kModuleErrorOnLoad;
		mHandle = LoadModule(mPath, false);

		if( mHandle != NULL )
		{
			//
			// get entry point function pointer
			//
			InitializePluginProc InitializePlugin = reinterpret_cast<InitializePluginProc>( 
				GetFunctionPointerFromModuleImpl(mHandle, "InitializePlugin") );
			
			if( InitializePlugin != NULL )
			{
				//
				// get module ID from plugin resource
				//
				std::string moduleID;
				GetResourceDataFromModule(mHandle, "MODULE_IDENTIFIER", "txt", moduleID);
				mPluginAPIs = new PluginAPI();
				
				//
				// initialize plugin by calling entry point function
				//
				WXMP_Error error;
				InitializePlugin( moduleID.c_str(), mPluginAPIs, &error );

				if( error.mErrorID == kXMPErr_NoError
					&& mPluginAPIs->mTerminatePluginProc
					&& mPluginAPIs->mSetHostAPIProc
					&& mPluginAPIs->mInitializeSessionProc
					&& mPluginAPIs->mTerminateSessionProc
					&& mPluginAPIs->mCacheFileDataProc
					&& mPluginAPIs->mUpdateFileProc
					&& mPluginAPIs->mWriteTempFileProc
					)
				{
					//
					// set host API at plugin
					//
					HostAPIRef hostAPI = PluginManager::getHostAPI( mPluginAPIs->mVersion );
					mPluginAPIs->mSetHostAPIProc( hostAPI, &error );
					
					if( error.mErrorID == kXMPErr_NoError )
					{
						mLoaded = kModuleLoaded;
					}
					else
					{
						errorMsg = "Incompatible plugin API version. (" + moduleID + ")";
					}
				}
				else
				{
					if( error.mErrorID != kXMPErr_NoError )
					{
						errorMsg = "Plugin initialization failed. (" + moduleID + ")";
					}
					else
					{
						errorMsg = "Plugin API incomplete. (" + moduleID + ")";
					}
				}
			}
			else
			{
				errorMsg = "Missing plugin entry point \"InitializePlugin\" in plugin " + mPath;
			}

			if( mLoaded != kModuleLoaded )
			{
				//
				// plugin wasn't loaded and initialized successfully,
				// so unload the module
				//
				this->unload();
			}
		}
		else
		{
			errorMsg = "Can't load module " + mPath;
		}

		if( mLoaded != kModuleLoaded )
		{
			//
			// error occurred
			//
			throw XMP_Error( kXMPErr_InternalFailure, errorMsg.c_str() );
		}
	}

	return ( mLoaded == kModuleLoaded ); 
}

void Module::unload()
{
	WXMP_Error error;

	//
	// terminate plugin
	//
	if( mPluginAPIs != NULL )
	{
		if( mPluginAPIs->mTerminatePluginProc ) 
        {
            mPluginAPIs->mTerminatePluginProc( &error );
        }
		delete mPluginAPIs;
		mPluginAPIs = NULL;
	}

	//
	// unload plugin module
	//
	if( mLoaded != kModuleNotLoaded )
	{
		UnloadModule(mHandle, false);
		mHandle = NULL;
		if( mLoaded == kModuleLoaded )
		{
			//
			// Reset mLoaded to kModuleNotLoaded, if the module was loaded successfully.
			// Otherwise let it remain kModuleErrorOnLoad so that we won't try to load 
			// it again if some other handler ask to do so.
			//
			mLoaded = kModuleNotLoaded;
		}
	}

	CheckError( error );
}

} //namespace XMP_PLUGIN
