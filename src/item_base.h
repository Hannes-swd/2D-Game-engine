#pragma once
#include "Items.h"
#include <functional>
#include <iostream>

// Makro für automatische Item-Registrierung
#define REGISTER_ITEM(ItemClass, ItemId, ItemName, TexturePath) \
    class ItemClass##Registrar { \
    public: \
        ItemClass##Registrar() { \
            g_itemManager.registriereItem(ItemId, ItemName, TexturePath, \
                []() { new ItemClass(); }); \
            std::cout << "Item registriert: " << ItemId << std::endl; \
        } \
    }; \
    static ItemClass##Registrar global_##ItemClass##Registrar;

// Basis-Klasse für Items
class BaseItem {
public:
    virtual ~BaseItem() = default;
    virtual void onKlick() {}
    virtual void onInventar() {}
    virtual void onHand() {}
    virtual void onUpdate() {}
    virtual void zeichnen(int x, int y, Texture2D textur) {
        if (textur.id != 0) {
            DrawTexture(textur, x, y, WHITE);
        }
    }
};

// ItemManager erweitern
class ItemManager {
private:
    struct ItemInfo {
        std::string name;
        std::string texturPfad;
        Texture2D textur;
        std::function<BaseItem*()> factory;
    };
    std::unordered_map<std::string, ItemInfo> itemTemplates;
    std::unordered_map<std::string, std::unique_ptr<BaseItem>> aktiveItems;
    
public:
    void registriereItem(const std::string& id, const std::string& name, 
                        const std::string& texturPfad, std::function<BaseItem*()> factory) {
        ItemInfo info;
        info.name = name;
        info.texturPfad = texturPfad;
        info.textur = LoadTexture(texturPfad.c_str());
        info.factory = factory;
        itemTemplates[id] = info;
    }
    
    BaseItem* erstelleItem(const std::string& id) {
        if (itemTemplates.count(id)) {
            auto item = std::unique_ptr<BaseItem>(itemTemplates[id].factory());
            aktiveItems[id] = std::move(item);
            return aktiveItems[id].get();
        }
        return nullptr;
    }
    
    Texture2D getTextur(const std::string& id) {
        if (itemTemplates.count(id)) {
            return itemTemplates[id].textur;
        }
        return {0};
    }
    
    // ... restliche Methoden
};