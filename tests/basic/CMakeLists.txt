if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.env)
    file (WRITE ".env" "DEHANCERD_URL=https://update.dehancer.com/v1/api")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.env)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.env ${CMAKE_CURRENT_BINARY_DIR}/.env COPYONLY)
endif()

include(../CMakeCommon.in)

set (TEST basic-connection-test)

file(GLOB TESTS_SOURCES ${TESTS_SOURCES}
        ./*.cpp
        )

add_executable(${TEST} ${TESTS_SOURCES})

target_link_libraries(
        ${TEST}
        ${PROJECT_LIB}
        ${TEST_LIBRARIES}
        ${COMMON_LIBRARIES}
)

add_test(test ${TEST})
if (COMMON_DEPENDECIES)
    add_dependencies(
            ${TEST}
            ${COMMON_DEPENDECIES}
    )
endif ()

enable_testing()
