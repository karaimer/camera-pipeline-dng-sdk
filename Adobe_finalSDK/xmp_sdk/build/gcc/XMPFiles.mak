# ==================================================================================================
# Copyright 2008 Adobe Systems Incorporated
# All Rights Reserved.
#
# NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# ==================================================================================================

# ---------------------------------
# Make sure the target OS is known.

Error =

TargetOS = ${os}
ifneq "${TargetOS}" "i80386linux"
	ifneq "${TargetOS}" "sparcsolaris"
		ifneq "${TargetOS}" "rs6000aix"
			ifneq "${TargetOS}" "hppahpux"
				Error += Invalid target OS "${TargetOS}"
			endif
		endif
	endif
endif

# deduct endianess and OSVer (not sure if the latter reflects much)
OSVer =
ifeq "${TargetOS}" "i80386linux"
	OSVer = linux2.6
	EndianSetting = -DkBigEndianHost=0
endif
ifeq "${TargetOS}" "sparcsolaris"
	OSVer = solaris9
	EndianSetting = -DkBigEndianHost=1
endif
ifeq "${TargetOS}" "rs6000aix"
	OSVer = aix5.2
	EndianSetting = -DkBigEndianHost=1
endif
ifeq "${TargetOS}" "hppahpux"
	OSVer = hp11.11
	EndianSetting = -DkBigEndianHost=1
endif

ifeq "${OSVer}" ""
	Error = Undefined OSVer
endif

# ensure libpath is set:
LibraryPath = ${libpath}
ifeq "${libpath}" ""
	Error += Unspecified library path. Must set libpath=...
endif

# -----------------------------------------------------------------------
# -----------------------------------------------------------------------
# Make sure the stage (debug/release) is known.

TargetStage = ${stage}
ifneq "${TargetStage}" "debug"
	ifneq "${TargetStage}" "release"
		Error += Invalid target stage "${TargetStage}"
	endif
endif

# architecture (x86, x64)
TargetArch = ${arch}
ifeq "${TargetArch}" ""
	Error += Unspecified target architecture "${TargetArch}"
else
	ifneq "${TargetArch}" "x86"
		ifneq "${TargetArch}" "x64"
			Error += Invalid target architecture "${TargetArch}"
		endif
	endif
endif

# ensure that in local mode, libpath is set:
LibraryPath = ${libpath}

# ------------------------------------------------------------
# Define variables for input and output directories and files.

XMPRoot  = ../..

# 64-bit specific additions: (added in every 32-bit case, remain empty if not 64 bit mode)
x64_ext =
x64_compileFlags =
x64_linkFlags =

ifeq "${TargetArch}" "x64"
	x64_ext = _x64
	x64_compileFlags =-m64
	x64_linkFlags =-m64
endif

LibRoot    = ${XMPRoot}/public/libraries/${TargetOS}${x64_ext}/${TargetStage}
TempRoot   = ${XMPRoot}/temp/XMPFiles/${TargetOS}${x64_ext}/${TargetStage}
TargetRoot = ${XMPRoot}/target/${TargetOS}${x64_ext}/${TargetStage}/bin

LibName = ${LibRoot}/staticXMPFiles${x64_ext}.ar
ModeOpt = -DXMP_StaticBuild=1

# ==================================================================================================

# ---------------------------------------------------------------------------------------------
# Define compile and link tools and options. We've moved to gcc on all platforms.

ToolName = gcc
LibPathVar = LD_LIBRARY_PATH
ifeq "${TargetOS}" "hppahpux"
	LibPathVar = SHLIB_PATH
endif

ToolPath = ${PATH}
Compile = PATH=${ToolPath} ${ToolName}
Link    = PATH=${ToolPath} ${LibPathVar}=${LibraryPath} ${ToolName}
	
CPPFlags =

XMPDefines = -DUNIX_ENV=1 -DHAVE_EXPAT_CONFIG_H=1 -DXML_STATIC=1 ${EndianSetting} ${ModeOpt}

ifneq "${TargetArch}" "x64"
	XMPDefines += -DXMP_64=0
else
	XMPDefines += -DXMP_64=1 -D__x86_64__=1
endif

DashG = -g
ifeq "${TargetOS}" "rs6000aix"
	DashG = -gxcoff
endif

ifeq "${TargetStage}" "debug"
	CompileFlags = ${DashG} -O0 -DDEBUG=1 -D_DEBUG=1
endif

OptFlags = -O2 -Os

ifeq "${TargetStage}" "release"
	CompileFlags = ${OptFlags} -DNDEBUG=1
endif

Includes = \
   -I${XMPRoot}/public/include \
   -I${XMPRoot} \
	 -I${XMPRoot}/XMPFilesPlugins/api/source \
   -I${XMPRoot}/build \
   -I${XMPRoot}/build/gcc

# --------------------------------
# Define settings specific to gcc.
CPPFlags = -x c++ -Wno-ctor-dtor-privacy

CompileFlags += -fPIC -funsigned-char -fexceptions -D_FILE_OFFSET_BITS=64
CompileFlags += -Wno-multichar -Wno-implicit

LinkFlags   = -shared -fPIC -z defs -Xlinker -Bsymbolic -Xlinker --version-script -Xlinker XMPFiles.exp
RuntimeLibs = -lc -lm -lpthread -L${LibraryPath} -Bdynamic -lstdc++ -shared-libgcc -ldl

ifeq "${TargetOS}" "i80386linux"
	LinkFlags += -Wl,-rpath,'$$ORIGIN'
	ifeq "${TargetArch}" "x86"
		CompileFlags += -m32 -mtune=i686
		LinkFlags    += -m32
	else
		CompileFlags += -m64
		LinkFlags    += -m64
	endif
endif

ifeq "${TargetOS}" "sparcsolaris"
	CompileFlags += -mcpu=ultrasparc
endif

# We need special link options on AIX and HPUX because gcc uses the native linker on them.

ifeq "${TargetOS}" "hppahpux"
	CompileFlags += -D_REENTRANT
	LinkFlags     = -shared -fPIC -Xlinker -Bsymbolic -Xlinker -c -Xlinker XMPFiles-acc.exp
	RuntimeLibs   = ${LibRoot}/libAdobeXMP.sl -lc -lm -L${ToolPath}/lib -Bdynamic -lstdc++ -shared-libgcc
endif		

ifeq "${TargetOS}" "rs6000aix"
	CompileFlags += -D_LARGE_FILE_API=1 -D_REENTRANT -pthread
	LinkFlags     = -shared -fPIC -Bsymbolic -Xlinker -bE:XMPFiles-vacpp.exp -pthread
	RuntimeLibs  += -Xlinker -brtl
endif

# add 64 bit flags (empty if n/a)
CompileFlags += ${x64_compileFlags}
LinkFlags += ${x64_linkFlags}

# ==================================================================================================
# -------------------------------
# Define the set of source files.

vpath %.incl_cpp\
    ${XMPRoot}/public/include:\
    ${XMPRoot}/public/include/client-glue:\

vpath %.cpp\
    ${XMPRoot}/XMPFiles/source:\
    ${XMPRoot}/XMPFiles/source/FileHandlers:\
    ${XMPRoot}/XMPFiles/source/FormatSupport:\
		${XMPRoot}/XMPFiles/source/FormatSupport/AIFF:\
		${XMPRoot}/XMPFiles/source/FormatSupport/IFF:\
		${XMPRoot}/XMPFiles/source/FormatSupport/WAVE:\
		${XMPRoot}/XMPFiles/source/NativeMetadataSupport:\
		${XMPRoot}/XMPFiles/source/PluginHandler:\
    ${XMPRoot}/source:\
    ${XMPRoot}/public/include:\
    ${XMPRoot}/public/include/client-glue:\
    ${XMPRoot}/third-party/zuid/interfaces:\

vpath %.c\
    ${XMPRoot}/third-party/zlib

CSources = \
	adler32.c \
    compress.c \
    crc32.c \
    deflate.c \
    gzclose.c \
    gzlib.c \
    gzread.c \
    gzwrite.c \
    inflate.c \
    infback.c \
    inftrees.c \
    inffast.c \
    trees.c \
    uncompr.c \
    zutil.c

CPPSources = \
	WXMPFiles.cpp \
	XMPFiles.cpp \
	XMPFiles_Impl.cpp \
	XMP_LibUtils.cpp \
	UnicodeConversions.cpp \
	XML_Node.cpp \
	MD5.cpp \
	ASF_Handler.cpp \
	AVCHD_Handler.cpp \
	RIFF_Handler.cpp \
	Basic_Handler.cpp \
	FLV_Handler.cpp \
	InDesign_Handler.cpp \
	JPEG_Handler.cpp \
	MP3_Handler.cpp \
	MPEG2_Handler.cpp \
	MPEG4_Handler.cpp \
	P2_Handler.cpp \
	PNG_Handler.cpp \
	PostScript_Handler.cpp \
	PSD_Handler.cpp \
	Scanner_Handler.cpp \
	SonyHDV_Handler.cpp \
	SWF_Handler.cpp \
	TIFF_Handler.cpp \
	Trivial_Handler.cpp \
	UCF_Handler.cpp \
	XDCAM_Handler.cpp \
	XDCAMEX_Handler.cpp \
	ASF_Support.cpp \
	IPTC_Support.cpp \
	MOOV_Support.cpp \
	ISOBaseMedia_Support.cpp \
	PNG_Support.cpp \
	PSIR_FileWriter.cpp \
	PSIR_MemoryReader.cpp \
	QuickTime_Support.cpp \
	Reconcile_Impl.cpp \
	ReconcileIPTC.cpp \
	ReconcileLegacy.cpp \
	ReconcileTIFF.cpp \
	RIFF.cpp \
	RIFF_Support.cpp \
	SWF_Support.cpp \
	TIFF_FileWriter.cpp \
	TIFF_MemoryReader.cpp \
	TIFF_Support.cpp \
	XDCAM_Support.cpp \
	XMPScanner.cpp \
	HandlerRegistry.cpp \
	AIFF_Handler.cpp \
	WAVE_Handler.cpp \
	ID3_Support.cpp \
	AIFFBehavior.cpp \
	AIFFMetadata.cpp \
	AIFFReconcile.cpp \
	Chunk.cpp \
	ChunkController.cpp\
	ChunkPath.cpp \
	IChunkBehavior.cpp \
	BEXTMetadata.cpp \
	CartMetadata.cpp \
	Cr8rMetadata.cpp \
	DISPMetadata.cpp \
	INFOMetadata.cpp \
	PrmLMetadata.cpp \
	WAVEBehavior.cpp \
	WAVEReconcile.cpp \
	IMetadata.cpp \
	IReconcile.cpp \
	MetadataSet.cpp \
	FileHandlerInstance.cpp \
	HostAPIImpl.cpp \
	Module.cpp \
	OS_Utils_Linux.cpp \
	PluginManager.cpp \
	XMPAtoms.cpp \
	Host_IO-UNIX.cpp \
	Host_IO-POSIX.cpp \
	XIO.cpp \
	XMPFiles_IO.cpp

CObjs   = $(foreach objs,${CSources:.c=.o},${TempRoot}/$(objs))
CPPObjs = $(foreach objs,${CPPSources:.cpp=.o},${TempRoot}/$(objs))

.SUFFIXES:                # Delete the default suffixes
.SUFFIXES: .o .c .cpp     # Define our suffix list

# ==================================================================================================

# ---------------------------------------
# Define the generic compile build rules.

${TempRoot}/%.o : %.c
	@echo "------------------------------"
	@echo "source= $<"
	@echo "target= $@"
	@echo x64_ext = ${x64_ext}
	@echo x64_compileFlags = ${x64_compileFlags}
	@echo x64_linkFlags = ${x64_linkFlags}
	rm -f $@
	${Compile} ${CompileFlags} ${XMPDefines} ${Includes} -c $< -o $@
	
${TempRoot}/%.o : %.cpp
	@echo "------------------------------"
	@echo "source= $<"
	@echo "target= $@"
	@echo x64_ext = ${x64_ext}
	@echo x64_compileFlags = ${x64_compileFlags}
	@echo x64_linkFlags = ${x64_linkFlags}
	rm -f $@
	${Compile} ${CPPFlags} ${CompileFlags} ${XMPDefines} ${Includes} -c $< -o $@
	
# ==================================================================================================

.PHONY: all rebuild msg create_dirs

# full re-build (with prior cleaning)
all : msg clean create_dirs cleanTemp ${LibName}

# currently a forward, soon to be the real thing ('all' only outputting usage info)
rebuild: all

# lazy build
build: msg create_dirs ${LibName} listOutput
	@echo "lazy build done."

msg :
ifeq "${Error}" ""
	@echo "----------------------------------------------------------------------------------"
	@echo Building XMPFiles for ${TargetOS} ${TargetStage} --------------------
else
	@echo ""
	@echo "Error: ${Error}"
	@echo ""
	@echo "# To build the Adobe XMPFiles DLL:"
	@echo "#   make -f XMPFiles.mak [os=<os>] [stage=<stage>] [arch=<arch>] "
	@echo "# where"
	@echo "#   os    = i80386linux | sparcsolaris | rs6000aix | hppahpux"
	@echo "#   stage = debug | release"
	@echo "#   arch  = x86 | x64"
	@echo "#"
	@echo "# The os and stage symbols can also be uppercase, OS and STAGE."
	@echo "# If the os is omitted it will try to default from the OSTYPE"
	@echo "# and MACHTYPE environment variables. If the stage is omitted"
	@echo "# it defaults to debug."
	@echo ""
	@exit 1
endif

create_dirs :
	@mkdir -p ${LibRoot} ${TempRoot} ${TargetRoot}

${LibName} : ${CObjs} ${CPPObjs}
	@echo ""
	@echo "Linking $@"
	rm -f $@
	ar -qsc $@ ${CObjs} ${CPPObjs}
	@#touch file to set actual, full date
	@touch $@

clean: cleanTemp cleanLib

cleanTemp:
	rm -rf ${TempRoot}/*
	
cleanLib:
	rm -f  ${LibName}
	rm -rf ${XMPRoot}/build/common-unix/intermediate/${TargetOS}/

listOutput:
	@echo "output directory: ${LibRoot}"
	@ls -l ${LibRoot}

