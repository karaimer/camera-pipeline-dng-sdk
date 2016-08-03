// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#ifndef MODULE_H
#define MODULE_H
#include "ModuleUtils.h"
#include "PluginManager.h"

namespace XMP_PLUGIN
{

/** @class Module
 *  @brief Manages module's loading and unloading.
 */
class Module
{
public:	
	Module( std::string & path ):
	  mPath( path ), mHandle( NULL ), mPluginAPIs( NULL ), mLoaded( kModuleNotLoaded ) { }
	
	~Module() { unload(); }

	inline OS_ModuleRef getHandle() const { return mHandle; }
	inline const std::string & getPath() const { return mPath; }
	
	/**
	 *  Returns pluginAPI. It loads the module if not already loaded.
	 *  @return pluginAPI.
	 */
	PluginAPIRef getPluginAPIs();

	/**
	 *  It loads the module if not already loaded.
	 *  @return true if module is loaded successfully otherwise returns false.
	 */
	bool load();

	/**
	 *  Unloads the module if it is loaded.
	 *  @return Void.
	 */
	void unload();

private:
	typedef enum
	{
		kModuleNotLoaded = 0,
		kModuleLoaded,
		kModuleErrorOnLoad
	} LoadStatus;
	
	std::string			mPath;
	OS_ModuleRef		mHandle;
	PluginAPIRef		mPluginAPIs;
	LoadStatus			mLoaded;
};

} //namespace XMP_PLUGIN
#endif //MODULE_H
