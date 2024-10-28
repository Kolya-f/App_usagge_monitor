cmake_minimum_required(VERSION 3.16)

project(App_usage_monitor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")


# Найти Qt Widgets, Multimedia и MultimediaWidgets
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

# Додавання QXlsx бібліотеки для роботи з Excel
add_subdirectory("D:/Fails/QQtd/libs/QXlsx-master/QXlsx" "${CMAKE_BINARY_DIR}/build_qxlsx")

# Підключення директорії з інклудами для QXlsx
include_directories("D:/Fails/QQtd/libs/QXlsx-master/QXlsx")

# Джерела проекту
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(App_usage_monitor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        gradient.h
        gradient.cpp
        threads.h
        eastor_egg.h
        eastor_egg.cpp
    )
else()
    if(ANDROID)
        add_library(App_usage_monitor SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(App_usage_monitor
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Зв'язування додатка з бібліотеками Qt и QXlsx
target_link_libraries(App_usage_monitor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::MultimediaWidgets QXlsx)

set_target_properties(App_usage_monitor PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS App_usage_monitor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(App_usage_monitor)
endif()
