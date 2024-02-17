cmake_minimum_required(VERSION 3.15)
project(Clipper2 VERSION 1.3.0 LANGUAGES C CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 17)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# CLIPPER2_HI_PRECISION: See GetIntersectPoint() in clipper.core.h
option(CLIPPER2_HI_PRECISION "Caution: enabling this will compromise performance" OFF)

option(CLIPPER2_UTILS "Build utilities" ON)
option(CLIPPER2_EXAMPLES "Build examples" ON)
option(CLIPPER2_TESTS "Build tests" ON)
option(USE_EXTERNAL_GBENCHMARK "Use the googlebenchmark" OFF)
option(BUILD_SHARED_LIBS "Build shared libs" OFF)
set(CLIPPER2_USINGZ "ON" CACHE STRING "Build Clipper2Z, either \"ON\" or \"OFF\" or \"ONLY\"")

# CLIPPER2_MAX_DECIMAL_PRECISION: maximum decimal precision when scaling PathsD to Paths64.
# Caution: excessive scaling will increase the likelihood of integer overflow errors.
set(CLIPPER2_MAX_DECIMAL_PRECISION 8 CACHE STRING "Maximum decimal precision range")


if (APPLE)
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
endif ()

include(GNUInstallDirs)
set(CLIPPER2_INC_FOLDER ${PROJECT_SOURCE_DIR}/Clipper2Lib/include/clipper2)
configure_file(clipper.version.in
  ${CLIPPER2_INC_FOLDER}/clipper.version.h NEWLINE_STYLE UNIX)

set(CLIPPER2_INC
  ${CLIPPER2_INC_FOLDER}/clipper.h
  ${CLIPPER2_INC_FOLDER}/clipper.version.h
  ${CLIPPER2_INC_FOLDER}/clipper.core.h
  ${CLIPPER2_INC_FOLDER}/clipper.engine.h
  ${CLIPPER2_INC_FOLDER}/clipper.export.h
  ${CLIPPER2_INC_FOLDER}/clipper.minkowski.h
  ${CLIPPER2_INC_FOLDER}/clipper.offset.h
  ${CLIPPER2_INC_FOLDER}/clipper.rectclip.h
)

set(CLIPPER2_SRC
  Clipper2Lib/src/clipper.engine.cpp
  Clipper2Lib/src/clipper.offset.cpp
  Clipper2Lib/src/clipper.rectclip.cpp
)

set(CLIPPER2_LIBS "") # one or both of Clipper2/Clipper2Z

# primary Clipper2 library
if (NOT (CLIPPER2_USINGZ STREQUAL "ONLY"))
  list(APPEND CLIPPER2_LIBS Clipper2)
  add_library(Clipper2 ${CLIPPER2_INC} ${CLIPPER2_SRC})

  target_compile_definitions(
    Clipper2 PUBLIC
    CLIPPER2_MAX_DECIMAL_PRECISION=${CLIPPER2_MAX_DECIMAL_PRECISION}
    $<$<BOOL:${CLIPPER2_HI_PRECISION}>:CLIPPER2_HI_PRECISION>
  )


  target_include_directories(Clipper2
    PUBLIC Clipper2Lib/include
  )

  if (MSVC)
    target_compile_options(Clipper2 PRIVATE /W4 /WX)
  else()
    target_compile_options(Clipper2 PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_link_libraries(Clipper2 PUBLIC -lm)
  endif()
endif()

# secondary Clipper2 library with USINGZ defined (if required)
if (NOT (CLIPPER2_USINGZ STREQUAL "OFF"))
  list(APPEND CLIPPER2_LIBS Clipper2Z)
  add_library(Clipper2Z ${CLIPPER2_INC} ${CLIPPER2_SRC})

  target_compile_definitions(
    Clipper2Z PUBLIC
    USINGZ
    CLIPPER2_MAX_DECIMAL_PRECISION=${CLIPPER2_MAX_DECIMAL_PRECISION}
    $<$<BOOL:${CLIPPER2_HI_PRECISION}>:CLIPPER2_HI_PRECISION>
  )

  target_include_directories(Clipper2Z
    PUBLIC Clipper2Lib/include
  )

  if (MSVC)
    target_compile_options(Clipper2Z PRIVATE /W4 /WX)
  else()
    target_compile_options(Clipper2Z PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_link_libraries(Clipper2Z PUBLIC -lm)
  endif()
endif()

set_target_properties(${CLIPPER2_LIBS} PROPERTIES FOLDER Libraries
                                         VERSION ${PROJECT_VERSION}
                                         SOVERSION ${PROJECT_VERSION_MAJOR}
                                         PUBLIC_HEADER "${CLIPPER2_INC}"
)

if(CLIPPER2_UTILS OR CLIPPER2_TESTS OR CLIPPER2_EXAMPLES)
  set(CLIPPER2_UTILS_INC
    Utils/clipper.svg.h
    Utils/ClipFileLoad.h
    Utils/ClipFileSave.h
    Utils/Timer.h
    Utils/Colors.h
    Utils/CommonUtils.h
  )
  set(CLIPPER2_UTILS_SRC
    Utils/clipper.svg.cpp
    Utils/ClipFileLoad.cpp
    Utils/ClipFileSave.cpp
  )
  set(CLIPPER2_UTILS "") # one or both of Clipper2utils/Clipper2Zutils

  if (NOT (CLIPPER2_USINGZ STREQUAL "ONLY"))
    list(APPEND CLIPPER2_UTILS Clipper2utils)
    add_library(Clipper2utils STATIC ${CLIPPER2_UTILS_INC} ${CLIPPER2_UTILS_SRC})

    target_link_libraries(Clipper2utils PUBLIC Clipper2)
    target_include_directories(Clipper2utils
      PUBLIC Utils
    )
  endif()

  if (NOT (CLIPPER2_USINGZ STREQUAL "OFF"))
    list(APPEND CLIPPER2_UTILS Clipper2Zutils)
    add_library(Clipper2Zutils STATIC ${CLIPPER2_UTILS_INC} ${CLIPPER2_UTILS_SRC})

    target_link_libraries(Clipper2Zutils PUBLIC Clipper2Z)
    target_include_directories(Clipper2Zutils
      PUBLIC Utils
    )
  endif()

  set_target_properties(${CLIPPER2_UTILS} PROPERTIES FOLDER Libraries)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    foreach(lib ${CLIPPER2_UTILS})
      target_compile_options(${lib} PRIVATE -Wno-unused-variable -Wno-unused-function)
    endforeach()
  endif()
endif()

if(CLIPPER2_EXAMPLES)
  ##########################################################################
  ##########################################################################

  set(ALL_EXAMPLES "") # 2d and 3d examples (if enabled)

  if (NOT (CLIPPER2_USINGZ STREQUAL "ONLY"))
    set(EXAMPLES
      Benchmarks
      Inflate
      MemLeakTest
      PolygonSamples
      RandomClipping
      UnionClipping
      RectClipping
      SimpleClipping
      VariableOffset
    )

    foreach(ex ${EXAMPLES})
      add_executable(${ex} Examples/${ex}/${ex}.cpp)
      target_link_libraries(${ex} PRIVATE Clipper2 Clipper2utils)
    endforeach()

    file(COPY Examples/Inflate/rabbit.svg DESTINATION ${CMAKE_BINARY_DIR} FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )
    list(APPEND ALL_EXAMPLES ${EXAMPLES})
  endif()

  if (NOT (CLIPPER2_USINGZ STREQUAL "OFF"))
    set(EXAMPLESZ "UsingZ")
    foreach(ex ${EXAMPLESZ})
      add_executable(${ex} Examples/${ex}/${ex}.cpp)
      target_link_libraries(${ex} PRIVATE Clipper2Z Clipper2Zutils)
    endforeach()

    list(APPEND ALL_EXAMPLES ${EXAMPLESZ})
  endif()

  set_target_properties(${ALL_EXAMPLES} PROPERTIES FOLDER Examples)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    foreach(ex ${ALL_EXAMPLES})
      target_compile_options(${ex} PRIVATE -Wno-unused-variable -Wno-unused-function)
    endforeach()
  endif()
endif()

if(USE_EXTERNAL_GBENCHMARK)
  add_subdirectory(BenchMark)
endif()

set(CLIPPER2_PCFILES "")
foreach(lib ${CLIPPER2_LIBS})
  set(pc "${CMAKE_CURRENT_BINARY_DIR}/${lib}.pc")
  list(APPEND CLIPPER2_PCFILES ${pc})
  CONFIGURE_FILE(Clipper2.pc.cmakein "${pc}" @ONLY)
endforeach()

install(TARGETS ${CLIPPER2_LIBS}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/clipper2
)
install(FILES ${CLIPPER2_PCFILES} DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# disable exceptions
#string(REGEX REPLACE "/W[3|4]" "/w" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#add_definitions(-D_HAS_EXCEPTIONS=0) # for STL
