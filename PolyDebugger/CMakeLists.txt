

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
  message(STATUS "Qt windows")
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

include_directories(${PROJECT_SOURCE_DIR}/build/include
                    ${PROJECT_SOURCE_DIR}/PolyDebugger)

add_executable(PolyGui
    offsetisland/plineoffsetislandsalgorithmview.cpp
    offsetisland/plineoffsetislandsalgorithmview.h
    offsetisland/PlineOffsetIslandsScene.qml

    combine/plinecombinealgorithmview.cpp
    combine/plinecombinealgorithmview.h
    combine/PlineCombineScene.qml

    settings/settings.h
    settings/settings.cpp
    settings/DebuggerSettings.qml

    offset/plineoffsetalgorithmview.cpp
    offset/plineoffsetalgorithmview.h
    offset/rawoffsetsegmentsnode.cpp
    offset/rawoffsetsegmentsnode.h
    offset/PlineOffsetScene.qml
    offset/spatialindexboundingboxesnode.h

    adaptor/geometrycanvasitem.cpp
    adaptor/geometrycanvasitem.h
    adaptor/plinesegmentnode.cpp
    adaptor/plinesegmentnode.h
    adaptor/pointsetnode.cpp
    adaptor/pointsetnode.h
    adaptor/viewmodel.h
    adaptor/viewmodel.cpp

    casedata.cpp
    casedata.h
    datamanager.h
    datamanager.cpp

    flatcolorgeometrynode.cpp
    flatcolorgeometrynode.h
    graphicshelpers.cpp
    graphicshelpers.h
    main.cpp
    simplecirclenode.cpp
    simplecirclenode.h

    GeometrySceneView.qml
    main.qml
    qml.qrc
)

target_compile_definitions(PolyGui
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(PolyGui
  PRIVATE ${Qt_LIBRARIES})
