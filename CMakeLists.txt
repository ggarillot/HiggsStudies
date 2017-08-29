########################################################
# cmake file for building Marlin example Package
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( HiggsStudies )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )



### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )


FIND_PACKAGE( Marlin 1.0 REQUIRED )
FIND_PACKAGE( FastJet REQUIRED )
FIND_PACKAGE( ROOT REQUIRED )

FOREACH( pkg Marlin FastJet ROOT )
    IF( ${pkg}_FOUND )
		INCLUDE_DIRECTORIES( SYSTEM ${${pkg}_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
        ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
    ENDIF()
ENDFOREACH()

### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )
IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ENDIF()


### LIBRARY #################################################################

# include directories
include_directories( ${PROJECT_SOURCE_DIR}/include )
#INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=c++11" )
ADD_DEFINITIONS( "-Wno-long-long" )

# include directories
include_directories( ./include )

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.h)

#add library
add_library( ${PROJECT_NAME} SHARED ${sources} ${headers} )
install( TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib )


add_executable(toto ${PROJECT_SOURCE_DIR}/src/toto.cxx)
target_link_libraries( toto ${PROJECT_NAME} ${ROOT_COMPONENT_LIBRARIES} )
install(TARGETS toto RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)



#add_executable( ${testname} ${testsourcefile} )

#ADD_EXECUTABLE( toto ${toto} ${headers} ${sources} )
#TARGET_LINK_LIBRARIES( toto ${ROOT_COMPONENT_LIBRARIES} )
#INSTALL(TARGETS toto RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)

#AUX_SOURCE_DIRECTORY( src library_sources )
#ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${library_sources} )
#INSTALL_SHARED_LIBRARY( ${PROJECT_NAME} DESTINATION lib )

# display some variables and write them to cache
#DISPLAY_STD_VARIABLES()
