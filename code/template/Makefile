# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= template
GAME_TITLE	:= Template
GAME_SUBTITLE	:= Patater
GAME_AUTHOR	:= www.patater.com

# Source code paths
# -----------------

SOURCEDIRS	:= source
INCLUDEDIRS	:=
GFXDIRS		:=
BINDIRS		:=
AUDIODIRS	:=

# Libraries
# ---------

LIBS		:= -lmm9 -lnds9
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod \
			   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
