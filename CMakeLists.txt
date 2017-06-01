cmake_minimum_required(VERSION 3.4)

# bitprim-server
#==============================================================================
project(bitprim-server
  VERSION 3.0.0
  LANGUAGES CXX C)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Check for baseline language coverage in the compiler for the C++11 standard.
#------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Process options.
#==============================================================================
# Implement --with-remote-blockchain and declare WITH_REMOTE_BLOCKCHAIN.
#------------------------------------------------------------------------------
option(WITH_REMOTE_BLOCKCHAIN "Compile with remote bitprim-blockchain (aka. bitprim-blockchain-requester)." ON)

# Implement --with-remote-database and declare WITH_REMOTE_DATABASE.
#------------------------------------------------------------------------------
option(WITH_REMOTE_DATABASE "Compile with remote bitprim-database (aka. bitprim-database-requester)." ON)

# Implement --with-remote-database and declare WITH_REMOTE_DATABASE.
#------------------------------------------------------------------------------
option(WITH_LOCAL_MINING "Compile with remote bitprim-mining." ON)
# Implement --with-tests and declare WITH_TESTS.
#------------------------------------------------------------------------------
option(WITH_TESTS "Compile with unit tests." ON)

# Implement --with-console and declare WITH_CONSOLE.
#------------------------------------------------------------------------------
option(WITH_CONSOLE "Compile console application." ON)

# Inherit --enable-shared and define BOOST_TEST_DYN_LINK.
#------------------------------------------------------------------------------
option(ENABLE_SHARED "" OFF)

# Implement --with-litecoin.
#------------------------------------------------------------------------------
option(WITH_LITECOIN "Compile with Litecoin support." OFF)
if (WITH_LITECOIN)
  add_definitions(-DLITECOIN)
endif()


# Check dependencies.
#==============================================================================
# Require Boost of at least version 1.56.0 and output ${boost_CPPFLAGS/LDFLAGS}.
#------------------------------------------------------------------------------
if (NOT ENABLE_SHARED)
  set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(
  Boost 1.56.0 REQUIRED
  COMPONENTS unit_test_framework)

# Require bitprim-node of at least version 3.0.0 and output ${bitprim_node_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
if (NOT TARGET bitprim-node)
  find_package(bitprim-node 3.0.0 REQUIRED)
endif()

# Require bitprim-protocol of at least version 3.0.0 and output ${bitprim_protocol_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
if (NOT TARGET bitprim-protocol)
  find_package(bitprim-protocol 3.0.0 REQUIRED)
endif()

# Require bitprim-blockchain of at least version 3.0.0 and output ${bitprim_blockchain_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
if(WITH_REMOTE_BLOCKCHAIN)
  if (NOT TARGET bitprim-blockchain-requester)
    find_package(bitprim-blockchain-requester 3.0.0 REQUIRED)
  endif()
else()
  if (NOT TARGET bitprim-blockchain)
    find_package(bitprim-blockchain 3.0.0 REQUIRED)
  endif()
endif()

# Require bitprim-mining of at least version 3.0.0 and output ${bitprim_mining_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
if (WITH_LOCAL_MINING)
  if (NOT TARGET bitprim-mining)
    find_package(bitprim-mining 3.0.0 REQUIRED)
    add_definitions(-DWITH_LOCAL_MINING)
  endif()
endif()

# Set flags.
#==============================================================================
include(CheckCXXCompilerFlag)

function(_add_c_compile_flag _Flag _Var)
  check_cxx_compiler_flag(${_Flag} ${_Var})
  if (${_Var})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_Flag}" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_Flag}" )
  endif()
endfunction()

function(_add_cxx_compile_flag _Flag _Var)
  check_cxx_compiler_flag(${_Flag} ${_Var})
  if (${_Var})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_Flag}" )
  endif()
endfunction()

function(_add_link_flag _Flag _Var)
  check_cxx_compiler_flag(${_Flag} ${_Var})
  if (${_Var})
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${_Flag}" )
    set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${_Flag}" )
  endif()
endfunction()

# Warn on all stuff.
#------------------------------------------------------------------------------
if (NOT MSVC)
  _add_c_compile_flag(-Wall _has_all_warning_flag)
else()
  _add_c_compile_flag(-W4 _has_all_warning_flag)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

# Warn on extra stuff.
#------------------------------------------------------------------------------
if (NOT MSVC)
  _add_c_compile_flag(-Wextra _has_extra_warning_flag)
endif()

# Be really annoying.
#------------------------------------------------------------------------------
_add_c_compile_flag(-Wpedantic _has_pedantic_warning_flag)
if (_has_pedantic_warning_flag)
  _add_c_compile_flag(-pedantic _has_pedantic_flag)
endif()

# Conform to style.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-Wno-missing-braces _has_no_missing_braces_warning_flag)

# Conflict in stdlib under clang. Enabled in clang only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-Wno-mismatched-tags _has_no_mismatched_tags_warning_flag)

# Clean up boost 1.55 headers. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_c_compile_flag(-Wno-deprecated-declarations _has_no_deprecated_declarations_warning_flag)

# Protect stack.
#------------------------------------------------------------------------------
_add_link_flag(-fstack-protector _has_stack_protector_flag)

# Protect stack comprehensively.
#------------------------------------------------------------------------------
_add_link_flag(-fstack-protector-all _has_stack_protector_all_flag)

# Hide internal functions from external libs. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-fvisibility-hidden _has_visibility_hidden_flag)

# Hide inlines from external libs. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-fvisibility-inlines-hidden _has_visibility_inlines_hidden_flag)

# Target Windows Vista. Enabled in msvc only.
#------------------------------------------------------------------------------
if (MSVC)
  add_definitions(-D_WIN32_WINNT=0x0600)
endif()

# Build
#==============================================================================
function(_group_sources target sources_dir)
  file(GLOB_RECURSE _headers
    ${sources_dir}/include/*.h ${sources_dir}/include/*.hpp)
  target_sources(${target} PRIVATE ${_headers})

  get_target_property(sources ${target} SOURCES)
  foreach (source ${sources})
    get_filename_component(group ${source} ABSOLUTE)
    get_filename_component(group ${group} DIRECTORY)
    file(RELATIVE_PATH group "${sources_dir}" "${group}")
    if (group)
      if (MSVC)
        string(REPLACE "/" "\\" group "${group}")
      endif()
      source_group("${group}" FILES "${source}")
    endif()
  endforeach()

  set_target_properties(${target} PROPERTIES FOLDER "server")
endfunction()

# src/bitprim-server.la => ${libdir}
#------------------------------------------------------------------------------
set(MODE STATIC)
if (ENABLE_SHARED)
  set(MODE SHARED)
endif()

add_library(bitprim-server ${MODE}
  #src/address_key.cpp
  src/configuration.cpp
  src/parser.cpp
  src/server_node.cpp
  src/settings.cpp
  #src/interface/address.cpp
  src/interface/blockchain.cpp
  src/interface/subscribe.cpp
  src/interface/unsubscribe.cpp
  #src/interface/protocol.cpp
  src/interface/transaction_pool.cpp
  src/messages/message.cpp
  src/messages/route.cpp
  src/messages/subscription.cpp
  src/services/block_service.cpp
  src/services/heartbeat_service.cpp
  src/services/query_service.cpp
  src/services/transaction_service.cpp
  #src/utility/authenticator.cpp
  #src/utility/fetch_helpers.cpp
  src/workers/notification_worker.cpp
  src/workers/query_worker.cpp
  src/workers/authenticator.cpp)
target_include_directories(bitprim-server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_compile_definitions(bitprim-server PUBLIC -DBCS_STATIC)
if (NOT MSVC)
  target_compile_definitions(bitprim-server PUBLIC -DSYSCONFDIR=\"${SYSCONFDIR}\")
endif()

if (WITH_LOCAL_MINING)
target_link_libraries(bitprim-server bitprim-node bitprim-protocol bitprim-mining)
else()
  target_link_libraries(bitprim-server bitprim-node bitprim-protocol)
endif()

if (WITH_REMOTE_BLOCKCHAIN)
  target_compile_definitions(bitprim-server PUBLIC -DWITH_REMOTE_BLOCKCHAIN)
endif()

if (WITH_REMOTE_DATABASE)
  target_compile_definitions(bitprim-server PUBLIC -DWITH_REMOTE_DATABASE)
endif()

_group_sources(bitprim-server "${CMAKE_CURRENT_LIST_DIR}")

# Tests
#==============================================================================
function(_add_tests target)
  if (ENABLE_SHARED)
    target_compile_definitions(${target} PRIVATE -DBOOST_TEST_DYN_LINK)
  endif()
  target_include_directories(${target} SYSTEM PUBLIC ${Boost_INCLUDE_DIR})
  target_link_libraries(${target} PUBLIC ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

  foreach (_test_name ${ARGN})
    add_test(
      NAME test.server.${_test_name}
      COMMAND ${target}
        --run_test=${_test_name}
        --show_progress=no
        --detect_memory_leak=0
        --report_level=no
        --build_info=yes)
  endforeach()
endfunction()

if (WITH_TESTS)
  enable_testing()
endif()

# local: test/bitprim_server_test
#------------------------------------------------------------------------------
if (WITH_TESTS)
  add_executable(bitprim_server_test
    test/main.cpp
    test/server.cpp
    test/stress.sh)
  target_link_libraries(bitprim_server_test PUBLIC bitprim-server)
  _group_sources(bitprim_server_test "${CMAKE_CURRENT_LIST_DIR}/test")

  _add_tests(bitprim_server_test
    server_tests)
endif()

# console/bs => ${bindir}
#------------------------------------------------------------------------------
if (WITH_CONSOLE)
  add_executable(console.bs
    console/executor.cpp
    console/executor.hpp
    console/main.cpp)
  target_link_libraries(console.bs bitprim-server)

  set_target_properties(
    console.bs PROPERTIES
    FOLDER "server"
    OUTPUT_NAME bs)
endif()

# Install
#==============================================================================
install(TARGETS bitprim-server
  EXPORT bitprim-server
  ARCHIVE DESTINATION lib)

if (WITH_CONSOLE)
  install(TARGETS console.bs
    RUNTIME DESTINATION bin)
endif()

set(_bitprim_headers
  # include_bitcoin_HEADERS =
  bitcoin/server.hpp
  # include_bitcoin_server_HEADERS =
  bitcoin/server/configuration.hpp
  bitcoin/server/define.hpp
  bitcoin/server/parser.hpp
  bitcoin/server/server_node.hpp
  bitcoin/server/settings.hpp
  bitcoin/server/version.hpp
  # include_bitcoin_server_interface_HEADERS =
#  bitcoin/server/interface/address.hpp
  bitcoin/server/interface/blockchain.hpp
  bitcoin/server/interface/subscribe.hpp
  bitcoin/server/interface/unsubscribe.hpp
#  bitcoin/server/interface/protocol.hpp
  bitcoin/server/interface/transaction_pool.hpp
  # include_bitcoin_server_messages_HEADERS =
  bitcoin/server/messages/message.hpp
  bitcoin/server/messages/route.hpp
  bitcoin/server/messages/subscription.hpp
  # include_bitcoin_server_services_HEADERS =
  bitcoin/server/services/block_service.hpp
  bitcoin/server/services/heartbeat_service.hpp
  bitcoin/server/services/query_service.hpp
  bitcoin/server/services/transaction_service.hpp
  # include_bitcoin_server_utility_HEADERS =
  #bitcoin/server/utility/address_key.hpp
  #bitcoin/server/utility/authenticator.hpp
  #bitcoin/server/utility/fetch_helpers.hpp
  # include_bitcoin_server_workers_HEADERS =
  bitcoin/server/workers/authenticator.hpp
  bitcoin/server/workers/notification_worker.hpp
  bitcoin/server/workers/query_worker.hpp)
foreach (_header ${_bitprim_headers})
  get_filename_component(_directory "${_header}" DIRECTORY)
  install(FILES "include/${_header}" DESTINATION "include/${_directory}")
endforeach()

# Export
#==============================================================================
include(CMakePackageConfigHelpers)

export(EXPORT bitprim-server
  FILE "${CMAKE_CURRENT_BINARY_DIR}/bitprim-serverTargets.cmake")
export(PACKAGE bitprim-server)

if (UNIX)
  set(_config_package_location "lib/cmake")
elseif (WIN32)
  set(_config_package_location "cmake")
endif()

install(EXPORT bitprim-server
  FILE bitprim-serverTargets.cmake
  DESTINATION ${_config_package_location})

configure_file(bitprim-serverConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/bitprim-serverConfig.cmake"
  @ONLY)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/bitprim-serverConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/bitprim-serverConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/bitprim-serverConfigVersion.cmake"
  DESTINATION ${_config_package_location})
