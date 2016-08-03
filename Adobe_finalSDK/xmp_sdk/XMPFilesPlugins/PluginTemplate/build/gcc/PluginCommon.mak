# ==================================================================================================
# Copyright 2011 Adobe Systems Incorporated
# All Rights Reserved.
#
# NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# ==================================================================================================

# --------------------------------------------------------------------
# Make sure the stage (debug/release) and mode (dll/static) are known.

Error =

TargetOS = ${os}
ifneq "${TargetOS}" "i80386linux"
	Error += Invalid target OS "${TargetOS}"
endif

TargetStage = ${stage}
ifneq "${TargetStage}" "debug"
	ifneq "${TargetStage}" "release"
		Error += Invalid target stage "${TargetStage}"
	endif
endif

TargetMode = ${mode}
ifneq "${TargetMode}" "dll"
	ifneq "${TargetMode}" "static"
		Error += Invalid target mode "${TargetMode}"
	endif
endif

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

# ------------------------------------------------------------
# Define variables for input and output directories and files.

ProjectRoot = ../..
PluginsRoot = ../../..
ToolkitRoot = ../../../..

x64_ext =
ifeq "${TargetArch}" "x64"
	x64_ext = _x64
endif

LibRoot    = ${ToolkitRoot}/public/libraries/${TargetOS}${x64_ext}/${TargetStage}
TempRoot   = ${PluginsRoot}/temp/${PluginName}/${TargetOS}${x64_ext}/${TargetStage}/${TargetMode}
TargetRoot = ${PluginsRoot}/public/${TargetOS}${x64_ext}/${TargetStage}

ResourceSource = ${ProjectRoot}/build/resource/txt
ResourceDest   = ${TargetRoot}/${PluginName}.resources

ifeq "${TargetMode}" "dll"
	LibName = ${TargetRoot}/${PluginName}.xpi
else
	LibName = ${TargetRoot}/static${PluginName}${x64_ext}.ar
	ModeOpt = -DXMP_StaticBuild=1
endif

# ==================================================================================================

# ------------------------------------------
# Define compile and link tools and options.
ToolName = gcc
LibPathVar = LD_LIBRARY_PATH
ifeq "${TargetOS}" "hppahpux"
	LibPathVar = SHLIB_PATH
endif

ToolPath = ${PATH}
Compile = PATH=${ToolPath} ${ToolName}
Link    = PATH=${ToolPath} ${LibPathVar}=${LibraryPath} ${ToolName}

# Do something innocuous by default for Strip.
Strip = ls -l

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

CPPFlags =

XMPDefines = -DUNIX_ENV=1 -DAdobePrivate=1 -DHAVE_EXPAT_CONFIG_H=1 -DXML_STATIC=1 ${ModeOpt}

ifeq "${TargetArch}" "x64"
	XMPDefines += -DXMP_64=1 -D__x86_64__=1
endif

DashG = -g

ifeq "${TargetStage}" "debug"
	CompileFlags = ${DashG} -O0 -DDEBUG=1 -D_DEBUG=1
endif

OptFlags = -O2 -Os

ifeq "${TargetStage}" "release"
	CompileFlags = ${OptFlags} -DNDEBUG=1
endif

Includes += \
   -I${ToolkitRoot} \
   -I${ToolkitRoot}/public/include \
   -I${PluginsRoot}/api/source \
   -I${ProjectRoot}/source

# --------------------------------
# Define settings specific to gcc.

CPPFlags = -x c++ -Wno-ctor-dtor-privacy

CompileFlags += -fPIC -funsigned-char -fexceptions -D_FILE_OFFSET_BITS=64
CompileFlags += -Wno-multichar -Wno-implicit

LinkFlags = -shared -z defs
#LinkFlags  = -shared -fPIC -z defs -Wl,-z,relro -Wl,-z,now
#LinkFlags += -Xlinker -Bsymbolic -Xlinker --version-script -Xlinker XMPCore.ex

RuntimeLibs += -lc -lm -lpthread -L${LibPath} -Bdynamic -lstdc++ -shared-libgcc 

ifeq "${TargetOS}" "i80386linux"
	ifeq "${TargetArch}" "x86"
		CompileFlags += -m32 -mtune=i686
		LinkFlags    += -m32
	else
		CompileFlags += -m64
		LinkFlags    += -m64
	endif
	LinkFlags += -Wl,-rpath,'$$ORIGIN'
endif

# ==================================================================================================
# Define the set of source files.

vpath %.incl_cpp \
    ${ToolkitRoot}/public/include: \
    ${ToolkitRoot}/public/include/client-glue:

vpath %.c \
    ${ProjectRoot}/source:

vpath %.cpp \
    ${PluginsRoot}/api/source: \
    ${ProjectRoot}/source: \
    ${ToolkitRoot}/source:

CPPSources += \
	HostAPIAccess.cpp \
	PluginAPIImpl.cpp \
	PluginBase.cpp \
	PluginRegistry.cpp

CObjs = $(foreach objs,${CSources:.c=.o},${TempRoot}/$(objs))
CPPObjs = $(foreach objs,${CPPSources:.cpp=.o},${TempRoot}/$(objs))

.SUFFIXES:                # Delete the default suffixes
.SUFFIXES: .o .c .cpp     # Define our suffix list

# ==================================================================================================

# ---------------------------------------
# Define the generic compile build rules.

${TempRoot}/%.o : %.c
	@echo ""
	@echo "source = $<"
	@echo "target = $@"
	rm -f $@
	${Compile} ${CPPFlags} ${CompileFlags} ${XMPDefines} ${Includes} -c $< -o $@
	@touch $@

${TempRoot}/%.o : %.cpp
	@echo ""
	@echo "source = $<"
	@echo "target = $@"
	rm -f $@
	${Compile} ${CPPFlags} ${CompileFlags} ${XMPDefines} ${Includes} -c $< -o $@
	@touch $@

# ==================================================================================================

.PHONY: all rebuild msg create_dirs

all : msg clean create_dirs cleanTemp ${LibName}

rebuild: all

build: msg create_dirs ${LibName}
	@echo "Lazy build done."

msg :
ifeq "${Error}" ""
	@echo ""
	@echo Building ${PluginName} for ${TargetOS}-${TargetArch} ${TargetMode} ${TargetStage}
	@echo ""
else
	@echo ""
	@echo "Error: ${Error}"
	@echo ""
	@echo "# To build the Adobe ${PluginName} DLL:"
	@echo "#   make -f ${PluginName}.mak [os=<os>] [stage=<stage>] [mode=<mode>] [arch=<arch>] "
	@echo "# where"
	@echo "#   os    = i80386linux"
	@echo "#   stage = debug | release"
	@echo "#   mode  = static | dll"
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
	@mkdir -p ${TempRoot} ${TargetRoot} ${ResourceDest}

${LibName} : ${CObjs} ${CPPObjs}
	@echo ""
	@echo "Linking $@"
	rm -f $@
	rm -f ${ResourceDest}/*
	${Link} ${LinkFlags} $? ${LibRoot}/staticXMPCore${x64_ext}.ar ${RuntimeLibs} -o $@
	${Strip} $@
	@touch $@
	cp -f ${ResourceSource}/MODULE_IDENTIFIER.txt ${ResourceDest}/MODULE_IDENTIFIER.txt
ifeq "${TargetArch}" "x86"
	cp -f ${ResourceSource}/XMPPLUGINUIDS-32.txt ${ResourceDest}/XMPPLUGINUIDS.txt
else
	cp -f ${ResourceSource}/XMPPLUGINUIDS-64.txt ${ResourceDest}/XMPPLUGINUIDS.txt
endif
	@echo ""
	@echo ${PluginName} built for ${TargetOS}-${TargetArch} ${TargetMode} ${TargetStage}
	@echo ""
	@exit 0

clean: cleanTemp cleanLib

cleanTemp:
	rm -rf ${TempRoot}/*

cleanLib:
	rm -f  ${LibName}
	rm -rf ${ResourceDest}
	rm -rf ${ProjectRoot}/build/common-unix/intermediate/${TargetOS}/
