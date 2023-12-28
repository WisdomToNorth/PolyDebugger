
set(CLIPPER2_LIB_FILES
    clipper.engine.cpp
    clipper.rectclip.cpp
    clipper.offset.cpp
    clipper.h
    clipper.minkowski.h
    clipper.core.h
    clipper.export.h
    clipper.engine.h
    clipper.rectclip.h
    clipper.version.h
    clipper.offset.h
)

add_library(CLIPPER2 STATIC ${CLIPPER2_LIB_FILES})

target_include_directories(CLIPPER2 PRIVATE)

set(CLIPPER2_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/build/include/clipper2)
if (EXISTS ${CLIPPER2_INCLUDE_DIR})
    file(REMOVE_RECURSE ${CLIPPER2_INCLUDE_DIR})
endif()

set(CLIPPER2_HEADERS
    clipper.h
    clipper.minkowski.h
    clipper.core.h
    clipper.export.h
    clipper.engine.h
    clipper.rectclip.h
    clipper.version.h
    clipper.offset.h
)

foreach(CLIPPER2_HEADER ${CLIPPER2_HEADERS})
    file(COPY ${CLIPPER2_HEADER} DESTINATION ${CLIPPER2_INCLUDE_DIR})
endforeach()

