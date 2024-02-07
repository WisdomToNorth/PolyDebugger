
set(NGPOLY_LIB_FILES
    ngpolygonset.h
    ngpolygonset.cpp
)

add_library(NGPOLY STATIC ${NGPOLY_LIB_FILES})

target_include_directories(NGPOLY PRIVATE)

set(NGPOLY_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/build/include/ngpoly)
if (EXISTS ${NGPOLY_INCLUDE_DIR})
    file(REMOVE_RECURSE ${NGPOLY_INCLUDE_DIR})
endif()

set(NGPOLY_HEADERS
ngpolygonset.h
)

foreach(NGPOLY_HEADER ${NGPOLY_HEADERS})
    file(COPY ${NGPOLY_HEADER} DESTINATION ${NGPOLY_INCLUDE_DIR})
endforeach()

