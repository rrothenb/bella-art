
SDKNAME       = bella_scene_sdk
OUTNAME       = console
UNAME         = $(shell uname)

ifeq ($(UNAME), Darwin)

  SDKBASE     = .
  SOEXT       = .dylib
  SDKFNAME    = lib$(SDKNAME)$(SOEXT)
  USDFNAME    = libdl_usd_ms$(SOEXT)
  INCLUDEDIRS = -I$(SDKBASE)/src
  LIBDIR      = $(SDKBASE)/lib
  LIBDIRS     = -L$(LIBDIR)
  OBJDIR      = obj/$(UNAME)
  BINDIR      = bin/$(UNAME)
  OUTPUT      = $(BINDIR)/$(OUTNAME)

  ISYSROOT    = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk

  CC          = clang
  CXX         = clang++

  CCFLAGS     = -arch x86_64\
                -arch arm64\
                -mmacosx-version-min=11.0\
                -isysroot $(ISYSROOT)\
                -fvisibility=hidden\
                -O3\
                $(INCLUDEDIRS)

  CFLAGS      = $(CCFLAGS)\
                -std=c11

  CXXFLAGS    = $(CCFLAGS)\
                -std=c++17

  CPPDEFINES  = -DNDEBUG=1\
                -DDL_USE_SHARED

  LIBS        = -l$(SDKNAME)\
                -dl_usd_ms\
                -lm\
                -ldl

  LINKFLAGS   = -mmacosx-version-min=11.0\
                -isysroot $(ISYSROOT)\
                -framework Cocoa\
                -framework IOKit\
                -fvisibility=hidden\
                -O5\
                -rpath @executable_path
else

  SDKBASE     = .
  SOEXT       = .so
  SDKFNAME    = lib$(SDKNAME)$(SOEXT)
  USDFNAME    = libdl_usd_ms$(SOEXT)
  INCLUDEDIRS = -I$(SDKBASE)/src
  LIBDIR      = $(SDKBASE)/lib
  LIBDIRS     = -L$(LIBDIR)
  OBJDIR      = obj/$(UNAME)
  BINDIR      = bin/$(UNAME)
  OUTPUT      = $(BINDIR)/$(OUTNAME)

  CC          = gcc
  CXX         = g++

  CCFLAGS     = -m64\
                -Wall\
                -Werror\
                -fvisibility=hidden\
                -D_FILE_OFFSET_BITS=64\
                -O3\
                $(INCLUDEDIRS)

  CFLAGS      = $(CCFLAGS)\
                -std=c11

  CXXFLAGS    = $(CCFLAGS)\
                -std=c++17\
                -Wno-class-conversion

  CPPDEFINES  = -DNDEBUG=1\
                -DDL_USE_SHARED

  LIBS        = -l$(SDKNAME)\
                -dl_usd_ms\
                -lm\
                -ldl\
                -lrt\
                -lpthread

  LINKFLAGS   = -m64\
                -fvisibility=hidden\
                -O3\
                -Wl,-rpath,'$$ORIGIN'\
                -Wl,-rpath,'$$ORIGIN/lib'\
                -Wl,-rpath-link=./lib
endif

# Add new series numbers here — one entry per series
SERIES = 1 2 3 4 5

SERIES_OUTPUTS       = $(addprefix $(BINDIR)/series, $(SERIES))
CREATE_SIMPLE_OUTPUT = $(BINDIR)/create_simple
TEST_BSA_OUTPUT      = $(BINDIR)/test_bsa_files
TEST_SURFACE_OUTPUT  = $(BINDIR)/test_surface_bsa

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CPPDEFINES)

# Generic link rule — covers all series and other single-source binaries
$(BINDIR)/%: $(OBJDIR)/%.o
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(LIBDIRS) $(LIBS)
	@cp $(LIBDIR)/$(SDKFNAME) $(BINDIR)/$(SDKFNAME)
	@cp $(LIBDIR)/$(USDFNAME) $(BINDIR)/$(USDFNAME)
	@cp -r $(LIBDIR)/usd $(BINDIR)/usd

# console binary has a different name than its source (main.cpp)
$(OUTPUT): $(OBJDIR)/main.o
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(LIBDIRS) $(LIBS)
	@cp $(LIBDIR)/$(SDKFNAME) $(BINDIR)/$(SDKFNAME)
	@cp $(LIBDIR)/$(USDFNAME) $(BINDIR)/$(USDFNAME)
	@cp -r $(LIBDIR)/usd $(BINDIR)/usd

.PHONY: clean $(addprefix series, $(SERIES)) create_simple test_bsa test_surface

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(OUTPUT) $(SERIES_OUTPUTS) $(CREATE_SIMPLE_OUTPUT) $(TEST_BSA_OUTPUT) $(TEST_SURFACE_OUTPUT)
	rm -f $(BINDIR)/$(SDKFNAME)
	rm -f $(BINDIR)/$(USDFNAME)
	rm -rf $(BINDIR)/usd

$(addprefix series, $(SERIES)): series%: $(BINDIR)/series%

create_simple: $(CREATE_SIMPLE_OUTPUT)

test_bsa: $(TEST_BSA_OUTPUT)

test_surface: $(TEST_SURFACE_OUTPUT)
