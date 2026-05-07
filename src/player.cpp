#include "player.h"
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
        // Neue Datei existiert nicht → Standardwerte
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