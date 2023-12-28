set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

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
  PRIVATE Qt5::Core Qt5::Quick)
