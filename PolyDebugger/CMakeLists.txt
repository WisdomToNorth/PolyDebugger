set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
  message(STATUS "Qt windows")
  include(D:/OpenSource/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5/Qt5Config.cmake)
  find_package(Qt5 REQUIRED COMPONENTS Core Quick Gui)
  set(Qt_LIBRARIES Qt5::Core Qt5::Quick Qt5::Gui)
  set(Qt_LIBRARY_DIR ${Qt5_LIBRARY_DIR})

elseif(APPLE)
    message(STATUS "Qt Apple")
    find_package(Qt6 REQUIRED COMPONENTS Core Quick Gui)
    set(Qt_LIBRARIES Qt6::Core Qt6::Quick Qt6::Gui)
    set(Qt_LIBRARY_DIR ${Qt6_LIBRARY_DIR})
    set(Qt_INCLUDE_DIR ${Qt6_INCLUDE_DIRS})

elseif(UNIX)
  message(STATUS "Qt linux")
  find_package(Qt5 REQUIRED COMPONENTS Core Quick Gui)
  set(Qt_LIBRARIES Qt5::Core Qt5::Quick Qt5::Gui)
  set(Qt_LIBRARY_DIR ${Qt5_LIBRARY_DIR})
endif()

include_directories(${PROJECT_SOURCE_DIR}/build/include)

add_executable(InteractiveUI
    main.cpp
    geometrycanvasitem.cpp
    plineoffsetalgorithmview.cpp
    plinecombinealgorithmview.cpp
    plineoffsetislandsalgorithmview.cpp
    demofuncs.cpp
    graphicshelpers.cpp
    polylinenode.cpp
    spatialindexboundingboxesnode.cpp
    flatcolorgeometrynode.cpp
    rawoffsetsegmentsnode.cpp
    plinesegmentnode.cpp
    simplecirclenode.cpp
    pointsetnode.cpp
    qml.qrc)

#target_include_directories(InteractiveUI PUBLIC ${PROJECT_SOURCE_DIR}/build/include)

target_compile_definitions(InteractiveUI
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(InteractiveUI
  PRIVATE ${Qt_LIBRARIES})
