cmake_minimum_required(VERSION 3.20)
project(2DGameEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_compile_definitions(
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ── Quellen: src/ + items/ ────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/items/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/items/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/items/*.cxx"
)

message(STATUS "Gefundene Quell-Dateien:")
foreach(SOURCE_FILE ${SOURCES})
    message(STATUS "  - ${SOURCE_FILE}")
endforeach()

# ── Assets beim Configure einmalig kopieren (nur wenn Ziel noch nicht existiert) ──
# Wird beim ersten cmake .. ausgeführt – danach übernimmt POST_BUILD
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    if(NOT EXISTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
        file(COPY "${CMAKE_SOURCE_DIR}/assets"
             DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        message(STATUS "Assets (einmalig) -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
    else()
        message(STATUS "Assets bereits vorhanden, überspringe Configure-Kopie")
    endif()
endif()

# ── FetchContent ──────────────────────────────────────────────────────────────
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

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
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

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# ── ImGui Library ─────────────────────────────────────────────────────────────
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

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src      # alle Header in src/
    ${CMAKE_SOURCE_DIR}/items    # items können src-Header finden
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    imgui_lib
    nlohmann_json::nlohmann_json
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
    # NOGDI/VC_EXTRALEAN NUR für eigenes Target, nicht für raylib
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        NOGDI
        VC_EXTRALEAN
    )
elseif(APPLE)
    find_library(COCOA_LIBRARY    Cocoa)
    find_library(IOKIT_LIBRARY    IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY}
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        GL m pthread dl rt X11
    )
endif()

# ── POST_BUILD: Assets kopieren, Spielstände schützen ────────────────────────
# Das externe Script protect_save.cmake übernimmt die Logik:
#   - Alle Assets werden kopiert (copy_directory)
#   - welt.json und player.json werden NUR kopiert wenn sie im Ziel noch
#     nicht existieren → gespeicherte Spielstände bleiben erhalten
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -DSRC_ASSETS="${CMAKE_SOURCE_DIR}/assets"
            -DDST_ASSETS="$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
            -P "${CMAKE_SOURCE_DIR}/cmake/protect_save.cmake"
        COMMENT "Kopiere Assets (Spielstaende werden geschuetzt)..."
    )
endif()

message(STATUS "=== 2DGameEngine Konfiguration ===")
message(STATUS "Build-Dir: ${CMAKE_BINARY_DIR}")
message(STATUS "Bin-Dir:   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")