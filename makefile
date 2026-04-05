
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
                -std=c++11
        
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
                -std=c++11\
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

OBJS = main.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(OBJS))

SERIES1_OBJS = series1.o
SERIES1_OBJ = $(patsubst %,$(OBJDIR)/%,$(SERIES1_OBJS))
SERIES1_OUTPUT = $(BINDIR)/series1

CREATE_SIMPLE_OBJS = create_simple.o
CREATE_SIMPLE_OBJ = $(patsubst %,$(OBJDIR)/%,$(CREATE_SIMPLE_OBJS))
CREATE_SIMPLE_OUTPUT = $(BINDIR)/create_simple

TEST_BSA_OBJS = test_bsa_files.o
TEST_BSA_OBJ = $(patsubst %,$(OBJDIR)/%,$(TEST_BSA_OBJS))
TEST_BSA_OUTPUT = $(BINDIR)/test_bsa_files

TEST_SURFACE_OBJS = test_surface_bsa.o
TEST_SURFACE_OBJ = $(patsubst %,$(OBJDIR)/%,$(TEST_SURFACE_OBJS))
TEST_SURFACE_OUTPUT = $(BINDIR)/test_surface_bsa

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CPPDEFINES)

$(OUTPUT): $(OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(LIBDIRS) $(LIBS)
	@cp $(LIBDIR)/$(SDKFNAME) $(BINDIR)/$(SDKFNAME)
	@cp $(LIBDIR)/$(USDFNAME) $(BINDIR)/$(USDFNAME)
	@cp -r $(LIBDIR)/usd $(BINDIR)/usd

$(SERIES1_OUTPUT): $(SERIES1_OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(LIBDIRS) $(LIBS)
	@cp $(LIBDIR)/$(SDKFNAME) $(BINDIR)/$(SDKFNAME)
	@cp $(LIBDIR)/$(USDFNAME) $(BINDIR)/$(USDFNAME)
	@cp -r $(LIBDIR)/usd $(BINDIR)/usd

$(CREATE_SIMPLE_OUTPUT): $(CREATE_SIMPLE_OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(LIBDIRS) $(LIBS)
	@cp $(LIBDIR)/$(SDKFNAME) $(BINDIR)/$(SDKFNAME)
	@cp $(LIBDIR)/$(USDFNAME) $(BINDIR)/$(USDFNAME)
	@cp -r $(LIBDIR)/usd $(BINDIR)/usd

$(TEST_BSA_OUTPUT): $(TEST_BSA_OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(LIBDIRS) $(LIBS)
	@cp $(LIBDIR)/$(SDKFNAME) $(BINDIR)/$(SDKFNAME)
	@cp $(LIBDIR)/$(USDFNAME) $(BINDIR)/$(USDFNAME)
	@cp -r $(LIBDIR)/usd $(BINDIR)/usd

$(TEST_SURFACE_OUTPUT): $(TEST_SURFACE_OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(LIBDIRS) $(LIBS)
	@cp $(LIBDIR)/$(SDKFNAME) $(BINDIR)/$(SDKFNAME)
	@cp $(LIBDIR)/$(USDFNAME) $(BINDIR)/$(USDFNAME)
	@cp -r $(LIBDIR)/usd $(BINDIR)/usd

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(OUTPUT)
	rm -f $(SERIES1_OUTPUT)
	rm -f $(CREATE_SIMPLE_OUTPUT)
	rm -f $(TEST_BSA_OUTPUT)
	rm -f $(BINDIR)/$(SDKFNAME)
	rm -f $(BINDIR)/$(USDFNAME)
	rm -rf $(BINDIR)/usd

.PHONY: series1
series1: $(SERIES1_OUTPUT)

.PHONY: create_simple
create_simple: $(CREATE_SIMPLE_OUTPUT)

.PHONY: test_bsa
test_bsa: $(TEST_BSA_OUTPUT)

.PHONY: test_surface
test_surface: $(TEST_SURFACE_OUTPUT)
