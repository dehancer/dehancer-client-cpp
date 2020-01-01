# Finds the C++ (RXCPP) Library
#
#  RXCPP_INCLUDE_DIRS   - Directory to include to get RXCPP headers

SET(_RXCPP_REQUIRED_VARS  RXCPP_INCLUDE_DIR RXCPP_INCLUDE_DIRS )

#
### FIRST STEP: Find the headers.
#
FIND_PATH(
        RXCPP_INCLUDE_DIR rxcpp/rx.hpp
        PATH "${RXCPP_SOURCE_DIR}"
        DOC "rxcpp include directory")
MARK_AS_ADVANCED(RXCPP_INCLUDE_DIR)

SET(RXCPP_INCLUDE_DIRS
        ${RXCPP_INCLUDE_DIR}/Rx/CPP/src
        ${RXCPP_INCLUDE_DIR}/Ix/CPP/src
        ${RXCPP_INCLUDE_DIR})