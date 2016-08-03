// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

/**************************************************************************
* @file PluginBase.h
* @brief Basic functionality of the plugin.
*
* All plugin should be derived from PluginBase. This is the basic miinimum 
* functionalty which a  plugin should provide.
* 
* @author Praveen Kumar Goyal (pkgoyal)
* @bug No known bugs.
***************************************************************************/

#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <string>
#include <string.h>
#include "HostAPIAccess.h"
#define TXMP_STRING_TYPE std::string
#include "XMP.hpp"

namespace XMP_PLUGIN
{

/** Base class for the plugin file handler. All file handler should be derived from PluginBase, 
    which is an abstract class. Plugin developer need to implement all of the virtual functions. 

	virtual void cacheFileData( const IOAdapter& file, std::string& xmpStr );
	virtual void updateFile( const IOAdapter& file, bool doSafeUpdate, const std::string& xmpStr );
	virtual void writeTempFile( const IOAdapter& srcFile, const IOAdapter& tmpFile, const std::string& xmpStr ) ;
	virtual void importToXMP( SXMPMeta& xmp );
	virtual void exportFromXMP( const SXMPMeta& xmp );

    First two functions are pure virtual functions so these should be implemented for sure. 
    Last three functions(eg. writeTempFile, importToXMP, exportToXMP) may not be required by the file
    handler so plug-in developer may want to skip these functions. 

    Plugin developer also need to implement following static functions.

	static bool initialize();
	static bool terminate();
	static bool checkFileFormat( const std::string& filePath, const HostFileSys& file );
	static inline bool checkFolderFormat( const std::string& rootPath, const std::string& gpName, const std::string& parentName, const std::string& leafName );

	Though file handler may need only one of the last two function but both of them need to be implemented.
	One function which is actually required, should be implemented properly and the other function will just return false.
 */

/** @class PluginBase
 *  @brief Base class of the plugin.
 *
 *  All plugin should be derived from PluginBase. This is the basic minimum 
 *  functionality which a file handler should provide.
 */

class PluginBase
{
public:
	PluginBase( const std::string& filePath, XMP_Uns32 openFlags, XMP_Uns32 format = 0, XMP_Uns32 handlerFlags = 0 ) 
		: mPath( filePath ),mHandlerFlags(handlerFlags), mOpenFlags( openFlags ), mFormat( format ) {}
	virtual ~PluginBase(){};

	/** @brief Delegator functions which will eventually call the corresponding virtual function.
	 */
	void cacheFileData( XMP_IORef fileRef, XMP_StringPtr* xmpStr );
	void updateFile( XMP_IORef fileRef, bool doSafeUpdate, XMP_StringPtr xmpStr );
	void writeTempFile( XMP_IORef srcFileRef, XMP_IORef fileRef, XMP_StringPtr xmpStr );
	/** @brief Get file format
	 *
	 * Get the file format of this handler. 
	 * The format is defined in the plugin manifest.
	 */
	inline XMP_FileFormat getFormat() const					{ return mFormat; }

	/** @brief Get handler flags
	 *
	 * Get the handler flags for the file handler related to the current format.
	 * The flags are defined in the plugin manifest.
	 */
	inline XMP_OptionBits getHandlerFlags() const			{ return mHandlerFlags; }


	/** @brief Return the path to the input file/folder
	 *
	 *  Return the path to the input file/folder. Empty if the data source is neither
	 *  a file nor folder.
	 *  @return Path string
	 */
	inline const std::string& getPath() const { return mPath; }

	/** @brief Return the open flags.
	 *
	 *  Return the flags that describe the desired access
	 *  @return flags
	 */
	inline XMP_OptionBits getOpenFlags() const { return mOpenFlags; }

	/** @brief Ask XMPFiles if current operation should be aborted.
	 *
	 *  @param doAbort	If true and the operation should be aborted then
	 *					abort it by throwing an exception
	 *  @return true if the current operation should be aborted
	 */
	bool checkAbort( bool doAbort = false );

	/** @brief Return the modification date/time of the metadata file
	 *
	 * The purpose of the method getFileModDate is to return the most recent file system modification timestamp 
	 * for any associated file containing metadata, XMP or non-XMP. In the case of a typical single file with 
	 * embedded metadata this is that one file. In the case of embedded non-XMP and sidecar XMP, this is the more 
	 * recent of both files.
	 * 
	 * The default implementation only cares about the case of a single file with embedded metadata.
	 * All other cases will fail and return false.
	 *
	 * @param modDate	A required pointer to return the modification date.
	 * @return			True if a modification date could be determined
	 *
	 */
	virtual bool getFileModDate ( XMP_DateTime* modDate );

	/** Virtual functions which need to be implemented by the plugin Developer in the derived class. 
	 */
	virtual void cacheFileData( const IOAdapter& file, std::string& xmpStr ) = 0;
	virtual void updateFile( const IOAdapter& file, bool doSafeUpdate, const std::string& xmpStr ) = 0;
	virtual void writeTempFile( const IOAdapter& srcFile, const IOAdapter& tmpFile, const std::string& xmpStr ) {}
private:
	std::string		mPath;
	XMP_OptionBits	mHandlerFlags;
	XMP_OptionBits	mOpenFlags;
	XMP_FileFormat	mFormat;
};


/** External functions which need to be implemented by Plugin Developer.
 *  This function are unique for a plugins. Unlike PluginBase class which is used 
 *  for each file handler, these function are written only once for each plug-in.
 */
 
/** @brief Returns the unique identifier string of the plug-in.
 *
 *  Return the uid string of the plugin. This string should match with the string which is present in
 *  plugins resource file "MODULE_IDENTIFIER.txt".
 */
const char* GetModuleIdentifier();

/** @brief Register the file handlers available in the plug-in. 
 *
 *  This function need to be implemented by plugin-developer. This function register the file handlers available in the plugin. 
 *  Plug-in developer may want to register only few of the available plug-ins as per requirement. 
 *  Only registered plug-in will be loaded by the XMPFiles.
 */
void RegisterFileHandlers();
	
} //namespace XMP_PLUGIN
#endif // PLUGINBASE_H

