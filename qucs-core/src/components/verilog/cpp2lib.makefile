# Makefile used to compile ADMS C++ output into a dynamic loadable library

# PREFIX can be passed as parameter to make:
# $make -f cpp2lib.make PREFIX='/qucs/prefix/'

# TODO
# * How to make sure all gets recompiled if the .VA file changes?
# * use -O3 by default?
# * set correct flags in each platform
# * check library linker flags

######################
## Makefile Parameters
######################

# Module name
MODEL=

# Project dir
PROJDIR=

# Please define
PREFIX=

# Installed headers path
INC=$(PREFIX)/include/qucs-core

ifeq ($(OS),Windows_NT)
  #FLAGS=
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
    DLLEXT=.so
    CXX=g++
    FLAGS=-fPIC -shared -rdynamic  -Wl,-soname,$(MODEL)$(DLLEXT)
    #-Wl,-headerpad_max_install_names
  endif

  ifeq ($(UNAME_S),Darwin)
    DLLEXT=.dylib
    CXX=clang++ -stdlib=libc++
    FLAGS=-dynamiclib -flat_namespace -undefined dynamic_lookup -Wl,-headerpad_max_install_names
  endif
endif

# Definition and include paths
CPPFLAGS= -DHAVE_CONFIG_H -I./  -I$(INC)

# Link to system libraries
LIBSRUNTIME=-lqucs -ldl
LDFLAGS=-L"$(PREFIX)/lib" $(LIBSRUNTIME) -Wl,-rpath -Wl,$(PROJDIR)

# Required sources
SRC=$(MODEL).core.cpp $(MODEL).analogfunction.cpp

# Default
.PHONY: cpp2lib

# Main target
cpp2lib: $(MODEL)$(DLLEXT)

# Build library
$(MODEL)$(DLLEXT): $(SRC)
	$(CXX)  $(CPPFLAGS) -o $(MODEL)$(DLLEXT) $(SRC) $(FLAGS) -L"$(PROJDIR)" $(LDFLAGS)

clean:
	@rm -f $(MODEL)$(DLLEXT)

#bundle:
#	@tar -cvf $(MODEL)_Files.tar $(GEN_SRC)

