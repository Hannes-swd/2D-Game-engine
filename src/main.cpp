#include "raylib.h"
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "ground.h"
#include "map.h"
#include "Cam.h"
#include "player.h"
#include "Items.h"

using json = nlohmann::json;

int main()
{
    int screenWidth  = 1280;
    int screenHeight = 720;
    int TILE_SIZE    = 32;

    InitWindow(screenWidth, screenHeight, "2D Game Engine");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    player neuerSpieler;
    initCamera();

    // config.json laden
    std::ifstream configFile("assets/config.json");
    if (configFile.is_open()) {
        json config = json::parse(configFile);
        SetWindowTitle(config.value("title", std::string("2D Game Engine")).c_str());
        SetTargetFPS(config.value("fps", 60));
    }

    // Items laden (scannt assets/json/items/)
    g_itemManager.scanneUndLadeItems();

    // Map laden
    Map welt;
    welt.laden("assets/json/Map/welt.json");

    // Spieler laden
    loadPlayer(neuerSpieler);

    // Bodendatenbank laden
    BodenDatenbank boden;
    boden.laden("assets/json/Map/ground.json");
    boden.lade_texturen();
    welt.init(boden);

    while (!WindowShouldClose())
    {
        moovePlayer(neuerSpieler);
        kameramoovment();

        BeginDrawing();
            ClearBackground(WHITE);

            // Welt in Kamera-Koordinaten
            BeginMode2D(camera);
                draw_ground(welt, boden, TILE_SIZE);
                DrawPlayer(neuerSpieler);
            EndMode2D();

            // UI in Screen-Koordinaten (NACH EndMode2D!)
            DrawInventar(neuerSpieler);

        EndDrawing();
    }

    // Cleanup
    boden.entlade_texturen();
    welt.speichern("assets/json/Map/welt.json");
    savePlayer(neuerSpieler);
    CloseWindow();
    return 0;
}