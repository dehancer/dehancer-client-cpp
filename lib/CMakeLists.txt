include(CMakePackageConfigHelpers)

set(CMAKE_INSTALL_PREFIX /usr/local/${PROJECT_LIB})

if (APPLE)

    include(macos/Cmake.inc)

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    # Linux & FreeBSD
    set_target_properties(${CURRENT_TARGET_NAME} PROPERTIES LINK_FLAGS "-Wl,-fvisibility=hidden,--version-script=${OFX_SUPPORT_HEADER_DIR}/linuxSymbols")
    set_target_properties(${CURRENT_TARGET_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../../Libraries")

endif ()


include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ../include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
        ${EXTERNAL_LOCATION}
)

FILE(GLOB PUBLIC_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/include/dehancer/*.h

        )

FILE(GLOB PUBLIC_CLIENT_INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/include/dehancer/client/*.h
        )

FILE(GLOB INCLUDE_FILES
        ${PUBLIC_INCLUDE_FILES}
        )

set(PLATFORM_SOURCES)

if (APPLE)
    FILE(GLOB PLATFORM_SOURCES
            ../src/client/platform/*.mm
            ../src/client/platform/*.cpp
            ../src/client/platform/macos/*.mm
            ../src/client/platform/macos/*.cpp
            )
endif()

FILE(GLOB SOURCES
        ../src/*.cpp
        ../src/client/*.cpp
        ../src/client/film/*.cpp
        ../src/client/camera/*.cpp
        ${PLATFORM_SOURCES}
        )

add_library(
        ${PROJECT_LIB}
        ${SOURCES})

target_link_libraries (
        ${PROJECT_LIB}  PUBLIC
)

target_include_directories(
        ${PROJECT_LIB}
        PUBLIC
        ../include
)

if (COMMON_DEPENDECIES)
    add_dependencies(${PROJECT_LIB} ${COMMON_DEPENDECIES})
endif ()

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}")
set(namespace "${PROJECT_NAME}::")

write_basic_package_version_file(
        "${version_config}" COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
        "cmake/Config.cmake.in"
        "${project_config}"
        INSTALL_DESTINATION "${config_install_dir}"
)

install(TARGETS ${PROJECT_LIB}   DESTINATION lib)
install(FILES   ${PUBLIC_INCLUDE_FILES} DESTINATION include/dehancer)
install(FILES   ${PUBLIC_CLIENT_INCLUDE_FILES} DESTINATION include/dehancer/client)
install(FILES   ${project_config} ${version_config} DESTINATION lib/cmake/${PROJECT_LIB})
install(FILES cmake/${PROJECT_LIB}.cmake DESTINATION lib/cmake/${PROJECT_LIB})