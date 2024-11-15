# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= c9_sounds
GAME_TITLE	:= Chapter 9
GAME_SUBTITLE	:= Sounds
GAME_AUTHOR	:= www.patater.com

# Source code paths
# -----------------

SOURCEDIRS	:= source
INCLUDEDIRS	:= include
GFXDIRS		:= gfx
BINDIRS		:=
AUDIODIRS	:= audio

# Libraries
# ---------

LIBS		:= -lmm9 -lnds9
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod \
			   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
