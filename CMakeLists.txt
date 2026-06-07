cmake_minimum_required(VERSION 3.16)
project(BoneFertilizerApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic compilation tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Include directories so your GUI can find your Core headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)

# Optionally find LinguistTools for translations
find_package(Qt6LinguistTools QUIET)

# Translation files
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/i18n/bonechain_en.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/i18n/bonechain_ar.ts
)

# Define your source files
set(PROJECT_SOURCES
    GUI/main.cpp
    GUI/mainwindow.cpp
    GUI/mainwindow.h
    GUI/userdialog.cpp
    GUI/userdialog.h
    GUI/supplierdialog.cpp
    GUI/supplierdialog.h
    GUI/productdialog.cpp
    GUI/productdialog.h
    GUI/articledialog.cpp
    GUI/articledialog.h
    GUI/articlereaderdialog.cpp
    GUI/articlereaderdialog.h
    GUI/customerdialog.cpp
    GUI/customerdialog.h
    GUI/orderstatusdialog.cpp
    GUI/orderstatusdialog.h
    GUI/deliverydialog.cpp
    GUI/deliverydialog.h
)

# Create the executable
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})

# Link Qt libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# Process translation files if LinguistTools is available
if(Qt6LinguistTools_FOUND)
    qt_add_translations(${PROJECT_NAME} TS_FILES ${TS_FILES})
else()
    message(STATUS "Qt6 LinguistTools not found — skipping .qm generation. Run lupdate/lrelease manually.")
endif()
