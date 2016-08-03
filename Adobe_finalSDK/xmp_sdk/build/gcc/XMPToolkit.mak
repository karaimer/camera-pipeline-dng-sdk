# ========================================================================================
# Copyright 2008 Adobe Systems Incorporated
# All Rights Reserved.
#
# NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance
# with the terms of the Adobe license agreement accompanying it.
# =======================================================================================
Error =

TargetStage = ${stage}
ifneq "${TargetStage}" "debug"
	ifneq "${TargetStage}" "release"
		Error += Invalid target stage "${TargetStage}"
	endif
endif

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

# =============================================================================================
XMPRoot  = ../..
# =============================================================================================
.PHONY: usage_advice build rebuild msg

all : msg specify_target usage_advice

# NB: pre-emtive msg target is used to output any errors encountered in argument parsing
build : msg
	make -f XMPCore.mak os=${TargetOS} arch=${TargetArch} \
			stage=${TargetStage} libpath=${LibraryPath} build
	make -f XMPFiles.mak os=${TargetOS} arch=${TargetArch} \
			stage=${TargetStage} libpath=${LibraryPath} build
	make -f XMPToolkit.mak os=${TargetOS} arch=${TargetArch} stage=${TargetStage} \
			listOutput

rebuild : msg
	make -f XMPCore.mak os=${TargetOS} arch=${TargetArch} \
			stage=${TargetStage} libpath=${LibraryPath} rebuild
	make -f XMPFiles.mak os=${TargetOS} arch=${TargetArch} \
			stage=${TargetStage} libpath=${LibraryPath} rebuild
	make -f XMPToolkit.mak os=${TargetOS} arch=${TargetArch} stage=${TargetStage} \
			listOutput

clean: msg
	make -C ${XMPRoot}/build/gcc4 -f XMPCore.mak os=${TargetOS} arch=${TargetArch} \
			stage=${TargetStage} libpath=${LibraryPath} clean
	make -C ${XMPRoot}/build/gcc4 -f XMPFiles.mak os=${TargetOS} arch=${TargetArch} \
			stage=${TargetStage} libpath=${LibraryPath} clean
	make -f XMPToolkit.mak os=${TargetOS} arch=${TargetArch} stage=${TargetStage} \
			libpath=${LibraryPath} listOutput
	
msg :
	@echo ""
ifneq "${Error}" ""
	@echo ""
	@echo "Error: ${Error}"
	@echo ""
	@echo "make -f XMPToolkit.mak usage_advice for usage details"
	@exit 1
endif

specify_target :
	@echo "please specify target."
	@echo ""

usage_advice:
	@echo "# To build XMPToolkit (XMPCore+XMPFiles):"
	@echo "#   make -f XMPToolkit.mak os=<os> stage=<stage> arch=<arch> libpath=<path> (build|clean|rebuild|listOutput)"
	@echo "# "
	@echo "#   os    	= i80386linux | sparcsolaris | rs6000aix | hppahpux"
	@echo "#   stage 	= debug | release"
	@echo "#   arch  	= x86 | x64  (just say x86 for anything 32 bit)"
	@echo "#	 libpath= Ensure path contains all the paths to your gcc compiler of choice."
	@echo "#            You MUST set libpath to the directory where the matching libstdc++.a"
	@echo "#            resides (directory name w/o trailing slash, i.e. /usr/lib/gcc/4.1.2/lib)"
	@echo ""
	@exit 0

#Output lists same directory for Core and Files, hence only call on
listOutput:
	@echo ""
	@make -f XMPFiles.mak os=${TargetOS} arch=${TargetArch} \
		stage=${TargetStage} listOutput
	@echo ""
