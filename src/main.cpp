#include "raylib.h"
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "ground.h"
#include "map.h"
#include "Cam.h"
#include "player.h"
#include "inventar.h"
#include "Items.h"

using json = nlohmann::json;

#ifndef ASSETS_PATH
    #define ASSETS_PATH "assets"
#endif

std::string assetPfad(const std::string& relativ) {
    return std::string(ASSETS_PATH) + "/" + relativ;
}

Map welt;

int main()
{
    int screenWidth  = 1280;
    int screenHeight = 720;
    int TILE_SIZE    = 32;

    // ── Working Directory auf den Projektordner setzen ────────────────────────
    // Damit funktionieren alle relativen Pfade in JSON-Dateien (Texturen etc.)
    // unabhängig davon, aus welchem Verzeichnis das Binary gestartet wird.
    {
        namespace fs = std::filesystem;
        fs::path assetsDir = fs::path(ASSETS_PATH).parent_path();
        if (!assetsDir.empty() && fs::exists(assetsDir)) {
            ChangeDirectory(assetsDir.string().c_str());
        }
    }

    InitWindow(screenWidth, screenHeight, "2D Game Engine");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    // config.json laden
    {
        std::ifstream configFile(assetPfad("config.json"));
        if (configFile.is_open()) {
            json config = json::parse(configFile);
            SetWindowTitle(config.value("title", std::string("2D Game Engine")).c_str());
            SetTargetFPS(config.value("fps", 60));
        }
    }

    player neuerSpieler;
    initCamera();

    g_itemManager.scanneUndLadeItems();

    // ── Map laden ─────────────────────────────────────────────────────────────
    {
        namespace fs = std::filesystem;
        const std::string save = assetPfad("json/Map/welt.json");
        const std::string def  = assetPfad("json/Map/welt_default.json");
        if (!fs::exists(save) && fs::exists(def))
            fs::copy_file(def, save);
    }
    welt.laden(assetPfad("json/Map/welt.json"));

    loadPlayer(neuerSpieler);

    BodenDatenbank boden;
    boden.laden(assetPfad("json/Map/ground.json"));
    boden.lade_texturen();
    welt.init(boden);

    float speicherTimer = 0.0f;
    const float SPEICHER_INTERVALL = 30.0f;

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();
        moovePlayer(neuerSpieler);
        kameramoovment();

        speicherTimer += delta;
        if (speicherTimer >= SPEICHER_INTERVALL) {
            welt.speichern(assetPfad("json/Map/welt.json"));
            savePlayer(neuerSpieler);
            speicherTimer = 0.0f;
        }

        BeginDrawing();
            ClearBackground(WHITE);
            BeginMode2D(camera);
                draw_ground(welt, boden, TILE_SIZE);
                DrawPlayer(neuerSpieler);
            EndMode2D();
            DrawInventar(neuerSpieler);
        EndDrawing();
    }

    welt.speichern(assetPfad("json/Map/welt.json"));
    savePlayer(neuerSpieler);
    boden.entlade_texturen();
    CloseWindow();
    return 0;
}