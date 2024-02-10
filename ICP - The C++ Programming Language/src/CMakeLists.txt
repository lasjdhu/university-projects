cmake_minimum_required(VERSION 3.5)

project(icp_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

function(make_paths_relative target)
    get_target_property(target_sources ${target} SOURCES)
    foreach(source ${target_sources})
        get_source_file_property(source_location ${source} LOCATION)
        file(RELATIVE_PATH relative_source_location ${CMAKE_SOURCE_DIR} ${source_location})
        set_source_files_properties(${source} PROPERTIES LOCATION ${relative_source_location})
    endforeach()
endfunction()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    Model/GameModel.cpp
    Model/MapModel.cpp
    Model/CharacterModel.cpp
    Model/Model.h
    View/GameWidget.cpp
    View/ReplayWidget.cpp
    View/EndWidget.cpp
    View/Widget.h
    View/mainwindow.cpp
    View/mainwindow.h
    View/mainwindow.ui
    Controller/GameController.cpp
    Controller/InterfaceController.cpp
    Controller/Controller.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(icp_project
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET icp_project APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(icp_project SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(icp_project
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(icp_project PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(icp_project PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS icp_project
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(icp_project)
endif()
