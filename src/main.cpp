#include "raylib.h"
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "ground.h"
#include "map.h"

using json = nlohmann::json;

int main()
{
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "2D Game Engine");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    // config.json laden
    std::ifstream configFile("assets/config.json");
    if (configFile.is_open())
    {
        json config = json::parse(configFile);
        std::string title = config.value("title", std::string("2D Game Engine"));
        int fps = config.value("fps", 60);
        SetWindowTitle(title.c_str());
        SetTargetFPS(fps);
    }


    //map
    Map welt;
    welt.laden("assets/json/Map/welt.json");


    // ========== BODEN DATENBANK LADEN ==========
    BodenDatenbank boden;
    boden.laden("assets/json/Map/ground.json");
    boden.lade_texturen();

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(WHITE);


            EndDrawing();
    }

    // Cleanup
    boden.entlade_texturen();
    welt.speichern("assets/json/Map/welt.json");
    CloseWindow();
    return 0;
}