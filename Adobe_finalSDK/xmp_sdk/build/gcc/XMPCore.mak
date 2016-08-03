# ==================================================================================================
# Copyright 2008 Adobe Systems Incorporated
# All Rights Reserved.
#
# NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# ==================================================================================================
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

OSVer =
ifeq "${TargetOS}" "i80386linux"
	OSVer = linux2.6
endif
ifeq "${TargetOS}" "sparcsolaris"
	OSVer = solaris9
endif
ifeq "${TargetOS}" "rs6000aix"
        OSVer = aix5.2
endif
ifeq "${TargetOS}" "hppahpux"
        OSVer = hp11.11
endif

ifeq "${OSVer}" ""
	Error = Undefined OSVer
endif

# ensure libpath is set:
LibraryPath = ${libpath}
ifeq "${libpath}" ""
	Error += Unspecified library path. Must set libpath=...
endif

# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------
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

ifeq "${TargetArch}" "x64"
	x64_ext = _x64
endif

LibRoot    = ${XMPRoot}/public/libraries/${TargetOS}${x64_ext}/${TargetStage}
TempRoot   = ${XMPRoot}/temp/XMPCore/${TargetOS}${x64_ext}/${TargetStage}
TargetRoot = ${XMPRoot}/target/${TargetOS}${x64_ext}/${TargetStage}/bin

LibName = ${LibRoot}/staticXMPCore${x64_ext}.ar
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

XMPDefines = -DUNIX_ENV=1 -DHAVE_EXPAT_CONFIG_H=1 -DXML_STATIC=1 ${ModeOpt}

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
   -I${XMPRoot}/build \
   -I${XMPRoot}/build/gcc

# --------------------------------
# Define settings specific to gcc.

CPPFlags = -x c++ -Wno-ctor-dtor-privacy

CompileFlags += -fPIC -funsigned-char -fexceptions -D_FILE_OFFSET_BITS=64
CompileFlags += -Wno-multichar -Wno-implicit

LinkFlags   = -shared -fPIC -z defs -Wl,-z,relro -Wl,-z,now -Xlinker -Bsymbolic -Xlinker --version-script -Xlinker XMPCore.exp

RuntimeLibs = -lc -lm -lpthread -L${LibraryPath} -Bdynamic -lstdc++ -shared-libgcc

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
	LinkFlags     = -shared -fPIC -Xlinker -Bsymbolic -Xlinker -c -Xlinker XMPCore-acc.exp
	RuntimeLibs   = -lc -lm -L${ToolPath}/lib -Bdynamic -lstdc++ -shared-libgcc
endif		

ifeq "${TargetOS}" "rs6000aix"
	CompileFlags += -D_LARGE_FILE_API=1 -D_REENTRANT -pthread
	LinkFlags     = -shared -fPIC -Bsymbolic -Xlinker -bE:XMPCore-vacpp.exp -pthread
	RuntimeLibs  += -Xlinker -brtl
endif

# ==================================================================================================
# Define the set of source files.

vpath %.incl_cpp\
    ${XMPRoot}/public/include:\
    ${XMPRoot}/public/include/client-glue:\

vpath %.cpp\
    ${XMPRoot}/XMPCore/source:\
    ${XMPRoot}/source:\
    ${XMPRoot}/public/include:\
    ${XMPRoot}/public/include/client-glue:\
    ${XMPRoot}/third-party/zuid/interfaces

vpath %.c\
    ${XMPRoot}/third-party/expat/lib

CSources = \
    xmlparse.c \
    xmlrole.c \
    xmltok.c

CPPSources = \
    XMPMeta.cpp \
    XMPMeta-GetSet.cpp \
    XMPMeta-Parse.cpp \
    XMPMeta-Serialize.cpp \
    XMPIterator.cpp \
    XMPUtils.cpp \
    XMPUtils-FileInfo.cpp \
    XMPCore_Impl.cpp \
    WXMPMeta.cpp \
    WXMPIterator.cpp \
    WXMPUtils.cpp \
    ExpatAdapter.cpp \
    XML_Node.cpp \
    ParseRDF.cpp \
   	UnicodeConversions.cpp \
		XMP_LibUtils.cpp \
    MD5.cpp \

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
	@echo "TargetPaths= ${TargetPaths}"
	
	@echo x64_ext = ${x64_ext}
	@echo CompileFlags = ${CompileFlags}

	rm -f $@
	${Compile} ${CompileFlags} ${XMPDefines} ${Includes} -c $< -o $@
	@touch $@

${TempRoot}/%.o : %.cpp
	@echo "------------------------------"
	@echo "source= $<"
	@echo "target= $@"
	@echo "TargetPaths= ${TargetPaths}"

	@echo x64_ext = ${x64_ext}
	@echo CompileFlags = ${CompileFlags}

	rm -f $@
	${Compile} ${CPPFlags} ${CompileFlags} ${XMPDefines} ${Includes} -c $< -o $@
	@touch $@

# ==================================================================================================

.PHONY: all rebuild msg create_dirs

# full re-build (with prior cleaning)
all : msg clean create_dirs cleanTemp ${LibName} listOutput

# currently a forward, soon to be the real thing ('all' only outputting usage info)
rebuild: all

# lazy build
build: msg create_dirs ${LibName} listOutput
	@echo "lazy build done."

msg :
ifeq "${Error}" ""
	@echo "----------------------------------------------------------------------------------"
	@echo Building XMPCore for ${TargetOS} ${TargetStage} --------------------
else
	@echo ""
	@echo "Error: ${Error}"
	@echo ""
	@echo "# To build the Adobe XMP Core DLL:"
	@echo "#   make -f XMPCore.mak [os=<os>] [stage=<stage>] [arch=<arch>]"
	@echo "# where"
	@echo "#   os    = i80386linux | sparcsolaris | rs6000aix | hppahpux"
	@echo "#   stage = debug | release"
	@echo "#   arch  = x86 | x64  (just say x86 for anything 32 bit)"
	@echo "#"
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
	@echo $
	@echo "output directory: ${LibRoot}"
	@ls -l ${LibRoot}


