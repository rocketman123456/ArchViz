option(BUILD_TOOLS "Build tools in the src directory (zipcmp, zipmerge, ziptool)" OFF)
option(BUILD_REGRESS "Build regression tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_DOC "Build documentation" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

option(ENABLE_BZIP2 "Enable use of BZip2" OFF)
option(ENABLE_LZMA "Enable use of LZMA" OFF)
option(ENABLE_ZSTD "Enable use of Zstandard" OFF)

# SET(BUILD_TOOLS OFF CACHE BOOL "Build tools in the src directory (zipcmp, zipmerge, ziptool)" FORCE)
# SET(BUILD_REGRESS OFF CACHE BOOL "Build regression tests" FORCE)
# SET(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
# SET(BUILD_DOC OFF CACHE BOOL "Build documentation" FORCE)
# SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# SET(ENABLE_BZIP2 OFF CACHE BOOL "Enable use of BZip2" FORCE)
# SET(ENABLE_LZMA OFF CACHE BOOL "Enable use of LZMA" FORCE)
# SET(ENABLE_ZSTD OFF CACHE BOOL "Enable use of Zstandard" FORCE)

add_subdirectory(libzip)
