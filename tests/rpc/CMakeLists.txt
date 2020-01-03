if(NOT EXISTS .env)
    file (WRITE ".env" "DEHANCERD_URL=https://update.dehancer.com/v1/api")
endif()

if(EXISTS .env)
    configure_file(.env ${CMAKE_CURRENT_BINARY_DIR}/camera/.env COPYONLY)
endif()

include(../CMakeCommon.in)

add_executable(login-test login.cpp)
add_executable(set-info set_info.cpp)

target_link_libraries(
        login-test
        ${PROJECT_LIB}
        ${TEST_LIBRARIES}
        ${COMMON_LIBRARIES}
)

target_link_libraries(
        set-info
        ${PROJECT_LIB}
        ${TEST_LIBRARIES}
        ${COMMON_LIBRARIES}
)

add_test(test login-test set-info)

if (COMMON_DEPENDECIES)
    add_dependencies(
            login-test
            ${COMMON_DEPENDECIES}
    )
    add_dependencies(
            set-info
            ${COMMON_DEPENDECIES}
    )
endif ()

enable_testing()
