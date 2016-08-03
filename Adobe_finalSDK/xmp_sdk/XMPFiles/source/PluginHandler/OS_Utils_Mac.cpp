// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "ModuleUtils.h"
#include "source/UnicodeConversions.hpp"
#include "source/XIO.hpp"

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFDate.h>
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFError.h>
#include <string>


namespace XMP_PLUGIN
{

/** ************************************************************************************************************************
** Auto-releasing wrapper for Core Foundation objects
** AutoCFRef is an auto-releasing wrapper for any Core Foundation data type that can be passed
** to CFRetain and CFRelease. When constructed with a Core Foundation object, it is assumed it
** has just been created; that constructor does not call CFRetain. This supports reference counting
** through Core Foundation's own mechanism.
*/
template <class T>
class AutoCFRef
{
	
public:
	/// Default constructor creates NULL reference
	AutoCFRef()
		{
			mCFTypeRef = NULL;
		}
	
	/// Construct with any CFXXXRef type.
	explicit AutoCFRef(const T& value)
		{
			mCFTypeRef = value;
		}
	
	/// Copy constructor
	AutoCFRef(const AutoCFRef<T>& rhs)
		{
			mCFTypeRef = rhs.mCFTypeRef;
			if (mCFTypeRef)
				CFRetain(mCFTypeRef);
		}
	
	/// Destructor
	~AutoCFRef()
		{
			if (mCFTypeRef)
				CFRelease(mCFTypeRef);
		}
	
	/// Assignment operator
	void operator=(const AutoCFRef<T>& rhs)
		{
			if (!Same(rhs))
			{
				if (mCFTypeRef)
					CFRelease(mCFTypeRef);
				mCFTypeRef = rhs.mCFTypeRef;
				if (mCFTypeRef)
					CFRetain(mCFTypeRef);
			}
		}
	
	/// Equivalence operator
	/** operator== returns logical equivalence, the meaning of which varies from class to class
		in Core Foundation. See also AutoCFRef::Same.
		@param rhs The AutoCFRef<T> to compare to
		@return true if objects are logically equivalent.
	*/
	bool operator==(const AutoCFRef<T>& rhs) const
		{
			if (mCFTypeRef && rhs.mCFTypeRef)
				return CFEqual(mCFTypeRef, rhs.mCFTypeRef);
			else
				return mCFTypeRef == rhs.mCFTypeRef;
		}
	
	/// Non-equivalence operator
	/** operator!= returns logical equivalence, the meaning of which varies from class to class
		in Core Foundation. See also AutoCFRef::Same.
		@param rhs The AutoCFRef<T> to compare to
		@return true if objects are not logically equivalent.
	*/
	bool operator!=(const AutoCFRef<T>& rhs) const
		{
			return !operator==(rhs);
		}
	
	/// References same CF object
	/** Same returns true if both objects reference the same CF object (or both are NULL).
		For logical equivalence (CFEqual) use == operator.
		@param rhs The AutoCFRef<T> to compare to
		@return true if AutoRefs reference the same object.
	*/
	bool Same(const AutoCFRef<T>& rhs) const
		{
			return mCFTypeRef == rhs.mCFTypeRef;
		}
		
	/// Change the object referenced
	/** Reset is used to put a new CF object into a preexisting AutoCFRef. Does NOT call CFRetain,
		so if its ref count is 1 on entry, it will delete on destruction, barring other influences.
		@param value (optional) the new CFXXXRef to set, default is NULL.
	*/
	void Reset(T value = NULL)
		{
			if (value != mCFTypeRef)
			{
				if (mCFTypeRef)
					CFRelease(mCFTypeRef);
				mCFTypeRef = value;
			}
		}
	
	/// Return true if no object referenced.
	bool IsNull() const
		{
			return mCFTypeRef == NULL;
		}
	
	/// Return retain count.
	/** Returns retain count of referenced object from Core Foundation. Can be used to track down
		failures in reference management, but be aware that some objects might be returned to your
		code by the operating system with a retain count already greater than one.
		@return Referenced object's retain count.
	*/
	CFIndex RetainCount()
		{
			return mCFTypeRef ? CFGetRetainCount(mCFTypeRef) : 0;
		}
	
	/// Const dereference operator
	/** operator* returns a reference to the contained CFTypeRef. Use this to pass the object into
		Core Foundation functions. DO NOT use this to create a new AutoCFRef; copy construct instead.
	*/
	const T& operator*() const
		{
			return mCFTypeRef;
		}

	/// Nonconst dereference operator
	/** operator* returns a reference to the contained CFTypeRef. Use this to pass the object into
		Core Foundation functions. DO NOT use this to create a new AutoCFRef; copy construct instead.
	*/
	T& operator*()
		{
			return mCFTypeRef;
		}

private:
	T			mCFTypeRef;
};

typedef AutoCFRef<CFURLRef>					AutoCFURL;
typedef AutoCFRef<CFStringRef>				AutoCFString;

typedef std::tr1::shared_ptr<FSIORefNum>	FilePtr;


/** ************************************************************************************************************************
** Convert string into CFString
*/
static inline CFStringRef MakeCFString(const std::string& inString, CFStringEncoding inEncoding = kCFStringEncodingUTF8)
{
	CFStringRef str = ::CFStringCreateWithCString( NULL, inString.c_str(), inEncoding );
	return str;
}

/** ************************************************************************************************************************
** Convert CFString into std::string
*/
static std::string MacToSTDString(CFStringRef inCFString)
{
	std::string result;

	if (inCFString == NULL)
	{
		return result;
	}

	// The CFStringGetLength returns the length of the string in UTF-16 encoding units.
	::CFIndex const stringUtf16Length = ::CFStringGetLength(inCFString);
	if (stringUtf16Length == 0)
	{
		return result;
	}

	// Check if the CFStringRef can allow fast-return.
	char const* ptr = ::CFStringGetCStringPtr(inCFString, kCFStringEncodingUTF8);
	if (ptr != NULL)
	{
		result = std::string( ptr ); // This kind of assign expects '\0' termination.
		return result;
	}

	// Since this is an encoding conversion, the converted string may not be the same length in bytes
	// as the CFStringRef in UTF-16 encoding units.

	::UInt8 const noLossByte = 0;
	::Boolean const internalRepresentation = FALSE; // TRUE is external representation, with a BOM.  FALSE means no BOM.
	::CFRange const range = ::CFRangeMake(0, stringUtf16Length); // [NOTE] length is in UTF-16 encoding units, not bytes.
	::CFIndex const maxBufferLength = ::CFStringGetMaximumSizeForEncoding(stringUtf16Length, kCFStringEncodingUTF8); // Convert from UTF-16 encoding units to UTF-8 worst-case-scenario encoding units, in bytes.
	::CFIndex usedBufferLength = 0; // In byte count.
	char buffer[1024];
	memset( buffer, 0, 1024 );
	::CFIndex numberOfUtf16EncodingUnitsConverted = ::CFStringGetBytes(inCFString, range, ::kCFStringEncodingUTF8, noLossByte, internalRepresentation, (UInt8*)buffer, maxBufferLength, &usedBufferLength);
	result = std::string( buffer );

	return result;
}


static void CloseFile(	FSIORefNum* inFilePtr )
{
	FSCloseFork(*inFilePtr);
	delete inFilePtr;
}

static FilePtr OpenResourceFile( OS_ModuleRef inOSModule, const std::string& inResourceName, const std::string& inResourceType, bool inSearchInSubFolderWithNameOfResourceType)
{
	FilePtr file;
	if (inOSModule != nil)
	{ 
		AutoCFString resourceName( MakeCFString( inResourceName ) );
		AutoCFString resourceType( MakeCFString( inResourceType ) );
		AutoCFString subfolderName(inSearchInSubFolderWithNameOfResourceType ? MakeCFString( inResourceType ) : nil);

		AutoCFURL url(
			::CFBundleCopyResourceURL(inOSModule, *resourceName, *resourceType, *subfolderName));

		FSRef fileReference;
		if (!url.IsNull() && ::CFURLGetFSRef(*url, &fileReference))
		{
			HFSUniStr255 str;
			if (::FSGetDataForkName(&str) == noErr)
			{
				FSIORefNum forkRef;
				if (::FSOpenFork(&fileReference, str.length, str.unicode, fsRdPerm, &forkRef) == noErr)
				{
					file.reset(new FSIORefNum(forkRef), CloseFile);
				}
			}
		}
	}
	return file;
}

bool IsValidLibrary( const std::string & inModulePath )
{
	bool result = false;
	AutoCFURL bundleURL(::CFURLCreateFromFileSystemRepresentation(
		kCFAllocatorDefault,
		(const UInt8*) inModulePath.c_str(),
		inModulePath.size(),
		false));
	if (*bundleURL != NULL)
	{
		CFBundleRef bundle = ::CFBundleCreate(kCFAllocatorDefault, *bundleURL);
		if (bundle != NULL)
		{
			CFArrayRef arrayRef = ::CFBundleCopyExecutableArchitectures(bundle);
			if (arrayRef != NULL)
			{
				result = true;
				::CFRelease(arrayRef);
			}

			::CFRelease(bundle);
		}
	}		
	return result;
}

OS_ModuleRef LoadModule( const std::string & inModulePath, bool inOnlyResourceAccess )
{
	OS_ModuleRef result = NULL;
	AutoCFURL bundleURL(::CFURLCreateFromFileSystemRepresentation(
		kCFAllocatorDefault,
		(const UInt8*) inModulePath.c_str(),
		inModulePath.size(),
		false));
	if (*bundleURL != NULL)
	{
		result = ::CFBundleCreate(kCFAllocatorDefault, *bundleURL);
		if (!inOnlyResourceAccess && (result != NULL))
		{
			::CFErrorRef errorRef = NULL;
			Boolean loaded = ::CFBundleIsExecutableLoaded(result);
			if (!loaded)
			{
				loaded = ::CFBundleLoadExecutableAndReturnError(result, &errorRef);
				if(!loaded || errorRef != NULL)
				{
					AutoCFString errorDescr(::CFErrorCopyDescription(errorRef));
					throw XMP_Error( kXMPErr_InternalFailure, MacToSTDString(*errorDescr).c_str() );
					::CFRelease(errorRef);
					// release bundle and return NULL
					::CFRelease(result);
					result = NULL;
				}
			}
		}
	}
	return result;
}

void UnloadModule( OS_ModuleRef inModule, bool inOnlyResourceAccess )
{
	if (inModule != NULL)
	{
		::CFRelease(inModule);
	}
}

void* GetFunctionPointerFromModuleImpl(	OS_ModuleRef inOSModule, const char* inSymbol )
{
	void* proc = NULL;
	if( inOSModule != NULL)
	{
		proc = ::CFBundleGetFunctionPointerForName( inOSModule, *AutoCFString(MakeCFString(inSymbol)) );
	}
	return proc;
}

bool GetResourceDataFromModule(
	OS_ModuleRef inOSModule,
	const std::string & inResourceName,
	const std::string & inResourceType,
	std::string & outBuffer)
{
	bool success = false;
	
	if (FilePtr file = OpenResourceFile(inOSModule, inResourceName, inResourceType, false))
	{
		ByteCount	size = 0;
		SInt64		fork_size = 0;
		::FSGetForkSize(*file.get(), &fork_size);
		// presumingly we don't want to load more than 2GByte at once (!)
		if( fork_size < std::numeric_limits<XMP_Int32>::max() )
		{
			size = static_cast<ByteCount>(fork_size); 
			if (size > 0)
			{
				outBuffer.resize(size);
				::FSReadFork(*file.get(), fsFromStart, 0, size, (unsigned char*)&outBuffer[0], (ByteCount*)&size);
			}
			success = true;
		}
	}

	return success;
}

} //namespace XMP_PLUGIN
