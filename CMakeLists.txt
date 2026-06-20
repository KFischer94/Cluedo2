cmake_minimum_required(VERSION 3.21)

project(CluedoBoardConfigurator
    VERSION 1.0.0
    DESCRIPTION "Cluedo Board Configuration Tool"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt6 auto-tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Windows: suppress console window in Release builds
if(WIN32)
    set(APP_TYPE WIN32)
endif()

# ── Find Qt6 ──────────────────────────────────────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

# ── Sources ───────────────────────────────────────────────────────────────────
set(MODEL_SOURCES
    src/model/CellState.h
    src/model/GridModel.h
    src/model/GridModel.cpp
    src/model/Room.h
    src/model/Room.cpp
    src/model/GamePiece.h
    src/model/GamePiece.cpp
    src/model/DoorConnection.h
    src/model/BoardConfig.h
    src/model/BoardConfig.cpp
)

set(VIEW_SOURCES
    src/view/MainWindow.h
    src/view/MainWindow.cpp
    src/view/BoardView.h
    src/view/BoardView.cpp
    src/view/GridOverlay.h
    src/view/GridOverlay.cpp
    src/view/ToolBar.h
    src/view/ToolBar.cpp
    src/view/GridSettingsPanel.h
    src/view/GridSettingsPanel.cpp
    src/view/RoomDialog.h
    src/view/RoomDialog.cpp
    src/view/PieceDialog.h
    src/view/PieceDialog.cpp
    src/view/DoorDialog.h
    src/view/DoorDialog.cpp
)

set(CONTROLLER_SOURCES
    src/controller/GridController.h
    src/controller/GridController.cpp
    src/controller/PaintController.h
    src/controller/PaintController.cpp
    src/controller/ConfigExporter.h
    src/controller/ConfigExporter.cpp
)

set(COMMAND_SOURCES
    src/commands/PaintCellCommand.h
    src/commands/PaintCellCommand.cpp
    src/commands/AssignDoorCommand.h
    src/commands/AssignDoorCommand.cpp
)

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${APP_TYPE}
    src/main.cpp
    ${MODEL_SOURCES}
    ${VIEW_SOURCES}
    ${CONTROLLER_SOURCES}
    ${COMMAND_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# ── Compiler warnings ─────────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── Windows: deploy Qt DLLs next to the executable (optional helper) ──────────
if(WIN32)
    find_program(WINDEPLOYQT windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT}
                --no-translations
                --no-system-d3d-compiler
                $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Deploying Qt runtime DLLs..."
        )
    endif()
endif()
