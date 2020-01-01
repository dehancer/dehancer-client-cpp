if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.env)
    file (WRITE ".env" "DEHANCERD_URL=https://update.dehancer.com/v1/api")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.env)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.env ${CMAKE_CURRENT_BINARY_DIR}/.env COPYONLY)
endif()

set (TEST client-download-test)
include(../CMakeCommon.in)