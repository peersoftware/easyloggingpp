cmake_minimum_required(VERSION 3.15)

project(Easyloggingpp CXX)

include(GNUInstallDirs)

option(test "Build all tests" OFF)
option(build_static_lib "Build easyloggingpp as a static library" OFF)
option(lib_utc_datetime "Build library with UTC date/time logging" OFF)
option(no_default_logfile "Do not write to default log file \"myeasylog.log\" (define ELPP_NO_DEFAULT_LOG_FILE)" OFF)
option(thread_safe "Build easyloggingpp thread safe (define ELPP_THREAD_SAFE)" OFF)
option(use_std_threads "Use standard library thread synchronization (define ELPP_FORCE_USE_STD_THREAD)" OFF)
option(is_uwp "The compilation platform is uwp." OFF)
option(use_winsock2 "Use winsock2.h over winsock.h (define ELPP_WINSOCK2)" OFF)

set(ELPP_MAJOR_VERSION "10")
set(ELPP_MINOR_VERSION "0")
set(ELPP_PATCH_VERSION "0")
set(ELPP_VERSION_STRING "${ELPP_MAJOR_VERSION}.${ELPP_MINOR_VERSION}.${ELPP_PATCH_VERSION}")

install(FILES
    src/easylogging++.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT dev)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/easyloggingpp.pc.cmakein
    ${CMAKE_CURRENT_BINARY_DIR}/easyloggingpp.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/easyloggingpp.pc DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

include(CheckIncludeFileCXX)
check_include_file_cxx("execinfo.h" HAVE_EXECINFO)
if (HAVE_EXECINFO)
	add_definitions(-DHAVE_EXECINFO)
endif()

if (build_static_lib)
        add_library(easyloggingpp STATIC
            src/easylogging++.cc
            src/easylogging++.h
        )
        set_property(TARGET easyloggingpp PROPERTY POSITION_INDEPENDENT_CODE ON)

        if (lib_utc_datetime)
            target_compile_definitions(easyloggingpp PUBLIC ELPP_UTC_DATETIME)
        endif()

        if (no_default_logfile)
            target_compile_definitions(easyloggingpp PUBLIC ELPP_NO_DEFAULT_LOG_FILE)
        endif()

        if (thread_safe)
            target_compile_definitions(easyloggingpp PUBLIC ELPP_THREAD_SAFE)
        endif()

        if (use_std_threads)
            target_compile_definitions(easyloggingpp PUBLIC ELPP_FORCE_USE_STD_THREAD)
        endif()

        if (is_uwp)
            target_compile_definitions(easyloggingpp PUBLIC WIN32_LEAN_AND_MEAN ELPP_WINSOCK2)
        endif()

        if (use_winsock2)
            target_compile_definitions(easyloggingpp PUBLIC ELPP_WINSOCK2)
        endif()

        target_compile_definitions(easyloggingpp PUBLIC
            ELPP_VERSION_STRING="${ELPP_VERSION_STRING}"
        )

        target_compile_features(easyloggingpp PRIVATE cxx_std_17)

        target_include_directories(easyloggingpp PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
endif()

export(PACKAGE ${PROJECT_NAME})


########################################## Unit Testing ###################################
if (test)
    find_package(GTest REQUIRED CONFIG)

    enable_testing()

    if (EMSCRIPTEN)
        set(CMAKE_EXE_LINKER_FLAGS "-O2 -s TOTAL_MEMORY=134217728")
    endif()

    add_executable(easyloggingpp-unit-tests
        src/easylogging++.cc
        test/main.cc
    )

    target_compile_features(easyloggingpp-unit-tests PRIVATE cxx_std_17)

    target_compile_definitions(easyloggingpp-unit-tests PUBLIC
        ELPP_VERSION_STRING="${ELPP_VERSION_STRING}"
        ELPP_FEATURE_ALL
        ELPP_LOGGING_FLAGS_FROM_ARG
        ELPP_NO_DEFAULT_LOG_FILE
        ELPP_FRESH_LOG_FILE
        ELPP_STOP_ON_FIRST_ASSERTION
        ELPP_STL_LOGGING
        ELPP_FORCE_ENV_VAR_FROM_BASH
        ELPP_ENABLE_THREADING
        ELPP_FEATURE_CRASH_LOG
    )

    check_include_file_cxx("syslog.h" HAVE_SYSLOG_H)
    if (HAVE_SYSLOG_H)
        target_compile_definitions(easyloggingpp-unit-tests PUBLIC ELPP_SYSLOG)
    endif()

    # Standard linking to gtest stuff.
    target_link_libraries(easyloggingpp-unit-tests GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(easyloggingpp-unit-tests)
endif()

set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
install(TARGETS easyloggingpp EXPORT ${TARGETS_EXPORT_NAME})

include(CMakePackageConfigHelpers)
set(CONFIG_PACKAGE_LOCATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(TARGET_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
set(TARGET_VERSION_CONFIG
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
)

install(EXPORT "${TARGETS_EXPORT_NAME}"
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION "${CONFIG_PACKAGE_LOCATION}"
)

write_basic_package_version_file(
	"${TARGET_VERSION_CONFIG}"
	VERSION ${ELPP_VERSION_STRING}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
	"${TARGET_CONFIG}"
	INSTALL_DESTINATION "${CONFIG_PACKAGE_LOCATION}"
)

install(FILES
	"${TARGET_CONFIG}"
	"${TARGET_VERSION_CONFIG}"
	DESTINATION "${CONFIG_PACKAGE_LOCATION}"
)
