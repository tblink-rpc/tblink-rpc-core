
cmake_minimum_required(VERSION 2.8.8)

file(GLOB TEST_SRCS 
	"cpp/*.cpp" 
	"cpp/*.h")

link_directories(
	${CMAKE_BINARY_DIR}/googletest/lib
	${CMAKE_BINARY_DIR}/googletest/lib64
	${CMAKE_BINARY_DIR}/glog/lib
	${CMAKE_BINARY_DIR}/glog/lib64
	${CMAKE_BINARY_DIR}/antlr/lib
	${CMAKE_BINARY_DIR}/pssparser
	${CMAKE_BINARY_DIR}/pss_ast/lib
	)

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
# list(APPEND CMAKE_CXX_FLAGS -DTEST_SRCDIR=${CMAKE_CURRENT_SOURCE_DIR})
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  string(REPLACE "/MDd" "/MTd" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

#********************************************************************
#* GoogleTest unit tests
#********************************************************************
add_executable(tblink_rpc_core_testmain ${TEST_SRCS})

set_property(TARGET tblink_rpc_core_testmain PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_dependencies(tblink_rpc_core_testmain GLOG googletest)
target_include_directories(tblink_rpc_core_testmain PRIVATE
	${CMAKE_BINARY_DIR}/googletest/include
	${PACKAGES_DIR}/googletest/googletest/include
	${CMAKE_CURRENT_SOURCE_DIR}/../cpp/include)

list(APPEND TBLINK_RPC_CORE_TESTMAIN_LIBS gtest gtest_main tblink_rpc_core)
list(APPEND TBLINK_RPC_CORE_TESTMAIN_LIBS glog)

if (WIN32)
#	if (BUILD_SHARED_LIBS)
#		list(APPEND TBLINK_RPC_CORE_TESTMAIN_LIBS antlr4-runtime)
#	else()
#		list(APPEND TBLINK_RPC_CORE_TESTMAIN_LIBS antlr4-runtime-static)
#	endif()
else()
	list(APPEND TBLINK_RPC_CORE_TESTMAIN_LIBS pthread)
endif()



target_link_libraries(tblink_rpc_core_testmain ${TBLINK_RPC_CORE_TESTMAIN_LIBS})

add_test(
	NAME unit
	COMMAND ${CMAKE_BINARY_DIR}/tests/pssparser_testmain)


