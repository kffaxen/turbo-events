# turbo-events
A fast and extensible event generator.

# Building
The Ubuntu packages required for building can be found on
the line with "apt-get install" in .github/workflows/adapted-cmake.yml.

The Python bindings require pybind 2.6.2 which is more
recent than the pybin11-dev package in Ubuntu 20.04 LTS.

## Using Valgrind or Address Sanitizer on turbo-events
The malloc routines in the embedded Python interpreter triggers
a lot of warnings. Set the environment variable PYTHONMALLOC=malloc
to instruct Python to use the regular malloc.

## Setting ASAN\_SYMBOLIZER\_PATH gives an error message about invalid symbolizer
This is an
[LLVM bug](https://bugs.llvm.org/show_bug.cgi?id=39514)
that is fixed in Clang 13.

On Ubuntu, the symbolizers are available in
/usr/lib/llvm-X/bin/llvm-symbolizer so you can set your
ASAN\_SYMBOLIZER\_PATH to that as workaround.
