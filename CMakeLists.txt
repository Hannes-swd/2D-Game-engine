cmake_minimum_required(VERSION 3.20)
project(2DGameEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Output alles in den build/ Ordner
# ---------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------------------------------------------------------------------------
# Alle .cpp Dateien im src/ Ordner automatisch finden (rekursiv)
# ---------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.cc"
)

# ---------------------------------------------------------------------------
# Assets automatisch in den Build-Ordner kopieren (ohne manuelle Einträge)
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message(STATUS "Assets werden kopiert nach: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
endif()

# ---------------------------------------------------------------------------
# Raylib 6.0 via FetchContent herunterladen
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        6.0
    GIT_SHALLOW    TRUE
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------------------------
# ImGui (neueste) + rlImGui Binding für Raylib herunterladen
# ---------------------------------------------------------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking   # neuester docking-branch (empfohlen für Tools)
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    rlimgui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(rlimgui)

# ImGui als eigene Library bauen
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

target_link_libraries(imgui PUBLIC raylib)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    imgui
)

# ---------------------------------------------------------------------------
# Platform-spezifische Libraries
# ---------------------------------------------------------------------------
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY}
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        GL m pthread dl rt X11
    )
endif()

# ---------------------------------------------------------------------------
# Assets nach jedem Build neu kopieren (falls geändert)
# ---------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Build fertig: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Kopiere Assets..."
    )
endif()

message(STATUS "=== 2DGameEngine Konfiguration ===")
message(STATUS "Quellen:    ${SOURCES}")
message(STATUS "Build-Dir:  ${CMAKE_BINARY_DIR}")
message(STATUS "Bin-Dir:    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")