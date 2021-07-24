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
 
  ExternalProject_Add(googletest
        PREFIX googletest
        SOURCE_DIR "${PACKAGES_DIR}/googletest"
        CMAKE_CACHE_ARGS
          -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/googletest
          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
          -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    )
   
  add_subdirectory(tests)
endif()

add_library(tblink_rpc_core ${tblink_rpc_core_SRC})

target_include_directories(tblink_rpc_core PRIVATE
	${PACKAGES_DIR}/json/include
	${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
	${CMAKE_BINARY_DIR}/glog/include
	)

install(TARGETS tblink_rpc_core
    DESTINATION lib
    EXPORT tblink_rpc_core-targets)

message("tlink-rpc-core: PACKAGES_DIR=${PACKAGES_DIR}")

