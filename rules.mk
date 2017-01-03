# Copyright (C) 2016 b20yang 
# ---
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.

# This program is distributed in the hope that it will be useful,but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

# You should have received a copy of the GNU Lesser General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.
################################################################################
CC=gcc
AR=ar
RM=rm
CPP=g++

CFLAGS_GLOBAL = -std=gnu99 
CPPFLAGS_GLOBAL = -std=gnu99 
LDFLAGS_GLOBAL = -ldl -lrt

INC_PATH = ../inc
SRC_PATH = ../src
LIB_PATH = ../lib
LIB_SMLD_HOOK = $(LIB_PATH)/libsmldhook.so
LIB_SMLD = $(LIB_PATH)/libsmld.a
