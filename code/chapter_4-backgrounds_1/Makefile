# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= c5_1_backgrounds
GAME_TITLE	:= Chapter 5.1
GAME_SUBTITLE	:= Backgrounds
GAME_AUTHOR	:= www.patater.com

# Source code paths
# -----------------

SOURCEDIRS	:= source
INCLUDEDIRS	:=
GFXDIRS		:= gfx
BINDIRS		:=
AUDIODIRS	:=

# Libraries
# ---------

LIBS		:= -lmm9 -lnds9
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod \
			   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
