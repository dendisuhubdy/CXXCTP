﻿cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

set(PROJECT_NAME CXXCTP_core)
set(PROJECT_VERSION 1.0.1)

# Building as separate project.
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CLING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cling-build/" CACHE STRING "CLING DIR")

# Conan package manager List of packages used by the project is container in
# conanfile.py
option(CONAN_AUTO_INSTALL "Let CMake call conan install automatically" ON)
if (CONAN_AUTO_INSTALL)
  set(CONAN_PROFILE
      "clang"
      CACHE STRING "Conan profile to use during installation")
  include(cmake/conan-auto-install.cmake)
  if (NOT CMAKE_BUILD_TYPE MATCHES "Debug" )
    set(conan_build_type "Release")
  else()
    set(conan_build_type "Debug")
  endif()
  conan_auto_install(${CONAN_PROFILE} "-s;build_type=${conan_build_type};--build=missing;-o;enable_tests=False;-o;openssl:shared=True")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
list(
  APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules/CXTPL/cmake/exports")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/") # for conan

# Setup conan and include everything.
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/)
  include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake OPTIONAL)
  conan_basic_setup(
    # prevent conan_basic_setup from resetting cmake variables
    TARGETS
    #KEEP_RPATHS
    # see https://github.com/conan-io/conan/issues/6012
    #NO_OUTPUT_DIRS
  )
else()
  message (WARNING "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake not found!")
  message (FATAL_ERROR "must use conan")
endif()

# Keep symbols for JIT resolution
set(LLVM_NO_DEAD_STRIP 1)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

set(USE_G3LOG
  TRUE CACHE BOOL
  "Use g3log logger")

set(USE_FOLLY
  TRUE CACHE BOOL
  "Use facebook/folly library (Apache License 2.0)")

set(USE_RANG
  FALSE CACHE BOOL
  "Use RANG for coloring terminal")

set(BUILD_SHARED_LIBS
  FALSE CACHE BOOL
  "Use .so/.dll")

set(BUILD_EXAMPLES
  FALSE CACHE BOOL
  "Use built-in examples")

set(ENABLE_TESTS
  FALSE CACHE BOOL
  "Use unit tests")

set(BUNDLE_EXAMPLE_SCRIPTS
  FALSE CACHE BOOL
  "Use built-in scripts from examples")

if(BUILD_SHARED_LIBS)
  set(CORE_LIB_TYPE SHARED)
else()
  set(CORE_LIB_TYPE STATIC)
endif(BUILD_SHARED_LIBS)

set(ENABLE_CLING TRUE CACHE BOOL "ENABLE_CLING")
message(STATUS "ENABLE_CLING=${ENABLE_CLING}")

set(ALLOW_PER_PROJECT_CTP_SCRIPTS
  TRUE CACHE BOOL
  "Use per-project ctp_scripts (requires Cling)")
message(STATUS "ALLOW_PER_PROJECT_CTP_SCRIPTS=${ALLOW_PER_PROJECT_CTP_SCRIPTS}")

set(INSTALL_CLING TRUE CACHE BOOL "INSTALL_CLING")
message(STATUS "INSTALL_CLING=${INSTALL_CLING}")

set(ENABLE_CXXCTP TRUE CACHE BOOL "ENABLE_CXXCTP")
message(STATUS "ENABLE_CXXCTP=${ENABLE_CXXCTP}")

set(CUSTOM_PLUGINS
    "${CMAKE_CURRENT_SOURCE_DIR}/custom_plugins.cmake"
    CACHE STRING
    "Path to custom plugins")
message(STATUS "CUSTOM_PLUGINS=${CUSTOM_PLUGINS}")

include(Dependencies.cmake)

include(Utils.cmake)

macro(add_ctp_plugin
      CALLBACK_NAME
      HEADER_FILE
      SOURCE_FILE
      CMAKELISTS_DIR)

  list(
    APPEND
      ADD_CALLBACKS_FOR_BUNDLED_SCRIPTS
      "clang_utils::add_cxxctp_callback(\"${CALLBACK_NAME}\", &${CALLBACK_NAME}){{SEMICOLON}}\n"
    )
  list(APPEND ADD_INCLUDES_FOR_BUNDLED_SCRIPTS "#include \"${HEADER_FILE}\"\n")

  if(EXISTS ${CMAKELISTS_DIR} AND EXISTS "${CMAKELISTS_DIR}/CMakeLists.txt")
    add_subdirectory(${CMAKELISTS_DIR})
  endif()

  target_sources(CXXCTP_core PRIVATE ${SOURCE_FILE} ${HEADER_FILE})
endmacro(add_ctp_plugin)

list(APPEND COMMON_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/options/ctp/options.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/options/ctp/options.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/clangUtils.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/clangUtils.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/funcParser.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/funcParser.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/DispatchQueue.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/DispatchQueue.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/clangPipeline.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/clangPipeline.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ClingInterpreterModule.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/ClingInterpreterModule.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/utils.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/inputThread.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/inputThread.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/reflect/ReflectionCache.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/reflect/ReflectionCache.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/reflect/ReflTypes.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/reflect/ReflTypes.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/reflect/TypeInfo.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/reflect/TypeInfo.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/reflect/ReflectAST.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/reflect/ReflectAST.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/reflect/ast_utils.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/template_engine/CXTPL_AnyDict.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/template_engine/CXTPL_AnyDict.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/template_engine/I_Dict.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/template_engine/I_Dict.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/integrations/outcome/error_utils.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/integrations/outcome/error_macros.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/core/errors/errors_CodegenError.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/errors/errors.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ctp_registry.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/ctp_registry.hpp)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/ctp_registry.cpp PROPERTIES
                            GENERATED 1)

if(ENABLE_CLING)
  #add_library(simple_lib SHARED
  #  ${CMAKE_CURRENT_SOURCE_DIR}/src/simplelib.cpp
  #)

  list(APPEND CLING_DEFINITIONS CLING_IS_ON=1)
  if(ENABLE_CXXCTP)
    # NOTE: cling does not support static libs, so we use SHARED
    # https://github.com/root-project/cling/issues/280
    # https://gitlab.kitware.com/cmake/cmake/issues/16473
    add_library(CXXCTP_core
                ${CORE_LIB_TYPE} ${COMMON_FILES}
                #${CMAKE_CURRENT_SOURCE_DIR}/src/main_cling.cpp
                # TODO: DEPENDS ${cxtpl_outputs}
                )
  endif(ENABLE_CXXCTP)
else(ENABLE_CLING)
  #message(FATAL_ERROR "Only Cling interpreter is supported for now")
  if(ENABLE_CXXCTP)
    # NOTE: cling does not support static libs, so we use SHARED
    # https://github.com/root-project/cling/issues/280
    # https://gitlab.kitware.com/cmake/cmake/issues/16473
    add_library(
      CXXCTP_core
      ${CORE_LIB_TYPE} ${COMMON_FILES}
      #${CMAKE_CURRENT_SOURCE_DIR}/src/main_cling.cpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/make_interface/make_interface.cpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/make_interface/make_interface.hpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/make_reflect/make_reflect.cpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/make_reflect/make_reflect.hpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/typeclass_instance/typeclass_instance.cpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/typeclass_instance/typeclass_instance.hpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/typeclass/typeclass.cpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/typeclass/typeclass.hpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/make_removefuncbody/make_removefuncbody.cpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/make_removefuncbody/make_removefuncbody.hpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/reflect_enum/reflect_enum.cpp
      #${CMAKE_CURRENT_SOURCE_DIR}/resources/ctp_scripts/2_scripts/reflect_enum/reflect_enum.hpp
      )
  endif(ENABLE_CXXCTP)
endif(ENABLE_CLING)

if(EXISTS ${CUSTOM_PLUGINS})
  include(${CUSTOM_PLUGINS})
endif()

if(BUNDLE_EXAMPLE_SCRIPTS)
  # scripts placed here will be bundled with CXXCTP_tool
  # (will become built-in C++ plugins)
  #list(APPEND BUILT_IN_PUGINS
  #  ${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/2_scripts/make_reflect/make_reflect.cpp
  #  #${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/2_scripts/make_reflect/make_reflect.hpp
  #  # NOTE: we control loading order by file/folder names (0_file, 1_file, ...)
  #  #${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/1_utils/CXTPL_STD/CXTPL_STD.cpp
  #  #${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/1_utils/CXTPL_STD/CXTPL_STD.hpp
  #  #${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/1_utils/CXXCTP_STD/CXXCTP_STD.cpp
  #  #${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/1_utils/CXXCTP_STD/CXXCTP_STD.hpp
  #)
  add_ctp_plugin(
    "make_reflect"
    ${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/2_scripts/make_reflect/make_reflect.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/2_scripts/make_reflect/make_reflect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ctp_scripts/2_scripts/make_reflect)
  add_ctp_plugin(
    "make_interface"
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/make_interface/make_interface.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/make_interface/make_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/make_interface)
  add_ctp_plugin(
    "make_removefuncbody"
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/make_removefuncbody/make_removefuncbody.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/make_removefuncbody/make_removefuncbody.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/make_removefuncbody)
  add_ctp_plugin(
    "reflect_enum"
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/reflect_enum/reflect_enum.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/reflect_enum/reflect_enum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/reflect_enum)
  add_ctp_plugin(
    "typeclass"
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass/typeclass.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass/typeclass.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass)
  add_ctp_plugin(
    "typeclass_combo"
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass_combo/typeclass_combo.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass_combo/typeclass_combo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass_combo)
  add_ctp_plugin(
    "typeclass_instance"
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass_instance/typeclass_instance.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass_instance/typeclass_instance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/2_scripts/typeclass_instance)
  target_sources(
    CXXCTP_core PRIVATE
    # NOTE: we control loading order by file/folder names (0_file, 1_file, ...)
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/1_utils/CXTPL_STD/CXTPL_STD.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/1_utils/CXTPL_STD/CXTPL_STD.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/1_utils/CXXCTP_STD/CXXCTP_STD.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple/ctp_scripts/1_utils/CXXCTP_STD/CXXCTP_STD.hpp
    )
endif(BUNDLE_EXAMPLE_SCRIPTS)

string(REPLACE ";" "\n" ADD_CALLBACKS_FOR_BUNDLED_SCRIPTS
               "${ADD_CALLBACKS_FOR_BUNDLED_SCRIPTS}")
string(REPLACE ";" "\n" ADD_INCLUDES_FOR_BUNDLED_SCRIPTS
               "${ADD_INCLUDES_FOR_BUNDLED_SCRIPTS}")
# SEMICOLON hack to add ";" symbol to configure_file
string(REPLACE "{{SEMICOLON}}" ";" ADD_CALLBACKS_FOR_BUNDLED_SCRIPTS
               "${ADD_CALLBACKS_FOR_BUNDLED_SCRIPTS}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/ctp_registry.cpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/ctp_registry.cpp)

set(USED_SYSTEM_LIBS
    Threads::Threads # pthread, https://cmake.org/cmake/help/v3.13/module/FindThreads.html
    ${X11_LIBRARIES} # https://cmake.org/cmake/help/v3.13/module/FindX11.html
    ${CMAKE_DL_LIBS} # https://cmake.org/cmake/help/v3.13/variable/CMAKE_DL_LIBS.html
    EXPAT::EXPAT # https://cmake.org/cmake/help/v3.13/module/FindEXPAT.html
    ZLIB::ZLIB # https://cmake.org/cmake/help/v3.13/module/FindZLIB.html
               # Custom libs
    stdc++fs # C++17 std::filesystem
    CACHE INTERNAL "USED_SYSTEM_LIBS")

#set(USED_BOOST_LIBS
#    Boost::boost
#    Boost::coroutine
#    Boost::fiber
#    Boost::context
#    Boost::filesystem
#    Boost::program_options
#    Boost::regex
#    Boost::system
#    CACHE INTERNAL "USED_BOOST_LIBS")

if(ENABLE_CLING)
    list(
      APPEND
        CLING_LIBS
        # TODO: use FindClang.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangARCMigrate.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangAnalysis.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangAST.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangASTMatchers.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangBasic.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangCodeGen.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangDriver.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangDynamicASTMatchers.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangEdit.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangFormat.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangFrontend.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangFrontendTool.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangIndex.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangLex.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangParse.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangRewrite.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangRewriteFrontend.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangSema.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangSerialization.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangStaticAnalyzerCheckers.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangStaticAnalyzerCore.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangStaticAnalyzerFrontend.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangTooling.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangToolingCore.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangToolingRefactor.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangStaticAnalyzerCore.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangDynamicASTMatchers.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangCodeGen.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangFrontendTool.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclang.so
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangEdit.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangRewriteFrontend.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangDriver.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangSema.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangASTMatchers.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangSerialization.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangBasic.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangAST.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangTooling.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangStaticAnalyzerFrontend.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangFormat.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangToolingRefactor.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangLex.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangFrontend.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangRewrite.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangToolingCore.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangIndex.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangAnalysis.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangParse.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangStaticAnalyzerCheckers.a
        ${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/libclangARCMigrate.a)
endif(ENABLE_CLING)

# ...which links against clingInterpreter (and its dependencies).
if(ENABLE_CXXCTP)
  if(ENABLE_CLING)
    target_link_libraries(
      CXXCTP_core
      PUBLIC
             Cling::Cling
             Cling::ClingInterpreter
             Cling::clingUtils
             Cling::clingMetaProcessor
             ${CLING_LIBS}
             #${CONAN_LIBS}
             #CONAN_PKG::catch2
      )
  endif(ENABLE_CLING)

  target_link_libraries(
    CXXCTP_core
    PUBLIC
         # system libs
         ${USED_SYSTEM_LIBS}
         # @note: Order matters https://stackoverflow.com/a/10269201/10904212
         ${LIBIBERTY_LIBRARY} # used by folly
         ${DOUBLE_CONVERSION_LIBRARY} # used by folly
         ${LIBEVENT_LIB} # used by folly
         ${LZ4_LIBRARY} # used by folly
         ${LIBUNWIND_LIBRARIES} # used by folly
         ${LIBLZMA_LIBRARIES} # used by folly
         CONAN_PKG::clang_folly_conan
         CONAN_PKG::boost
         CONAN_PKG::double-conversion
         CONAN_PKG::glog
         #CONAN_PKG::libevent
         CONAN_PKG::lz4
         #CONAN_PKG::openssl
         CONAN_PKG::lzma
         CONAN_PKG::zstd
         CONAN_PKG::snappy
         CONAN_PKG::libsodium
         CONAN_PKG::libdwarf
         CONAN_PKG::bzip2
         CONAN_PKG::gflags
         CONAN_PKG::libunwind
         #CONAN_PKG::poco
         #CONAN_PKG::OpenSSL
         CONAN_PKG::libelf
         #CONAN_PKG::zlib
         CONAN_PKG::xz_utils
         CONAN_PKG::corrade
         Corrade::PluginManager
         # boost libs
         ${USED_BOOST_LIBS}
         boost_outcome
         microsoft_gsl
         #${CONAN_LIBS}
         #CONAN_PKG::catch2
    )

    target_link_libraries( CXXCTP_core PRIVATE
      ${base_LIB}
      ${build_util_LIB}
    )

  #target_link_libraries(CXXCTP_core PRIVATE
  #  Cling::Cling
  #  Cling::ClingInterpreter
  #  Cling::clingUtils
  #  Cling::clingMetaProcessor
  #  ${CLING_LIBS}
  #)

  target_include_directories(
    CXXCTP_core
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src
            # path to ctp_scripts/1_utils/CXXCTP_STD/CXXCTP_STD.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple)

  target_include_directories(
    CXXCTP_core
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/submodules/boost.outcome/include/outcome/quickcpplib/include/ # TODO
      ${CMAKE_CURRENT_SOURCE_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/ # path to ctp_scripts
      ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple # path to ctp_scripts
      ${LIBIBERTY_INCLUDE_DIR} # used by folly
      #${G3LOG_INCLUDE_DIR} # used by folly
      ${LIBEVENT_INCLUDE_DIR} # used by folly
      ${LZ4_INCLUDE_DIR} # used by folly
      ${FOLLY_INCLUDE_DIR}
      ${CONAN_INCLUDE_DIRS})

  if(BUNDLE_EXAMPLE_SCRIPTS)
    target_include_directories(
      CXXCTP_core
      PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple # path to ctp_scripts
      )
  endif(BUNDLE_EXAMPLE_SCRIPTS)

  target_compile_options(CXXCTP_core PRIVATE -fno-rtti)

  target_compile_definitions(CXXCTP_core
                             PUBLIC
                             ${CLING_DEFINITIONS}
                             )

  target_compile_definitions(CXXCTP_core PRIVATE
                             # https://stackoverflow.com/a/30877725
                             BOOST_SYSTEM_NO_DEPRECATED BOOST_ERROR_CODE_HEADER_ONLY
                             IS_CXXCTP=1
                             # https://github.com/facebook/folly/issues/976
                             #FOLLY_ASSUME_NO_JEMALLOC=1
                             #UFOLLY_USE_JEMALLOC
                             )

  set_target_properties(CXXCTP_core PROPERTIES ENABLE_EXPORTS 1)

  # POSITION_INDEPENDENT_CODE for -fPIC
  set_property(TARGET CXXCTP_core PROPERTY POSITION_INDEPENDENT_CODE ON)

  if(ENABLE_CLING)
    if(MSVC)
      set_target_properties(CXXCTP_tool PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)
      set_property(
        TARGET CXXCTP_tool
        APPEND_STRING
        PROPERTY LINK_FLAGS
                 "/EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00D_K@Z
                  /EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00DM@Z
                  /EXPORT:cling_runtime_internal_throwIfInvalidPointer")
    endif()

    target_compile_definitions(CXXCTP_core PRIVATE CLING_IS_ON=1)
  endif(ENABLE_CLING)

  if(BUNDLE_EXAMPLE_SCRIPTS)
    target_compile_definitions(CXXCTP_core PRIVATE BUNDLE_EXAMPLE_SCRIPTS=1)
  endif(BUNDLE_EXAMPLE_SCRIPTS)

  if(ALLOW_PER_PROJECT_CTP_SCRIPTS)
    target_compile_definitions(CXXCTP_core PUBLIC ALLOW_PER_PROJECT_CTP_SCRIPTS=1)
  endif(ALLOW_PER_PROJECT_CTP_SCRIPTS)
endif(ENABLE_CXXCTP)

include(Codegen_files.cmake)

add_subdirectory(tool)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)

if(ENABLE_TESTS)
  # Now enable our tests.
  enable_testing()
  add_subdirectory(tests)
endif(ENABLE_TESTS)

if(ENABLE_CLING AND INSTALL_CLING)
  list(APPEND CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cling-build/build/lib/" # source directory
    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" # target directory
    FILES_MATCHING # install only matched files
    PATTERN "*.so"
    PATTERN "*.so.*"
    PATTERN "*.dll"
    PATTERN "*.dll.*"
    PATTERN "*.a"
    PATTERN "*.lib"
    PATTERN "CMake" EXCLUDE
    PATTERN "cmake" EXCLUDE
    PATTERN "CMakeFiles" EXCLUDE
    PATTERN "ToolDrivers" EXCLUDE
    PATTERN "ProfileData" EXCLUDE
    PATTERN "Linker" EXCLUDE
    PATTERN "LTO" EXCLUDE
    PATTERN "IR" EXCLUDE
    PATTERN "Option" EXCLUDE
    PATTERN "ObjectYAML" EXCLUDE
    PATTERN "DebugInfo" EXCLUDE
    PATTERN "LineEditor" EXCLUDE
    PATTERN "Target" EXCLUDE
    PATTERN "Demangle" EXCLUDE
    PATTERN "Passes" EXCLUDE
    PATTERN "Object" EXCLUDE
    PATTERN "XRay" EXCLUDE
    PATTERN "Fuzzer" EXCLUDE
    PATTERN "Support" EXCLUDE
    PATTERN "Bitcode" EXCLUDE
    PATTERN "CodeGen" EXCLUDE
    PATTERN "MC" EXCLUDE
    PATTERN "IRReader" EXCLUDE
    PATTERN "TableGen" EXCLUDE
    PATTERN "Analysis" EXCLUDE
    PATTERN "Testing" EXCLUDE
    PATTERN "AsmParser" EXCLUDE
    PATTERN "clang" EXCLUDE
    PATTERN "Transforms" EXCLUDE
    PATTERN "BinaryFormat" EXCLUDE
    PATTERN "ExecutionEngine" EXCLUDE)
endif(ENABLE_CLING AND INSTALL_CLING)

install(TARGETS CXXCTP_core DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
