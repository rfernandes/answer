# - Try to find Mongoose
# Once done this will define
#  MONGOOSE_FOUND - System has Mongoose
#  MONGOOSE_INCLUDE_DIRS - The Mongoose include directories
#  MONGOOSE_LIBRARIES - The libraries needed to use Mongoose
#  MONGOOSE_DEFINITIONS - Compiler switches required for using Mongoose

find_path(MONGOOSE_INCLUDE_DIR mongoose.h
          PATH_SUFFIXES mongoose )

find_library(MONGOOSE_LIBRARY NAMES mongoose)

             
set(MONGOOSE_LIBRARIES ${MONGOOSE_LIBRARY} )
set(MONGOOSE_INCLUDE_DIRS ${MONGOOSE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MONGOOSE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Mongoose DEFAULT_MSG
                                  MONGOOSE_LIBRARY MONGOOSE_INCLUDE_DIR)

mark_as_advanced(MONGOOSE_INCLUDE_DIR MONGOOSE_LIBRARY )