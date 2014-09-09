cmake_minimum_required(VERSION 2.6)

#Configuration du projet

project(ramsesRouter)

SET(BIN_NAME "raRt")

SET(CMAKE_VERBOSE_MAKEFILE 1)

SET(CMAKE_SKIP_BUILD_RPATH 1)

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BUILD_TYPE})

#INCLUDE(CTest)

include_directories (./ ../ ../libs/include)
#add_subdirectory (./tests/ ./bin/tests)




MESSAGE("Install distination directory :${DISTDIR}")

#IF("${DISTDIR}" MATCHES "")
#	set(DISTDIR /usr/bin)
#MESSAGE(STATUS "oups.")
#ENDIF(${DISTDIR})

SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 0)
SET(PATCH_VERSION 2)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(RART_OS_VERSION 64)
else()
  set(RART_OS_VERSION 32)
endif()



IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_SET_DESTDIR "on")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
SET(CPACK_GENERATOR "DEB")

SET(CPACK_PACKAGE_DESCRIPTION "raRt udp packet router to F1")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "raRt - simple udp to tcp router ")
SET(CPACK_PACKAGE_VENDOR "ESI")
SET(CPACK_PACKAGE_CONTACT "vassilux ")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${RART_OS_VERSION}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${RART_OS_VERSION}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost1.40-all-dev")

SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

SET(CPACK_COMPONENTS_ALL Libraries ApplicationData)
INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")




set(CONFIG_INSTALL_DIR "${DISTDIR}/param")

set(BIN_INSTALL_PREFIX "${DISTDIR}/plc/proc")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)

#Inclusion de Boost
include(FindBoost)
find_package(
	Boost
	1.40.0
	REQUIRED system
	REQUIRED thread
	REQUIRED program_options
)

#FIND_LIBRARY(ZLOG_LIBRARY_PATH ${BIN_NAME} /home/vassilux/Projects/ramses_proxy/libs/lib/libzlog.a)
set(ZLOG_LIBRARY_PATH "/home/vassilux/Projects/ramses_proxy/libs/lib/")

FIND_LIBRARY(ZLOG_LIBRARY NAMES zlog libzlog.a PATHS "/home/vassilux/Projects/ramses_proxy/libs/lib"  DOC "Freetype library")
MESSAGE("ZLOG_LIBRARY:${ZLOG_LIBRARY}")

link_directories(${Boost_LIBRARY_DIRS} ${ZLOG_LIBRARY_PATH})
include_directories(${Boost_INCLUDE_DIRS}, ../inc)

#Configuration de l'exécutable
file(
	GLOB_RECURSE
	source_files
	*.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

add_executable(
	${BIN_NAME}
	${source_files}
)

#Configuration de l'édition de liens
target_link_libraries(
	${BIN_NAME}
	${Boost_LIBRARIES}
	zlog
	
)

#for x64 m64
set_target_properties(${BIN_NAME} PROPERTIES COMPILE_FLAGS "-m${RART_OS_VERSION}" LINK_FLAGS "-m${RART_OS_VERSION}")

INSTALL(TARGETS ${BIN_NAME} 
    RUNTIME DESTINATION ${BIN_INSTALL_PREFIX}        COMPONENT Runtime     # .exe, .dll
   )
INSTALL(FILES ../conf/${BIN_NAME}.cfg 
    DESTINATION ${CONFIG_INSTALL_DIR}
    )

enable_testing ()

