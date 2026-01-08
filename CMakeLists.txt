cmake_minimum_required(VERSION 3.16)

project(econom VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        matrixsolver.cpp
        matrixsolver.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(econom
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(econom
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(econom PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


if(MINGW OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_link_options(econom PRIVATE "-Wl,--allow-multiple-definition")
endif()

set_target_properties(econom PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.econom
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS econom
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(econom)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(econom)
endif()
