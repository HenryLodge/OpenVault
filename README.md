# cs3520-proj

## Instructions

Will Usher's SDL 2.0 Tutorial can be found here: https://www.willusher.io/pages/sdl2/.

This starter code has done Lesson 0.

The Makefile is set up to build all `.cpp` files in the `src` folder and create executables in `bin/main`.  To just build the release executable, run `make exe`.

General structure:
 * src: for source (.cpp) files, everything here will get compiled/linked (src/main.cpp provided)
 * test: for test source code (using CxxText, expected to be in extern/cxxtest-4.4)
 * include: for header (.hpp) files, looked in for #include
 * res: for resources (e.g. images)
 * build: where created intermediate (e.g. object) files go
 * bin: where created executables go
 * doc: where generated documentation (Doxygen) goes

Configurations:
 * rel: for speed
 * deb: for debugging
 * cov: for code coverage (with gcov)
 * prf: for performance analysis (with gprof)
