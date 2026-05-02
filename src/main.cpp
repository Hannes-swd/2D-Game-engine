#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json;

int main()
{
    const int screenWidth  = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "2D Game Engine - Test");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // --- JSON Test: config.json laden (falls vorhanden) ---
    std::string windowTitle = "2D Game Engine";
    int         targetFps   = 60;

    std::ifstream configFile("assets/config.json");
    if (configFile.is_open())
    {
        json config = json::parse(configFile);
        windowTitle = config.value("title", windowTitle);
        targetFps   = config.value("fps",   targetFps);
        SetTargetFPS(targetFps);
        SetWindowTitle(windowTitle.c_str());
    }

    bool  showDemoWindow  = true;
    bool  showEnginePanel = true;
    bool  showJsonPanel   = true;
    float clearColor[3]   = { 0.1f, 0.1f, 0.15f };
    int   frameCount      = 0;

    while (!WindowShouldClose())
    {
        frameCount++;

        BeginDrawing();

            Color bg = {
                (unsigned char)(clearColor[0] * 255),
                (unsigned char)(clearColor[1] * 255),
                (unsigned char)(clearColor[2] * 255),
                255
            };
            ClearBackground(bg);

            DrawText("Raylib + ImGui + JSON funktioniert!", 40, 40, 28, RAYWHITE);
            DrawCircle(screenWidth / 2, screenHeight / 2, 60.0f, SKYBLUE);
            DrawText(TextFormat("Frame: %d", frameCount), 40, 80, 20, LIGHTGRAY);
            DrawFPS(10, 10);

            rlImGuiBegin();

                // --- Fullscreen Dockspace ---
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowBgAlpha(0.0f);

                ImGuiWindowFlags dockFlags =
                    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize   | ImGuiWindowFlags_NoMove     |
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                    ImGuiWindowFlags_NoBackground;

                ImGui::Begin("##Dockspace", nullptr, dockFlags);
                ImGui::DockSpace(ImGui::GetID("MainDockspace"), ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
                ImGui::End();

                // --- Engine Panel ---
                if (showEnginePanel)
                {
                    ImGui::Begin("Engine Panel", &showEnginePanel);
                    ImGui::Text("2D Game Engine");
                    ImGui::Separator();

                    ImGui::Text("Hintergrundfarbe:");
                    ImGui::ColorEdit3("##bg", clearColor);

                    ImGui::Separator();
                    ImGui::Text("FPS: %.1f", io.Framerate);
                    ImGui::Text("Frame: %d", frameCount);

                    if (ImGui::Button("ImGui Demo"))
                        showDemoWindow = !showDemoWindow;
                    ImGui::SameLine();
                    if (ImGui::Button("JSON Panel"))
                        showJsonPanel = !showJsonPanel;

                    ImGui::End();
                }

                // --- JSON Panel ---
                if (showJsonPanel)
                {
                    ImGui::Begin("JSON Test", &showJsonPanel);
                    ImGui::Text("nlohmann/json ist eingebunden!");
                    ImGui::Separator();

                    // JSON live erstellen und anzeigen
                    json live;
                    live["frame"]  = frameCount;
                    live["fps"]    = (int)io.Framerate;
                    live["title"]  = windowTitle;

                    ImGui::Text("Live JSON:");
                    ImGui::TextWrapped("%s", live.dump(2).c_str());

                    ImGui::Separator();
                    if (ImGui::Button("Config speichern"))
                    {
                        json save;
                        save["title"] = windowTitle;
                        save["fps"]   = targetFps;
                        save["bg"]    = { clearColor[0], clearColor[1], clearColor[2] };
                        std::ofstream out("assets/config.json");
                        out << save.dump(4);
                    }

                    ImGui::End();
                }

                if (showDemoWindow)
                    ImGui::ShowDemoWindow(&showDemoWindow);

            rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}