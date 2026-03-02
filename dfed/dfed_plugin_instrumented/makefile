#
# This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
# Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
# Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
#

# Inspired by https://stackoverflow.com/questions/27001837/makefile-to-compile-all-cpp-files-in-all-subdirs 

# Toolchain selection, with the needed flags
CXX = g++-10
CXXFLAGS = -fPIC -shared -fno-rtti -g -std=gnu++14 -w 

# Include folders
PATH_PLUGIN_HEADERS = ./pluginHeaders_armNoneEabiGcc7
INCLUDE_1 = .
INCLUDE_2 = ./Asm
INCLUDE_3 = ./DFED_Techniques
INCLUDE_4 = ./Printer
INCLUDE_5 = ./Targets
INCLUDE_6 = ./SupportClasses

INCLUDE_COMMAND = -I$(PATH_PLUGIN_HEADERS) -I$(INCLUDE_1) -I$(INCLUDE_2) -I$(INCLUDE_3) -I$(INCLUDE_4) -I$(INCLUDE_5) -I$(INCLUDE_6)

SRCDIR := .
OBJDIR := ./objects

SRCS := $(shell find $(SRCDIR) -name "*.cpp")
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

DFED_plugin64.so: $(OBJS)
	@echo "Linking $@"
	@$(CXX) $(CXXFLAGS) -m64 $^ -o $@
	@echo "DONE"
	
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR)/$(dir $<)
	@$(CXX) $(INCLUDE_COMMAND) $(CXXFLAGS) -c -m64 $< -o $@ 
	
clean:
	@echo "Deleting previous build"
	@rm -rf $(OBJDIR) DFED_plugin64.so
