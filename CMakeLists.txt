cmake_minimum_required(VERSION 3.16)
project(BoneFertilizerApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic compilation tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
find_package(Qt6 QUIET COMPONENTS Svg SvgWidgets)

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
    GUI/ThemeManager.cpp
    GUI/ThemeManager.h
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

# Windows resource file for app icon
set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
if(WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${APP_ICON_RESOURCE})
endif()

# Create the executable
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})

# Link Qt libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
if(Qt6Svg_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Svg Qt6::SvgWidgets)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_QT_SVG)
endif()

# Process translation files if LinguistTools is available
if(Qt6LinguistTools_FOUND)
    qt_add_translations(${PROJECT_NAME} TS_FILES ${TS_FILES})
else()
    message(STATUS "Qt6 LinguistTools not found — skipping .qm generation. Run lupdate/lrelease manually.")
endif()

# ──────────────────────────────────────────────
# Install rules — copies files next to the .exe
# ──────────────────────────────────────────────
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY Data/   DESTINATION Data)
install(DIRECTORY Themes/ DESTINATION Themes)
install(DIRECTORY Assets/ DESTINATION Assets)
install(DIRECTORY i18n/   DESTINATION i18n)

# ──────────────────────────────────────────────
# CPack — NSIS installer for Windows
# ──────────────────────────────────────────────
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "Bone Fertilizer OS")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "BoneChain")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fertilizer OS - Enterprise Edition")
set(CPACK_PACKAGE_EXECUTABLES "BoneFertilizerApp" "Bone Fertilizer OS")
set(CPACK_CREATE_DESKTOP_LINKS "BoneFertilizerApp")
set(CPACK_NSIS_DISPLAY_NAME "Bone Fertilizer OS")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_MENU_LINKS
    "bin/BoneFertilizerApp.exe" "Bone Fertilizer OS"
)
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  CreateShortCut '$SMSTARTUP\\\\Bone Fertilizer OS.lnk' '$INSTDIR\\\\bin\\\\BoneFertilizerApp.exe'
")
set(CPACK_NSIS_CREATE_ICONS_EXTRA "
  CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Uninstall Bone Fertilizer OS.lnk' '$INSTDIR\\\\Uninstall.exe'
")

include(CPack)
