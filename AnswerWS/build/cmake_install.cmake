# Install script for directory: /home/c/Project/Code/AnswerWS2/AnswerWS

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/opt/wps")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "library")
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libanswer.so.SOVERSION"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libanswer.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      FILE(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    ENDIF()
  ENDFOREACH()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/c/Project/Code/AnswerWS2/AnswerWS/build/libanswer.so.SOVERSION"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/build/libanswer.so"
    )
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libanswer.so.SOVERSION"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libanswer.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      FILE(RPATH_REMOVE
           FILE "${file}")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "${file}")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDFOREACH()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "library")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "library")
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libanswer-module.so.SOVERSION"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libanswer-module.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      FILE(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    ENDIF()
  ENDFOREACH()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/c/Project/Code/AnswerWS2/AnswerWS/build/libanswer-module.so.SOVERSION"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/build/libanswer-module.so"
    )
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libanswer-module.so.SOVERSION"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libanswer-module.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      FILE(RPATH_REMOVE
           FILE "${file}")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "${file}")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDFOREACH()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "library")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/answer" TYPE FILE FILES
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/InstantiationStrategy.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/Environment.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/Codec.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/XmlParams.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/Params.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/Context.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/OperationStore.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/WebModule.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/ClassTemplateHelpers.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/Cookie.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/Operation.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/Utils.hh"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/Exception.hh"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/answer/serialization" TYPE FILE FILES
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/serialization/ws_xml_iarchive.hpp"
    "/home/c/Project/Code/AnswerWS2/AnswerWS/include/answer/serialization/ws_xml_oarchive.hpp"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/c/Project/Code/AnswerWS2/AnswerWS/build/adapters/ApacheModule/cmake_install.cmake")
  INCLUDE("/home/c/Project/Code/AnswerWS2/AnswerWS/build/adapters/Axis2c/cmake_install.cmake")
  INCLUDE("/home/c/Project/Code/AnswerWS2/AnswerWS/build/adapters/FCGI/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/c/Project/Code/AnswerWS2/AnswerWS/build/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/c/Project/Code/AnswerWS2/AnswerWS/build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
