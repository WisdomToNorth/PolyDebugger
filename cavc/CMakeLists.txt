set(CAVC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/build/include/cavc)
set(CAVC_INTERNAL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/build/include/cavc/internal)

message(STATUS "CAVC_INCLUDE_DIR: ${CAVC_INCLUDE_DIR}")
message(STATUS "CAVC_INTERNAL_INCLUDE_DIR: ${CAVC_INTERNAL_INCLUDE_DIR}")
if (EXISTS ${CAVC_INCLUDE_DIR})
    file(REMOVE_RECURSE ${CAVC_INCLUDE_DIR})
endif()

set(CAVC_HEADERS
    intrlineseg2circle2.hpp
    intrlineseg2lineseg2.hpp
    mathutils.hpp
    intrcircle2circle2.hpp
    vector.hpp
    plinesegment.hpp
    polyline.hpp
    polylinecombine.hpp
    polylineintersects.hpp
    polylineoffset.hpp
    polylineoffsetislands.hpp
    staticspatialindex.hpp
    vector2.hpp
)

foreach(CAVC_HEADER ${CAVC_HEADERS})
    file(COPY ${CAVC_HEADER} DESTINATION ${CAVC_INCLUDE_DIR})
endforeach()

set(CAVC_INTERNAL_HEADERS
    internal/diagnostics.hpp
    internal/common.hpp
)

foreach(CAVC_INTERNAL_HEADER ${CAVC_INTERNAL_HEADERS})
    file(COPY ${CAVC_INTERNAL_HEADER} DESTINATION ${CAVC_INTERNAL_INCLUDE_DIR})
endforeach()
