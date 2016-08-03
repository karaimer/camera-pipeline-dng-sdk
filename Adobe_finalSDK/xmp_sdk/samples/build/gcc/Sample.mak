# ==================================================================================================
# Copyright 2006 Adobe Systems Incorporated
# All Rights Reserved.
#
# NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# ==================================================================================================

# ==================================================================================================

# Define variables set by user.

Error =

Sample = ${NAME}

ifeq "${Sample}" ""
	Sample = ${name}
endif

ifeq "${Sample}" ""
	Error += The sample name must be provided
endif

TargetOS = ${OS}

ifeq "${TargetOS}" ""
	TargetOS = ${os}
endif

ifeq "${TargetOS}" ""
	TargetOS = ${MACHTYPE}${OSTYPE}
endif

ifeq "${TargetOS}" "i386linux"
	TargetOS = i80386linux
endif

ifeq "${TargetOS}" "linux"
	TargetOS = i80386linux
endif

ifeq "${TargetOS}" "solaris"
	TargetOS = sparcsolaris
endif

ifneq "${TargetOS}" "i80386linux"
	ifneq "${TargetOS}" "sparcsolaris"
		Error += Invalid target OS "${TargetOS}"
	endif
endif

TargetStage = ${STAGE}

ifeq "${TargetStage}" ""
	TargetStage = ${stage}
endif

ifeq "${TargetStage}" ""
	TargetStage = debug
endif

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

# ensure libpath is set:
LibraryPath = ${libpath}
ifeq "${libpath}" ""
	Error += Unspecified library path. Must set libpath=...
endif

# ==================================================================================================
# Paths

XMPRoot    = ../../..
SampleRoot = ${XMPRoot}/samples

ifeq "${TargetArch}" "x64"
        x64_ext = _x64
else
	x64_ext =
endif

XMPLibRoot = ${XMPRoot}/public/libraries/${TargetOS}${x64_ext}/${TargetStage}

TargetRoot = ${SampleRoot}/target/${TargetOS}${x64_ext}/${TargetStage}
TempRoot   = ${SampleRoot}/temp/${Sample}/${TargetOS}${x64_ext}/${TargetStage}

LibXMPCore  = ${XMPLibRoot}/staticXMPCore${x64_ext}.ar
LibXMPFiles = ${XMPLibRoot}/staticXMPFiles${x64_ext}.ar

# ==================================================================================================
# project specific settings

ifeq "${Sample}" "XMPCoreCoverage"
	ExtraObjects =
	ExtraLibs    =
endif

ifeq "${Sample}" "XMPFilesCoverage"
	ExtraObjects =
	ExtraLibs    = ${LibXMPFiles}
endif

ifeq "${Sample}" "DumpMainXMP"
	ExtraObjects =
	ExtraLibs    = ${LibXMPFiles}
endif

ifeq "${Sample}" "DumpScannedXMP"
	ExtraObjects = ${TempRoot}/XMPScanner.o
	ExtraLibs    = ${LibXMPFiles}
endif

ifeq "${Sample}" "ReadingXMP"
	ExtraObjects =
	ExtraLibs    = ${LibXMPFiles}
endif

ifeq "${Sample}" "XMPIterations"
	ExtraObjects = 
	ExtraLibs    = ${LibXMPFiles}
endif

ifeq "${Sample}" "CustomSchema"
	ExtraObjects =
	ExtraLibs    = 
endif

ifeq "${Sample}" "ModifyingXMP"
	ExtraObjects =
	ExtraLibs    = ${LibXMPFiles}
endif

ifeq "${Sample}" "DumpFile"
	ExtraObjects = ${TempRoot}/main.o ${TempRoot}/Log.o ${TempRoot}/TagTree.o ${TempRoot}/LargeFileAccess.o ${TempRoot}/XMPScanner.o
	ExtraLibs    = ${LibXMPFiles}
endif

ifeq "${Sample}" "XMPCommand"
	ExtraObjects = ${TempRoot}/Log.o ${TempRoot}/Actions.o ${TempRoot}/PrintUsage.o ${TempRoot}/UnicodeConversions.o ${TempRoot}/LargeFileAccess.o
	ExtraLibs    = ${LibXMPFiles}
endif
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

# Do something innocuous by default for Strip.
Strip = ls

ifeq "${TargetStage}" "release"
	Strip = PATH=${ToolPath} strip

	ifeq "${TargetOS}" "rs6000aix"
		Strip = PATH=/usr/bin strip
	endif

	ifeq "${TargetOS}" "hppahpux"
		# Don't have a good gcc strip on HPUX yet.
		Strip = ls
	endif
endif

# ==================================================================================================
# compile and link settings

CFlags =  -DUNIX_ENV=1 -D_FILE_OFFSET_BITS=64 -DXMP_StaticBuild=1
CFlags += -fexceptions -funsigned-char -fPIC -Wno-multichar -Wno-implicit

LDFlags =

ifeq "${TargetOS}" "i80386linux"
	CFlags += -D__LITTLE_ENDIAN__=1
	LDFlags += -Wl,-rpath,'$$ORIGIN'
	ifeq "${TargetArch}" "x86"
		CFlags += -m32 -mtune=i686 -DXMP_64=0
		LDFlags+= -m32
	else
		CFlags += -m64 -DXMP_64=1 -D__x86_64__=1
		LDFlags+= -m64
	endif
endif

ifeq "${TargetOS}" "sparcsolaris"
	CFlags += -D__BIG_ENDIAN__=1 -mcpu=ultrasparc
endif

ifeq "${TargetStage}" "debug"
	CFlags += -g -O0 -DDEBUG=1 -D_DEBUG=1
else
	CFlags += -O2 -Os -DNDEBUG=1
endif

LDLibs  =  ${LibXMPCore} ${ExtraLibs} -Xlinker -R -Xlinker .
LDLibs  += -lc -lm -lpthread -L${LibraryPath} -Bdynamic -lstdc++ -shared-libgcc -ldl

ifeq "${TargetOS}" "sparcsolaris"
	LDLibs += -lgcc_eh
endif

CXXFlags = ${CFlags} -Wno-ctor-dtor-privacy

# ==================================================================================================
# Include paths

Includes = \
		-I${XMPRoot}/public/include \
		-I${XMPRoot} \
		-I${SampleRoot}/build/gcc

# ==================================================================================================
# patterns and rules

vpath %.cpp\
    ${SampleRoot}/source:\
    ${SampleRoot}/source/common:\
    ${SampleRoot}/source/dumpfile:\
    ${SampleRoot}/source/xmpcommand:\
    ${XMPRoot}/source:\
    ${XMPRoot}/public/include:\
    ${XMPRoot}/public/include/client-glue:\


${TempRoot}/%.o : %.c
	@echo ""
	@echo "Compiling $<"
	${Compile} ${CFlags} ${Includes} -c $< -o $@

${TempRoot}/%.o : %.cpp
	@echo ""
	@echo "Compiling $<"
	${Compile} ${CXXFlags} ${Includes} -c $< -o $@

${TargetRoot}/% : ${TempRoot}/%.o ${ExtraObjects}
	@echo ""
	@echo "Linking $@"
	rm -f $@
	${Link} ${LDFlags} $< ${ExtraObjects} ${LDLibs} -o $@
	rm -f ${TempRoot}/*

# ==================================================================================================
# Targets

.PHONY : clean clean_all

Sample : msg create_dirs ${TargetRoot}/${Sample}
	@echo ""

# full re-build (with prior cleaning)
rebuild : clean_all Sample

# lazy build
build : Sample


msg :
ifeq "${Error}" ""
	@echo "----------------------------------------------------------------------------------"
	@echo "Building ${Sample} for ${TargetOS} ${TargetStage} ${TargetArch}"
else
	@echo ""
	@echo "Error: ${Error}"
	@echo ""
	@echo "# To build one of the XMP samples:"
	@echo "#   make -f Sample.mak [os=<os>] [stage=<stage>] [arch=<arch>] name=<sample> target"
	@echo "# where"
	@echo "#   os    = i80386linux | sparcsolaris"
	@echo "#   stage = debug | release"
	@echo "#   arch  = x86 | x64"
	@echo "#   name  = XMPCoreCoverage | XMPFilesCoverage | ReadingXMP | ModifyingXMP | XMPIterations | XMPCommand | DumpFile | DumpMainXMP | DumpScannedXMP | CustomSchema"
	@echo "#   target= clean | build | rebuild"
	@echo "#"
	@echo "# The name argument is the "simple name" of the sample, e.g."
	@echo "# XMPCoreCoverage or DumpMainXMP."
	@echo "#"
	@echo "# The os and stage arguments can also be uppercase, OS and STAGE."
	@echo "# This makefile is only for Linux and Solaris." 
	@echo "# If the OS is omitted it will try to default from the OSTYPE and"
	@echo "# MACHTYPE environment variables. If the stage is omitted it"
	@echo "# defaults to debug."
	@echo ""
	@exit 1
endif

create_dirs :
	@mkdir -p ${TempRoot}
	@mkdir -p ${TargetRoot}

clean : 
	rm -rf ${TargetRoot}/${Sample} ${TempRoot}/${Sample}/*

clean_all : 
	rm -rf ${TargetRoot}/* ${TempRoot}/*
