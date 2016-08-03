# ==================================================================================================
# Copyright 2011 Adobe Systems Incorporated
# All Rights Reserved.
#
# NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
# of the Adobe license agreement accompanying it.
# ==================================================================================================

# -----------------------------------------------
# All of the real processing is in a common file.

PluginName = PluginTemplate

CPPSources = \
	Template_Handler.cpp \
	Host_IO-UNIX.cpp \
	XMPFiles_IO.cpp \
	XIO.cpp

include PluginCommon.mak

