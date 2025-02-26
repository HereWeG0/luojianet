set(SECURE_CXX_FLAGS "")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(WIN32)
        set(SECURE_CXX_FLAGS "-fstack-protector-all")
    else()
    set(SECURE_CXX_FLAGS "-fstack-protector-all -Wl,-z,relro,-z,now,-z,noexecstack")
    endif()
endif()
set(_ms_tmp_CMAKE_CXX_FLAGS_F ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
set(TOP_DIR ${CMAKE_SOURCE_DIR})

include(cmake/utils.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/external_libs/eigen.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/external_libs/json.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/dependency_securec.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/external_libs/protobuf.cmake)

if(MS_BUILD_GRPC)
    # build dependencies of gRPC
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/openssl.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/absl.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/c-ares.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/zlib.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/re2.cmake)
    # build gRPC
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/grpc.cmake)
    # build event
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/libevent.cmake)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/external_libs/pybind11.cmake)
MESSAGE("go to link flatbuffers")
include(${CMAKE_SOURCE_DIR}/cmake/external_libs/flatbuffers.cmake)
if(USE_GLOG)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/glog.cmake)
endif()

# find_package(Python3)
# include_directories(${Python3_INCLUDE_DIRS})
find_package(Python3 COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message("Python3 found, version: ${Python3_VERSION}")
    message("Python3 library path: ${Python3_LIBRARY}")
    message("Python3 interpreter: ${Python3_EXECUTABLE}")
	include_directories(${Python3_INCLUDE_DIRS})
elseif(Python3_LIBRARY AND Python3_EXECUTABLE AND
        ${Python3_VERSION} VERSION_GREATER_EQUAL "3.7.0" AND ${Python3_VERSION} VERSION_LESS "3.9.9")
    message(WARNING "Maybe python3 environment is broken.")
    message("Python3 library path: ${Python3_LIBRARY}")
    message("Python3 interpreter: ${Python3_EXECUTABLE}")
else()
    find_package(PythonInterp)
    find_package(PythonLibs)
    message("PYTHON_INCLUDE_DIR: ${PYTHON_INCLUDE_DIR}")
    message("PYTHON_LIBRARY: ${PYTHON_LIBRARY}")
    message("PYTHON_VERSION_STRING: ${PYTHON_VERSION_STRING}")
    message("PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
    include_directories(${PYTHON_INCLUDE_DIR})
    if(PYTHONINTERP_FOUND)
        message("Python3 found, version: ${PYTHON_VERSION_STRING}")
        message("Python3 library path: ${PYTHON_LIBRARY}")
        message("Python3 interpreter: ${PYTHON_EXECUTABLE}")
    elseif(PYTHON_LIBRARY AND PYTHON_EXECUTABLE AND
            ${PYTHON_VERSION_STRING} VERSION_GREATER_EQUAL "3.7.0" AND ${Python3_VERSION} VERSION_LESS "3.9.9")
        message(WARNING "Maybe python3 environment is broken.")
        message("Python3 library path: ${PYTHON_LIBRARY}")
        message("Python3 interpreter: ${PYTHON_EXECUTABLE}")
    else()
        message(FATAL_ERROR "Python3 not found, please install Python>=3.7.5, and set --enable-shared "
                "if you are building Python locally")
    endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/third_party)
if(ENABLE_MPI)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/ompi.cmake)
endif()

if(ENABLE_CPU)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/mkl_dnn.cmake)
endif()

if(ENABLE_GPU)
    if(ENABLE_MPI)
        include(${CMAKE_SOURCE_DIR}/cmake/external_libs/nccl.cmake)
    endif()
endif()

if(ENABLE_GE)
    include_directories(${CMAKE_SOURCE_DIR}/third_party/ge/include)
    include_directories(${CMAKE_SOURCE_DIR}/third_party/ge/include/external)
    include_directories(${CMAKE_SOURCE_DIR}/third_party/ge/include/external/graph)
    link_directories(${CMAKE_SOURCE_DIR}/third_party/ge/lib)
elseif(ENABLE_D OR ENABLE_ACL OR ENABLE_TESTCASES)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/inc)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/inc/external)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/metadef/inc)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/metadef/inc/external)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/metadef/inc/external/graph)
endif()

if(ENABLE_GE OR ENABLE_D OR ENABLE_ACL OR ENABLE_TESTCASES)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/inc)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/inc/external)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/inc/framework)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/third_party/fwkacllib/inc)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/third_party/fwkacllib/inc/toolchain)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/metadef/inc)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/metadef/inc/external)
    include_directories(${CMAKE_SOURCE_DIR}/graphengine/metadef/inc/external/graph)
endif()

if(ENABLE_MINDDATA)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/icu4c.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/libtiff.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/opencv.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/sqlite.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/tinyxml2.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/cppjieba.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/sentencepiece.cmake)
endif()

# if(ENABLE_RS)
	# include(${CMAKE_SOURCE_DIR}/cmake/external_libs/proj.cmake)
	# include(${CMAKE_SOURCE_DIR}/cmake/external_libs/gdal.cmake)
# endif()

if(ENABLE_MINDDATA)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/jpeg_turbo.cmake)
endif()

if(ENABLE_TESTCASES OR ENABLE_CPP_ST)
    include(${CMAKE_SOURCE_DIR}/cmake/external_libs/gtest.cmake)
endif()

set(CMAKE_CXX_FLAGS ${_ms_tmp_CMAKE_CXX_FLAGS_F})
