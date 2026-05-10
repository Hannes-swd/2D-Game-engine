cmake_minimum_required(VERSION 3.20)
project(2DGameEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Windows-spezifische Definitionen (global, aber OHNE NOGDI/VC_EXTRALEAN!)
# NOGDI würde GDI-Typen entfernen die raylib/GLFW intern braucht.
# ---------------------------------------------------------------------------
if(WIN32)
    add_compile_definitions(
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Output alles in den build/ Ordner
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Alle .cpp Dateien finden
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)
list(FILTER SOURCES EXCLUDE REGEX ".*/build/.*")

# Assets kopieren
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message(STATUS "Assets werden kopiert nach: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
endif()

# ---------------------------------------------------------------------------
# Libraries via FetchContent
# ---------------------------------------------------------------------------
include(FetchContent)

# Raylib 6.0
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        6.0
    GIT_SHALLOW    TRUE
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# ImGui (docking branch)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

# rlImGui Binding
FetchContent_Declare(
    rlimgui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(rlimgui)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# ---------------------------------------------------------------------------
# ImGui als eigene Library bauen
# ---------------------------------------------------------------------------
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

target_link_libraries(imgui_lib PUBLIC raylib)

# Debug-Ausgabe
message(STATUS "Gefundene Quell-Dateien:")
foreach(SOURCE_FILE ${SOURCES})
    message(STATUS "  - ${SOURCE_FILE}")
endforeach()

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/items
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    imgui_lib
    nlohmann_json::nlohmann_json
)

# Platform-spezifische Libraries
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

# Assets nach jedem Build neu kopieren
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Kopiere Assets..."
    )
endif()

# ---------------------------------------------------------------------------
# Windows: NOGDI + VC_EXTRALEAN NUR für eigenes Target (nicht raylib!)
# raylib/GLFW braucht GDI-Typen (DEVMODEW, PIXELFORMATDESCRIPTOR, etc.)
# ---------------------------------------------------------------------------
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        NOGDI
        VC_EXTRALEAN
    )
endif()

message(STATUS "=== 2DGameEngine Konfiguration ===")
message(STATUS "Quellen:    ${SOURCES}")
message(STATUS "Build-Dir:  ${CMAKE_BINARY_DIR}")
message(STATUS "Bin-Dir:    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
