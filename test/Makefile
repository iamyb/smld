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
include ../rules.mk

CFLAGS_LOCAL = -I$(INC_PATH) 
CFLAGS_LOCAL+= -I$(SRC_PATH)

CPPFLAGS = -I$(INC_PATH) 
CPPFLAGS+= -I$(SRC_PATH)

TEST_SMLD_SRC=test_smld.c 
TEST_SMLD_OBJ=$(TEST_SMLD_SRC:.c=.o)
TEST_SMLD_TGT=$(TEST_SMLD_SRC:.c=.out)

TEST_SMLD_CPP_SRC=test_smld.cpp
TEST_SMLD_CPP_OBJ=$(TEST_SMLD_CPP_SRC:.cpp=cpp.o)
TEST_SMLD_CPP_TGT=$(TEST_SMLD_CPP_SRC:.cpp=cpp.out)

LDFLAGS_LOCAL=-lsmld -lpthread
LDFLAGS =$(LDFLAGS_LOCAL)
LDFLAGS+=$(LDFLAGS_GLOBAL)

all: $(TEST_SMLD_TGT) $(TEST_SMLD_CPP_TGT)

#.PHONY:$(TEST_SMLD_TGT) $(TEST_SMLD_CPP_TGT)

$(TEST_SMLD_TGT): $(LIB_SMLD) $(TEST_SMLD_OBJ)
	$(CC) -o $@ $(TEST_SMLD_OBJ) -L$(LIB_PATH) $(LDFLAGS) 

$(TEST_SMLD_OBJ): $(TEST_SMLD_SRC)  
	$(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -c $? -o $@  

$(TEST_SMLD_CPP_TGT): $(LIB_SMLD) $(TEST_SMLD_CPP_OBJ)
	$(CPP) -o $@ $(TEST_SMLD_CPP_OBJ) -L$(LIB_PATH) $(LDFLAGS)  

$(TEST_SMLD_CPP_OBJ): $(TEST_SMLD_CPP_SRC)
	$(CPP) $(CPPFLAGS) -c $? -o $@   

clean:
	$(RM) $(TEST_SMLD_OBJ) $(TEST_SMLD_CPP_OBJ)
	$(RM) $(TEST_SMLD_TGT) $(TEST_SMLD_CPP_TGT)
