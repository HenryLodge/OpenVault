## default executable
DEFAULT_EXE=bin/main

## location of cxxtest
CXXTEST_HOME?=extern/cxxtest-4.4


## various options for possible build configurations
## use one of the following sets or add your own

CXXFLAGS_BASE=
LDFLAGS_BASE=

## for optimization
CXXFLAGS_EXTRA_REL:=-O2 -DNDEBUG
LDFLAGS_EXTRA_REL:=

## for gdb or valgrind
CXXFLAGS_EXTRA_DBG:=-Og -ggdb -fno-inline -fno-omit-frame-pointer -D_DEBUG
LDFLAGS_EXTRA_DBG:=

## for gcov
CXXFLAGS_EXTRA_COV:=-Og -ggdb -fno-inline -fno-omit-frame-pointer -D_DEBUG -fprofile-arcs -ftest-coverage
LDFLAGS_EXTRA_COV:=-fprofile-arcs

## for gprof
CXXFLAGS_EXTRA_PRF:=-O2 -pg
LDFLAGS_EXTRA_PRF:=-pg



## the following should not need to change

## generic options
CXXFLAGS_BASE:=$(CXXFLAGS_BASE) -std=c++20 -Wall -Werror -pedantic-errors -Iinclude -Isrc
LDFLAGS_BASE:=$(LDFLAGS_BASE) -std=c++20

## platform-specific options
ifeq ($(OS),Windows_NT)
	CXXFLAGS_BASE:=$(CXXFLAGS_BASE) -I/mingw64/include/SDL2
	LDFLAGS_BASE:=$(LDFLAGS_BASE) -lmingw32 -lSDL2main -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_mixer -lSDL2_net -lSDL2_ttf
	DOXYGEN=doxygen
else ifeq ($(shell sh -c 'uname'),Darwin)
	CXXFLAGS_BASE:=$(CXXFLAGS_BASE) -F/Library/Frameworks -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers -I/Library/Frameworks/SDL2_mixer.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers
	LDFLAGS_BASE:=$(LDFLAGS_BASE) -F/Library/Frameworks -framework SDL2 -framework SDL2_gfx -framework SDL2_image -framework SDL2_mixer -framework SDL2_net -framework SDL2_ttf
	ifneq ($(wildcard /Applications/Doxygen.app/Contents/Resources/doxygen),)
		DOXYGEN=/Applications/Doxygen.app/Contents/Resources/doxygen
	else
		DOXYGEN=doxygen
	endif
else
	ifeq (, $(shell which sdl2-config))
	else
		CXXFLAGS_BASE:=$(CXXFLAGS_BASE) $(shell sh -c 'sdl2-config --cflags')
		LDFLAGS_BASE:=$(LDFLAGS_BASE) $(shell sh -c 'sdl2-config --libs') -lSDL2_gfx -lSDL2_image -lSDL2_mixer -lSDL2_net -lSDL2_ttf
	endif
	DOXYGEN=doxygen
endif

## files
DEFAULT_EXE_DBG=$(addsuffix _dbg,$(DEFAULT_EXE))
DEFAULT_EXE_COV=$(addsuffix _cov,$(DEFAULT_EXE))
DEFAULT_EXE_PRF=$(addsuffix _prf,$(DEFAULT_EXE))
EXECUTABLE_SOURCE_FILES=$(shell sh -c '/usr/bin/find src -name "main*.cpp" 2>/dev/null')
SOURCE_FILES=$(filter-out $(EXECUTABLE_SOURCE_FILES),$(shell sh -c '/usr/bin/find src -name "*.cpp" 2>/dev/null'))
HEADER_FILES=$(shell sh -c '/usr/bin/find include src -name "*.hpp" 2>/dev/null')
OBJECT_FILES_REL=$(SOURCE_FILES:src/%.cpp=build/exe/%.o)
OBJECT_FILES_DBG=$(SOURCE_FILES:src/%.cpp=build/exe/%_dbg.o)
OBJECT_FILES_COV=$(SOURCE_FILES:src/%.cpp=build/exe/%_cov.o)
OBJECT_FILES_PRF=$(SOURCE_FILES:src/%.cpp=build/exe/%_prf.o)
EXECUTABLES_REL=$(patsubst src/%.cpp,bin/%,$(EXECUTABLE_SOURCE_FILES))
EXECUTABLES_DBG=$(patsubst src/%.cpp,bin/%_dbg,$(EXECUTABLE_SOURCE_FILES))
EXECUTABLES_COV=$(patsubst src/%.cpp,bin/%_cov,$(EXECUTABLE_SOURCE_FILES))
EXECUTABLES_PRF=$(patsubst src/%.cpp,bin/%_prf,$(EXECUTABLE_SOURCE_FILES))

## test files
CXXTEST_GEN=$(CXXTEST_HOME)/bin/cxxtestgen
CXXTEST_INCLUDE=$(CXXTEST_HOME)

TEST_FILES=$(shell sh -c '/usr/bin/find test -name "*.cxxtest.hpp"')
TEST_SOURCE_FILES=$(TEST_FILES:test/%.cxxtest.hpp=build/test/%.cxxtest.cpp) build/test/runner.cpp
TEST_OBJECT_FILES_REL=$(TEST_SOURCE_FILES:.cpp=.o)
TEST_OBJECT_FILES_DBG=$(TEST_SOURCE_FILES:.cpp=_dbg.o)
TEST_OBJECT_FILES_COV=$(TEST_SOURCE_FILES:.cpp=_cov.o)
TEST_OBJECT_FILES_PRF=$(TEST_SOURCE_FILES:.cpp=_prf.o)

CXXFLAGS_BASE:=$(CXXFLAGS_BASE) -I$(CXXTEST_INCLUDE)

## for optimization
CXXFLAGS_REL:=$(CXXFLAGS_BASE) $(CXXFLAGS_EXTRA_REL)
LDFLAGS_REL:=$(LDFLAGS_BASE) $(LDFLAGS_EXTRA_REL)

## for gdb or valgrind
CXXFLAGS_DBG:=$(CXXFLAGS_BASE) $(CXXFLAGS_EXTRA_DBG)
LDFLAGS_DBG:=$(LDFLAGS_BASE) $(LDFLAGS_EXTRA_DBG)

## for gcov
CXXFLAGS_COV:=$(CXXFLAGS_BASE) $(CXXFLAGS_EXTRA_COV)
LDFLAGS_COV:=$(LDFLAGS_BASE) $(LDFLAGS_EXTRA_COV)

## for gprof
CXXFLAGS_PRF:=$(CXXFLAGS_BASE) $(CXXFLAGS_EXTRA_PRF)
LDFLAGS_PRF:=$(LDFLAGS_BASE) $(LDFLAGS_EXTRA_PRF)



## rules
.PHONY: all clean doc exe exe_dbg exe_cov exe_prf tests tests_dbg tests_cov tests_prf run run_dbg run_cov run_prf run-tests run-tests_dbg run-tests_cov run-tests_prf

all: doc exe exe_dbg exe_cov exe_prf tests tests_dbg tests_cov tests_prf



ifeq (, $(shell which $(DOXYGEN)))
doc:
	@echo "Cannot find doxygen to build documentation."
	true
else
doc: $(HEADER_FILES) $(SOURCE_FILES) Makefile
	$(DOXYGEN)
endif

exe: $(EXECUTABLES_REL)

exe_dbg: $(EXECUTABLES_DBG)

exe_cov: $(EXECUTABLES_COV)

exe_prf: $(EXECUTABLES_PRF)

tests: bin/test

tests_dbg: bin/test_dbg

tests_cov: bin/test_cov

tests_prf: bin/test_prf



run: $(DEFAULT_EXE)
	$(DEFAULT_EXE)

run_dbg: $(DEFAULT_EXE_DBG)
	$(DEFAULT_EXE_DBG)

run_cov: $(DEFAULT_EXE_COV)
	$(DEFAULT_EXE_COV)

run_prf: $(DEFAULT_EXE_PRF)
	$(DEFAULT_EXE_PRF)

run-tests: bin/test
	bin/test

run-tests_dbg: bin/test_dbg
	bin/test_dbg

run-tests_cov: bin/test_cov
	bin/test_cov

run-tests_prf: bin/test_prf
	bin/test_prf



$(EXECUTABLES_REL): bin/%: build/exe/%.o $(OBJECT_FILES_REL)
	mkdir -p $(dir $@)
	g++  $^ $(LDFLAGS_REL) -o $@

$(EXECUTABLES_DBG): bin/%: build/exe/%.o $(OBJECT_FILES_DBG)
	mkdir -p $(dir $@)
	g++  $^ $(LDFLAGS_DBG) -o $@

$(EXECUTABLES_COV): bin/%: build/exe/%.o $(OBJECT_FILES_COV)
	mkdir -p $(dir $@)
	g++  $^ $(LDFLAGS_COV) -o $@

$(EXECUTABLES_PRF): bin/%: build/exe/%.o $(OBJECT_FILES_PRF)
	mkdir -p $(dir $@)
	g++  $^ $(LDFLAGS_PRF) -o $@

build/exe/%.o: src/%.cpp $(HEADER_FILES) Makefile
	mkdir -p $(dir $@)
	g++ -c $< $(CXXFLAGS_REL) -o $@

build/exe/%_dbg.o: src/%.cpp $(HEADER_FILES) Makefile
	mkdir -p $(dir $@)
	g++ -c $< $(CXXFLAGS_DBG) -o $@

build/exe/%_cov.o: src/%.cpp $(HEADER_FILES) Makefile
	mkdir -p $(dir $@)
	g++ -c $< $(CXXFLAGS_COV) -o $@

build/exe/%_prf.o: src/%.cpp $(HEADER_FILES) Makefile
	mkdir -p $(dir $@)
	g++ -c $< $(CXXFLAGS_PRF) -o $@

ifneq ($(wildcard $(CXXTEST_HOME)),)

bin/test: $(TEST_OBJECT_FILES_REL) $(OBJECT_FILES_REL)
	mkdir -p $(dir $@)
	g++  $^ $(LDFLAGS_REL) -o $@

bin/test_dbg: $(TEST_OBJECT_FILES_DBG) $(OBJECT_FILES_DBG)
	mkdir -p $(dir $@)
	g++  $^ $(LDFLAGS_DBG) -o $@

bin/test_cov: $(TEST_OBJECT_FILES_COV) $(OBJECT_FILES_COV)
	mkdir -p $(dir $@)
	g++  $^ $(LDFLAGS_COV) -o $@

bin/test_prf: $(TEST_OBJECT_FILES_PRF) $(OBJECT_FILES_PRF)
	mkdir -p $(dir $@)
	g++  $^ $(LDFLAGS_PRF) -o $@

build/test/%.o: build/test/%.cpp $(HEADER_FILES) Makefile
	mkdir -p $(dir $@)
	g++ -c $< $(CXXFLAGS_REL) -o $@

build/test/%_dbg.o: build/test/%.cpp $(HEADER_FILES) Makefile
	mkdir -p $(dir $@)
	g++ -c $< $(CXXFLAGS_DBG) -o $@

build/test/%_cov.o: build/test/%.cpp $(HEADER_FILES) Makefile
	mkdir -p $(dir $@)
	g++ -c $< $(CXXFLAGS_COV) -o $@

build/test/%_prf.o: build/test/%.cpp $(HEADER_FILES) Makefile
	mkdir -p $(dir $@)
	g++ -c $< $(CXXFLAGS_PRF) -o $@

.PRECIOUS: build/test/%.cxxtest.cpp
build/test/%.cxxtest.cpp: test/%.cxxtest.hpp
	mkdir -p $(dir $@)
	$(CXXTEST_GEN) --part --error-printer $< -o $@

build/test/runner.cpp:
	mkdir -p $(dir $@)
	$(CXXTEST_GEN) --root --error-printer -o $@

else

bin/test:
	@echo "Cannot find CxxTest to build tests."
	true

bin/test_dbg:
	@echo "Cannot find CxxTest to build tests."
	true

bin/test_cov:
	@echo "Cannot find CxxTest to build tests."
	true

bin/test_prf:
	@echo "Cannot find CxxTest to build tests."
	true

endif

clean:
	rm -rf bin/ build/ doc/ *.gcda *.gcno *.gcov gmon.out
