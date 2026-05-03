// ground.h
#pragma once
#include "raylib.h"
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>

using json = nlohmann::json;

struct BodenDatenbank {
    json alle_boden_arten;
    std::unordered_map<std::string, Texture2D> texturen;
    
    void laden(const std::string& datei);
    bool exists(const std::string& typ) const;
    const json& get(const std::string& typ) const;
    bool is_begehbar(const std::string& typ) const;
    void lade_texturen();
    void entlade_texturen();
};