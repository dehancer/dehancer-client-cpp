
include(../CMakeCommon.in)

set (TEST user-defaults-test)

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