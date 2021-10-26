#****************************************************************************
#* CMakeLists.txt
#*
#* project: tblink-rpc-core
#****************************************************************************

cmake_minimum_required(VERSION 3.11)

project(tblink-rpc-core)

include (ExternalProject)



if(NOT WIN32)
	add_compile_options(-fPIC)
#  list(APPEND CMAKE_C_FLAGS -fPIC)
#  list(APPEND CMAKE_CXX_FLAGS -fPIC)
endif()

set(CMAKE_CXX_STANDARD 11)

foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  string(REPLACE "/MDd" "/MTd" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

file(GLOB tblink_rpc_core_SRC
  cpp/*.cpp
  cpp/*.h)

if (NOT PACKAGES_DIR)
  set(PACKAGES_DIR ${CMAKE_SOURCE_DIR}/packages)

  # TODO: this should only be if tblink-rpc-core is a toplevel project 
  enable_testing()

  add_subdirectory(tests)
endif()


#******************************************************************** 
#* Static Library
#******************************************************************** 
add_library(tblink_rpc_core_static ${tblink_rpc_core_SRC})

target_include_directories(tblink_rpc_core_static PRIVATE
	${PACKAGES_DIR}/json/include
	${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
	)

install(TARGETS tblink_rpc_core_static
    DESTINATION lib
    EXPORT tblink_rpc_core-targets)


#******************************************************************** 
#* Shared Library
#******************************************************************** 
add_library(tblink_rpc_core SHARED ${tblink_rpc_core_SRC})

target_include_directories(tblink_rpc_core PRIVATE
	${PACKAGES_DIR}/json/include
	${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
	)

#target_link_directories(
#	tblink_rpc_core
#	PRIVATE
#	${CMAKE_BINARY_DIR}/glog/lib64
#	)

#target_link_libraries(
#	tblink_rpc_core
#	)

message("tlink-rpc-core: PACKAGES_DIR=${PACKAGES_DIR}")

