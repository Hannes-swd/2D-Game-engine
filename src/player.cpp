#include "player.h"
#include "Cam.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void loadPlayer(player& p) {
    std::ifstream f("assets/json/player.json");
    if (f.is_open()) {
        std::cout << "[loadPlayer] player.json gefunden" << std::endl;
        json data;
        f >> data;
        p.Set_position(data.value("posX", 0), data.value("posY", 0));
        p.Change_Name(data.value("name", std::string("Spieler")));
    } else {
        std::cout << "[loadPlayer] player.json NICHT gefunden -> Fallback" << std::endl;
        p.Set_position(0, 0);
        p.Change_Name("Spieler");
    }

    std::cout << "[loadPlayer] Inventar nach laden: "
              << p.getInventory().size() << " slots" << std::endl;
}

void savePlayer(const player& p) {
    json data;
    Vector2 pos  = p.Get_position();
    data["posX"] = pos.x;
    data["posY"] = pos.y;
    data["name"] = p.Get_Name();

    json inventarArray = json::array();
    for (const auto& slot : p.getInventory()) {
        inventarArray.push_back({
            {"id",     slot.itemId},
            {"anzahl", slot.anzahl}
        });
    }
    data["inventory"] = inventarArray;

    std::ofstream f("assets/json/player.json");
    if (f.is_open())
        f << data.dump(4);
}

void moovePlayer(player& p) {
    float delta = GetFrameTime();

    if (IsKeyDown(KEY_W)) p.Move(Up,    delta);
    if (IsKeyDown(KEY_S)) p.Move(Down,  delta);
    if (IsKeyDown(KEY_A)) p.Move(Left,  delta);
    if (IsKeyDown(KEY_D)) p.Move(Right, delta);

    Vector2 pos     = p.Get_position();
    camera.target.x = pos.x;
    camera.target.y = pos.y;
}

void DrawPlayer(player& p) {
    Vector2 playerpos = p.Get_position();
    float posx = playerpos.x;
    float posy = playerpos.y;


    DrawCircle(posx, posy, 10, ORANGE);
}