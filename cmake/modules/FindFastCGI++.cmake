# - Try to find FastCGI++
# Once done this will define
#
#  FastCGI++_FOUND - system has FastCGI++
#  FastCGI++_INCLUDE_DIRS - the FastCGI++ include directory
#  FastCGI++_LIBS - the FastCGI++ libraries
#  FastCGI++_VERSION_STRING - the version of FastCGI++ found (since CMake 2.8.8)


# This is just being set for now
# we need to check if it exists, and probably allow for specific components
set (FastCGI++_INCLUDE_DIRS
${FastCGI++_ROOT}/include
)
set (FastCGI++_LIBRARIES
${FastCGI++_ROOT}/lib/libfastcgipp.so
)
# liboms-php.so pyoms.so