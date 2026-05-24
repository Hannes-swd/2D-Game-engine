#pragma once
// ═══════════════════════════════════════════════════════════════════════════════
//  building_api.h  –  Das einzige Header das ein Building-Modder braucht
//
//  Verwendung in buildings/House.cpp:
//
//      #include "building_api.h"
//      BUILDING_BEGIN("House", House)
//
//          void onHover()  { drawTooltip("[Klick] Betreten"); }
//          void onClick()  { switchDimension("interior_" + getInstanceId()); }
//          void onPlace()  { }
//          void onEnter()  { }
//          void onLeave()  { }
//          void onUpdate() { }
//
//      BUILDING_END("House")
//
//  Alle sechs Callbacks muessen definiert sein (leerer Body ist OK).
//  Das zweite Argument von BUILDING_BEGIN muss ein gueltiger C++-Bezeichner sein.
// ═══════════════════════════════════════════════════════════════════════════════

#include "item api.h"   // setBuildMode, getTileMouse, leftClickPressed, g_player, world, ...
#include "Buildings.h"  // Building, BuildingManager, PlacedBuilding, g_buildingManager
#include <string>
#include <functional>

// ── Gebäude platzieren ───────────────────────────────────────────────────────
inline void placeBuilding(const std::string& buildingId, int x, int y) {
    Building* b = g_buildingManager.getBuilding(buildingId);
    if (!b) {
        return;
    }
    PlacedBuilding pb;
    pb.buildingId  = buildingId;
    pb.x           = x;
    pb.y           = y;
    pb.width       = b->width;
    pb.height      = b->height;
    pb.instanceId  = buildingId + "_" + std::to_string(x) + "_" + std::to_string(y);
    pb.state       = "";
    world.placeBuilding(pb);
    if (b->onPlace) b->onPlace();
}

// ── Interner Auto-Registrar ───────────────────────────────────────────────────
struct _BuildingAutoRegistrar {
    _BuildingAutoRegistrar(const char*           buildingId,
                           std::function<void()> fnHover,
                           std::function<void()> fnClick,
                           std::function<void()> fnPlace,
                           std::function<void()> fnEnter,
                           std::function<void()> fnLeave,
                           std::function<void()> fnUpdate)
    {
        std::string id(buildingId);
        auto& mgr = g_buildingManager;

        mgr.registerFunction(id + "_onHover",  fnHover);
        mgr.registerFunction(id + "_onClick",  fnClick);
        mgr.registerFunction(id + "_onPlace",  fnPlace);
        mgr.registerFunction(id + "_onEnter",  fnEnter);
        mgr.registerFunction(id + "_onLeave",  fnLeave);
        mgr.registerFunction(id + "_onUpdate", fnUpdate);

        // Sofort ans Building binden falls es bereits aus JSON geladen wurde
        Building* b = mgr.getBuilding(id);
        if (b) {
            b->onHover  = fnHover;
            b->onClick  = fnClick;
            b->onPlace  = fnPlace;
            b->onEnter  = fnEnter;
            b->onLeave  = fnLeave;
            b->onUpdate = fnUpdate;
        }
    }
};

// ── BUILDING_BEGIN / BUILDING_END ─────────────────────────────────────────────
//
//  tag = gueltiger C++-Bezeichner, z.B. House, ShopHouse, Dungeon
//  id  = Building-String, z.B. "House", "ShopHouse"
//
//  Alle sechs Funktionen muessen definiert werden (leerer Body ist OK).

#define BUILDING_BEGIN(buildingId, tag)                           \
    namespace _building_impl_##tag {                              \
        static const char* _BUILDING_ID = buildingId;

#define BUILDING_END(buildingId)                                  \
        static _BuildingAutoRegistrar _reg(                       \
            buildingId,                                           \
            onHover, onClick, onPlace,                            \
            onEnter, onLeave, onUpdate                            \
        );                                                        \
    }
