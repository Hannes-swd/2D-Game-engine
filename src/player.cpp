#include "player.h"
#include "Cam.h"
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void loadPlayer(player& p) {
    std::ifstream f("assets/json/player.json");
    if (f.is_open()) {
        json data;
        f >> data;
        p.Set_position(data.value("posX", 0), data.value("posY", 0));
        p.Change_Name(data.value("name", std::string("Spieler")));
    } else {
        p.Set_position(0, 0);
        p.Change_Name("Spieler");
    }
}

void savePlayer(const player& p) {
    json data;
    Vector2 pos = p.Get_position();
    data["posX"] = pos.x;
    data["posY"] = pos.y;
    data["name"] = p.Get_Name();

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

    // Kamera folgt dem Spieler
    Vector2 pos = p.Get_position();
    camera.target.x = pos.x;
    camera.target.y = pos.y;
}