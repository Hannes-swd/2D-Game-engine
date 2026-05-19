// ground.cpp
#include "ground.h"
#include <iostream>
#include <fstream>

void BodenDatenbank::laden(const std::string& datei) {
    std::ifstream f;
    f.open(datei.c_str());
    
    if (f.is_open()) {
        f >> alle_boden_arten;
        std::cout << "Geladen: " << datei << std::endl;
        f.close();
    } else {
        std::cerr << "Fehler: Kann Datei nicht öffnen: " << datei << std::endl;
    }
}

bool BodenDatenbank::exists(const std::string& typ) const {
    return alle_boden_arten.contains(typ);
}

const json& BodenDatenbank::get(const std::string& typ) const {
    return alle_boden_arten.at(typ);
}

bool BodenDatenbank::is_begehbar(const std::string& typ) const {
    if (!exists(typ)) return true;
    return get(typ).value("accessible", true);
}

void BodenDatenbank::lade_texturen() {
    for (auto& [typ, data] : alle_boden_arten.items()) {
        std::string pfad = data.value("textur", "");
        if (!pfad.empty()) {
            Texture2D tex = LoadTexture(pfad.c_str());
            texturen[typ] = tex;
            if (tex.id == 0)
                std::cerr << "[Boden] FEHLER: Textur nicht geladen: " << pfad << std::endl;
            else
                std::cout << "[Boden] Textur geladen: " << pfad << std::endl;
        }
    }
}

void BodenDatenbank::entlade_texturen() {
    for (auto& [typ, tex] : texturen) {
        UnloadTexture(tex);
        std::cout << "Textur entladen: " << typ << std::endl;
    }
    texturen.clear();
}