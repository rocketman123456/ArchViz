# option(BUILD_TOOLS "Build tools in the src directory (zipcmp, zipmerge, ziptool)" OFF)
# option(BUILD_REGRESS "Build regression tests" OFF)
# option(BUILD_EXAMPLES "Build examples" OFF)
# option(BUILD_DOC "Build documentation" OFF)
# option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

SET(BUILD_TOOLS OFF CACHE BOOL "Build tools in the src directory (zipcmp, zipmerge, ziptool)" FORCE)
SET(BUILD_REGRESS OFF CACHE BOOL "Build regression tests" FORCE)
SET(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
SET(BUILD_DOC OFF CACHE BOOL "Build documentation" FORCE)
SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

add_subdirectory(libzip)
