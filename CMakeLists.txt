cmake_minimum_required(VERSION 3.3)

project(paraminf)

set(CMAKE_CXX_STANDARD 17)

find_package(ament_cmake REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

# add cmake functions
list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include (add_doxygen_compile)
include (add_gtest_compile)

# add compile options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

option(BUILD_TEST "Build tests" OFF)
option(BUILD_DOC "Build documentation" OFF)
option(BUILD_ALL "Build all" OFF)

if(BUILD_ALL)
  set(BUILD_TEST ON)
  set(BUILD_DOC ON)
endif()


ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

###########
## Build ##
###########

include_directories(
  include
  ${yaml-cpp_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
)

## Specify additional locations of header files
set(HEADERS
  include/${PROJECT_NAME}/parameter_interface.h
  include/${PROJECT_NAME}/yaml_io_handler.h
)

set(SOURCES
  src/parameter_interface.cpp
  src/yaml_io_handler.cpp
)

## Declare a cpp library
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME} yaml-cpp)

#############
## Install ##
#############

install(
  DIRECTORY include/
  DESTINATION include
)

install(TARGETS
  ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  DESTINATION lib/${PROJECT_NAME}
)

#############
## Testing ##
#############

set(TEST_SOURCES
  test/src/yaml_parser_test.cpp
  test/src/parameter_interface_test.cpp
)

add_gtest_compile()

##########
## DOCS ##
##########

add_doxygen_compile()

ament_package()
