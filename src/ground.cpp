// ground.cpp
#include "ground.h"
#include <iostream>
#include <fstream>

// Asset-Pfad-Helper (definiert in main.cpp)
extern std::string assetPfad(const std::string& relativ);

// Gibt einen relativen assets/-Pfad zurueck, egal ob die JSON einen
// absoluten oder relativen Pfad gespeichert hat.
// Beispiel:  "C:/Users/foo/proj/assets/textures/gras.png"  →  "textures/gras.png"
//            "assets/textures/gras.png"                    →  "textures/gras.png"
//            "textures/gras.png"                           →  "textures/gras.png"
static std::string normalisierePfad(const std::string& roherPfad) {
    // Trennzeichen fuer assets/ suchen (vorwaerts und rueckwaerts Slash)
    const std::string MARKER = "assets/";
    // Plattformunabhaengig: beide Slash-Varianten pruefen
    for (const std::string& sep : { "assets/", "assets\\" }) {
        size_t pos = roherPfad.rfind(sep);
        if (pos != std::string::npos) {
            std::string rest = roherPfad.substr(pos + sep.size());
            // Backslashes → Forwardslashes
            for (char& c : rest) if (c == '\\') c = '/';
            return rest;
        }
    }
    // Kein assets/-Marker gefunden → Pfad unveraendert zurueckgeben
    // (backslashes normalisieren)
    std::string result = roherPfad;
    for (char& c : result) if (c == '\\') c = '/';
    return result;
}

void BodenDatenbank::laden(const std::string& datei) {
    std::ifstream f(datei.c_str());
    if (f.is_open()) {
        try {
            f >> alle_boden_arten;
            std::cout << "Geladen: " << datei << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Boden] JSON Fehler in " << datei << ": " << e.what() << std::endl;
        }
        f.close();
    } else {
        std::cerr << "Fehler: Kann Datei nicht oeffnen: " << datei << std::endl;
    }
}

bool BodenDatenbank::exists(const std::string& typ) const {
    return alle_boden_arten.contains(typ);
}

const json& BodenDatenbank::get(const std::string& typ) const {
    // .at() wirft bei fehlendem Key – statisches Fallback-Objekt zurueckgeben
    static const json leer = json::object();
    auto it = alle_boden_arten.find(typ);
    if (it != alle_boden_arten.end()) return it.value();
    return leer;
}

bool BodenDatenbank::is_begehbar(const std::string& typ) const {
    if (!exists(typ)) return true;
    return get(typ).value("accessible", true);
}

void BodenDatenbank::lade_texturen() {
    for (auto& [typ, data] : alle_boden_arten.items()) {
        std::string roherPfad = data.value("textur", "");
        if (roherPfad.empty()) continue;

        // Pfad normalisieren: absoluten Pfad vom alten Geraet → relativen Pfad
        std::string relPfad = normalisierePfad(roherPfad);
        std::string vollPfad = assetPfad(relPfad);

        std::cout << "[Boden] Lade Textur: " << vollPfad << std::endl;
        Texture2D tex = LoadTexture(vollPfad.c_str());
        texturen[typ] = tex;
        if (tex.id == 0)
            std::cerr << "[Boden] FEHLER: Textur nicht geladen: " << vollPfad << std::endl;
        else
            std::cout << "[Boden] Textur geladen: " << vollPfad << std::endl;
    }
}

void BodenDatenbank::entlade_texturen() {
    for (auto& [typ, tex] : texturen) {
        UnloadTexture(tex);
        std::cout << "Textur entladen: " << typ << std::endl;
    }
    texturen.clear();
}