#
# Makefile for C++ NISE-SPR project.
# @author Guilherme Oliveira Chagas (guilherme.o.chagas[a]gmail.com)
# @date 21/07/2019

CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Werror -fopenmp -DIL_STD
LDFLAGS = -s
RM = rm -rf

# -- OpenMP and filesystem libraries linker flags --
LDLIBS = -fopenmp -lstdc++fs

SRC_DIR = ./source
HDR_DIR = ./headers
OBJ_DIR = ./obj
OUT_BIN = nise_spr

SRCS = $(shell find . -type f -name '*.cpp')

OBJ = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .cpp,.o,$(SRCS))))

all: $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o $(OUT_BIN) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	$(RM) $(OBJ) $(OUT_BIN)

.PHONY: clean