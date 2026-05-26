#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

using json = nlohmann::json;

#ifndef ASSETS_PATH
#define ASSETS_PATH "assets"
#endif

static json readJson(const std::string& path) {
    std::ifstream f(path);
    if (!f) { std::cerr << "Fehler: Kann nicht lesen: " << path << "\n"; return {}; }
    return json::parse(f);
}

static bool writeJson(const std::string& path, const json& j) {
    std::ofstream f(path);
    if (!f) { std::cerr << "Fehler: Kann nicht schreiben: " << path << "\n"; return false; }
    f << j.dump(4);
    return true;
}

int main() {
    const std::string assets = ASSETS_PATH;

    std::cout << "=== Welt wird zurueckgesetzt ===\n\n";

    // world.json: Tiles leeren (nur 0,0 behalten), Gebaeude behalten
    {
        std::string path = assets + "/json/Map/world.json";
        json w = readJson(path);
        w["tiles"] = json::object();
        w["tiles"]["0,0"] = "gras";
        w["buildings"] = json::object();
        if (writeJson(path, w))
            std::cout << "[OK] world.json  -> Tiles geleert, Gebaeude entfernt\n";
    }

    // player.json: Position auf 0,0 setzen
    {
        std::string path = assets + "/json/player/player.json";
        json p = readJson(path);
        p["posX"] = 0;
        p["posY"] = 0;
        if (writeJson(path, p))
            std::cout << "[OK] player.json -> Position auf (0, 0) gesetzt\n";
    }

    // dimensions/*.json:
    //   - Basisdefinitionen (Schluesse aus dimensions.json): Tiles leeren
    //   - Instanzdateien (z.B. house_interior_House_1_2.json): loeschen
    {
        namespace fs = std::filesystem;
        std::string dimDir = assets + "/json/Map/dimensions";

        // Basis-IDs aus dimensions.json einlesen
        json dimDefs = readJson(assets + "/json/Map/dimensions.json");
        std::set<std::string> baseIds;
        for (auto& [key, _] : dimDefs.items())
            baseIds.insert(key);

        for (const auto& entry : fs::directory_iterator(dimDir)) {
            if (entry.path().extension() != ".json") continue;
            std::string stem = entry.path().stem().string();
            if (baseIds.count(stem)) {
                // Basisdatei: Tiles leeren
                json d = readJson(entry.path().string());
                d["tiles"] = json::object();
                if (writeJson(entry.path().string(), d))
                    std::cout << "[OK] " << entry.path().filename().string()
                              << " -> Tiles geleert\n";
            } else {
                // Instanzdatei: loeschen
                fs::remove(entry.path());
                std::cout << "[DEL] " << entry.path().filename().string()
                          << " -> geloescht\n";
            }
        }
    }

    std::cout << "\n=== Fertig! ===\n";
    std::cout << "  - Welt-Tiles:      nur Boden bei (0, 0)\n";
    std::cout << "  - Gebaeude:        alle entfernt\n";
    std::cout << "  - Innenraeume:     alle leer (Standard)\n";
    std::cout << "  - Spielerposition: (0, 0)\n";
    std::cout << "  - Boden/Dimensionsdefinitionen: unveraendert\n";
    std::cout << "\nDruecke Enter zum Beenden...";
    std::cin.get();
    return 0;
}
