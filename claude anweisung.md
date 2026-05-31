# NPC-System – Implementierungsanweisung

## Übersicht

Das NPC-System folgt exakt dem gleichen Muster wie das bestehende ITEM/BUILDING/DIMENSION-System.
Modder erstellen NPC-Typen über ein Makro-System. Die Engine übernimmt Rendering, Pathfinding, Kollision und Speichern/Laden automatisch.

---

## Neue Dateien

| Datei | Zweck |
|-------|-------|
| `src/NPC.h` | NPC-Struct, NPCManager, PlacedNPC |
| `src/NPC.cpp` | NPCManager-Implementierung, Update-Loop, Pathfinding, Speichern/Laden |
| `src/npc_api.h` | Modder-API: NPC_BEGIN/NPC_END, Hilfsfunktionen |
| `src/npc_pathfinding.h` | A*-Algorithmus (intern, Modder nutzt ihn nicht direkt) |
| `npc_config/npc_settings.cpp` | Globale Engine-Einstellungen (Pathfinding-Performance, Wander-Timing) |
| `npc_config/npc_ext_vars.h` | Globale Zusatzvariablen für ALLE NPCs – standardmäßig leer |
| `npcs/` | Verzeichnis für Modder-NPC-Definitionen (z. B. `npcs/Farmer.cpp`) |

---

## Kern-Datenstrukturen (`src/NPC.h`)

```cpp
struct PlacedNPC {
    std::string npcId;       // Template-ID (z. B. "farmer")
    std::string instanceId;  // Eindeutig: "farmer_12_8"
    float posX, posY;
    direction lastDir = direction::Down;

    // Erscheinung – alle Werte kommen aus JSON, können pro Instanz überschrieben werden
    std::string bodyId, shirtId, pantsId, hairId;

    // Inventar (optional, Modder befüllt es selbst)
    std::vector<InventorySlot> inventory;

    // Bewegung – Defaults aus JSON
    bool movementEnabled = true;
    float speed = 50.0f;
    float wanderRadius = 96.0f;
    float originX, originY;  // Startposition als Wandermittelpunkt

    // Interaktion – Defaults aus JSON
    float proximityRadius = 80.0f;

    // Kollision – Defaults aus JSON
    bool collideWithTiles  = true;
    bool collideWithNpcs   = false;
    bool collideWithPlayer = false;

    // Zustand
    std::string state = "idle";  // "idle", "wander", "follow_path", "follow_player", "flee_player", "go_to", "custom"
    std::string customData;      // Frei nutzbarer String

    // Pathfinding (intern)
    std::vector<Vector2> path;
    int   pathIndex   = 0;
    float wanderTimer = 0.0f;

    // Instanz-Variablen des Modders (NPC_VAR / NPC_REF)
    std::unordered_map<std::string, std::any> customVars;

    // Globale Zusatzvariablen (aus npc_ext_vars.h)
    #include "../npc_config/npc_ext_vars.h"

    // Callbacks
    std::function<void(PlacedNPC&)> onUpdate;       // Jeden Frame
    std::function<void(PlacedNPC&)> onProximity;    // Jeden Frame solange Spieler in Radius
    std::function<void(PlacedNPC&)> onLeaveProximity; // Einmal wenn Spieler Radius verlässt
    std::function<void(PlacedNPC&)> onSpawn;        // Einmal beim Spawnen
};

struct NPCTemplate {
    std::string id, name;

    // Defaults – alle aus JSON geladen
    std::string bodyId, shirtId, pantsId, hairId;
    float speed            = 50.0f;
    float wanderRadius     = 96.0f;
    float proximityRadius  = 80.0f;
    bool  movementEnabled  = true;
    bool  collideWithTiles  = true;
    bool  collideWithNpcs   = false;
    bool  collideWithPlayer = false;

    std::function<void(PlacedNPC&)> onUpdate;
    std::function<void(PlacedNPC&)> onProximity;
    std::function<void(PlacedNPC&)> onLeaveProximity;
    std::function<void(PlacedNPC&)> onSpawn;
};

class NPCManager {
public:
    void       registerTemplate(const std::string& id, NPCTemplate tmpl);
    PlacedNPC& spawnNPC(const std::string& npcId, float x, float y);
    void       removeNPC(const std::string& instanceId);
    PlacedNPC* getNPC(const std::string& instanceId);
    std::vector<PlacedNPC>& getAll();

    PlacedNPC* getLastProximityNPC();  // Letzter NPC in dessen onProximity der Spieler war

    void update(float deltaTime);
    void draw();

    void save(const std::string& path);
    void load(const std::string& path);

private:
    std::unordered_map<std::string, NPCTemplate> templates;
    std::vector<PlacedNPC> placed;
    int nextId = 0;

    void updateMovement(PlacedNPC& npc, float deltaTime);
    void updateProximity(PlacedNPC& npc);
    std::vector<Vector2> findPath(Vector2 from, Vector2 to);
};

NPCManager& getNPCManager();
#define g_npcManager getNPCManager()
```

---

## Modder-Makro-System (`src/npc_api.h`)

### Callbacks

| Callback | Wann aufgerufen |
|----------|----------------|
| `onSpawn()` | Einmal direkt nach dem Spawnen |
| `onUpdate()` | Jeden Frame |
| `onProximity()` | Jeden Frame solange Spieler innerhalb `proximityRadius` |
| `onLeaveProximity()` | Einmal wenn Spieler `proximityRadius` verlässt |

**`onInteract` gibt es nicht.** Der Modder reagiert auf Tasten/Klicks selbst in `onProximity` oder `onUpdate`.

### Basis-Struktur

```cpp
NPC_BEGIN("farmer", Farmer)

    void onSpawn() { }

    void onUpdate() {
        // Jeden Frame – NPC-Daten: npc.posX, npc.posY, npc.state usw.
    }

    void onProximity() {
        // Spieler ist in der Nähe – läuft jeden Frame
        // Hier Tasten abfragen und UI öffnen:
        setPendingTooltip("[E] Sprechen");

        if (IsKeyPressed(KEY_E)) {
            openUI("farmer_dialog");
        }
        if (leftClickPressed()) {
            openUI("farmer_dialog");
        }
    }

    void onLeaveProximity() {
        // Spieler hat Radius verlassen – einmalig
    }

NPC_END("farmer")
```

### Instanz-Variablen: NPC_VAR

Jede gespawnte Instanz hat ihre eigene Kopie. Typ, Name, Default-Wert.

```cpp
NPC_VAR(int,   level,    1)
NPC_VAR(bool,  hostile,  false)
NPC_VAR(float, stamina,  100.0f)

// Lesen:  NPC_GET(level)
// Setzen: NPC_SET(level, 5)
```

Gespeichert/geladen werden: `int`, `float`, `bool`, `std::string`.

### NPC-Referenz: NPC_REF

Saubere Variante um einen anderen NPC per instanceId zu referenzieren.

```cpp
NPC_REF(partner)   // Intern: std::string partner_ref = ""

// Setzen:
npcSetRef(npc, "partner", otherNpc.instanceId);

// Lesen (gibt PlacedNPC* oder nullptr):
PlacedNPC* p = npcGetRef(npc, "partner");
if (p) { /* ... */ }
```

Wird wie ein normaler customVar gespeichert (als String mit instanceId).

### Makro-Implementierung (intern)

```cpp
#define NPC_BEGIN(id_str, class_name) \
namespace npc_##class_name { \
static struct _Reg { \
    _Reg() { \
        NPCTemplate t; t.id = id_str; \
        struct _Impl {

#define NPC_VAR(type, name, def) \
        // → npc.customVars[#name] = def  beim Spawnen

#define NPC_GET(name)      npc_get_var<decltype(name##_def)>(npc, #name)
#define NPC_SET(name, val) npc_set_var(npc, #name, val)

#define NPC_REF(name) \
        // → npc.customVars[#name "_ref"] = ""  beim Spawnen

#define NPC_END(id_str) \
        }; \
        getNPCManager().registerTemplate(id_str, t); \
    } \
} _r; }
```

---

## Globale Zusatzvariablen (`npc_config/npc_ext_vars.h`)

**Standardmäßig leer.** Modder fügen hier Variablen hinzu, die JEDER NPC haben soll.

```cpp
// npc_config/npc_ext_vars.h  – vom Modder befüllt, z. B.:
// int health = 100;
// int maxHealth = 100;
```

Werden automatisch gespeichert/geladen (bekannte Typen: int, float, bool, string).

Beispiel für ein Kampfspiel:
```cpp
int   health    = 100;
int   maxHealth = 100;
float attackDmg = 10.0f;
bool  isHostile = false;
```

Beispiel für ein reines Chill-Game (Datei bleibt leer):
```cpp
// leer – kein Kampf, keine Stats nötig
```

---

## Bewegungssystem

### Modi (per `npc.state`)

| Modus | Verhalten |
|-------|-----------|
| `"idle"` | NPC steht still |
| `"wander"` | Zufällig im `wanderRadius` herum |
| `"follow_path"` | Folgt vorgegebener Waypoint-Liste |
| `"follow_player"` | Läuft dem Spieler hinterher |
| `"flee_player"` | Flieht vor dem Spieler |
| `"go_to"` | Läuft zu Ziel, wird dann idle |
| `"custom"` | Kein automatisches Movement – Modder steuert alles in `onUpdate` |

`movementEnabled = false` im JSON deaktiviert Bewegung komplett unabhängig vom Modus.

### Pathfinding

- A* auf dem Tile-Grid (tile-walkability aus `Ground`-Daten)
- Aktiv wenn `movementEnabled = true` und Modus nicht `"idle"` / `"custom"`
- Wenn Ziel ohne Hindernis direkt erreichbar → direkt bewegen, kein A* nötig

**API für Modder (in allen Callbacks nutzbar):**
```cpp
npcGoTo(npc, x, y);                         // Laufe zu Weltposition (A*)
npcSetWander(npc, true);                    // Wandermodus
npcSetMovement(npc, false);                 // Bewegung stoppen
npcSetWaypoints(npc, {{x1,y1},{x2,y2}});   // Feste Wegpunkte
npcFollowPlayer(npc);                        // Folge dem Spieler
npcSetSpeed(npc, 80.0f);                    // Geschwindigkeit
```

---

## JSON-Format (`assets/json/npcs/`)

### Template-Definition

Alle NPC-Eigenschaften inklusive Kollision und Bewegung kommen aus der JSON-Datei.

```json
// assets/json/npcs/farmer.json
{
    "farmer": {
        "name": "Bauer",
        "bodyId": "human_base",
        "shirtId": "shirt_brown",
        "pantsId": "pants_green",
        "hairId": "hair_blonde",
        "speed": 45.0,
        "movementEnabled": true,
        "wanderRadius": 128.0,
        "proximityRadius": 80.0,
        "collision": {
            "withTiles":  true,
            "withNpcs":   false,
            "withPlayer": false
        }
    }
}
```

NPC ohne Kleidung (z. B. Monster mit eigener Textur):
```json
{
    "slime": {
        "name": "Schleim",
        "textur": "assets/Images/npcs/slime.png",
        "speed": 30.0,
        "movementEnabled": true,
        "wanderRadius": 64.0,
        "proximityRadius": 60.0,
        "collision": {
            "withTiles":  true,
            "withNpcs":   true,
            "withPlayer": false
        }
    }
}
```

NPC der komplett still steht:
```json
{
    "statue": {
        "name": "Statue",
        "textur": "assets/Images/npcs/statue.png",
        "movementEnabled": false,
        "proximityRadius": 48.0,
        "collision": {
            "withTiles":  false,
            "withNpcs":   false,
            "withPlayer": false
        }
    }
}
```

### Gespeicherte Instanzen (Weltzustand)

```json
// assets/json/Map/npcs.json – automatisch vom NPCManager
{
    "farmer_12_8": {
        "npcId":    "farmer",
        "posX":     384.0,
        "posY":     256.0,
        "bodyId":   "human_base",
        "shirtId":  "shirt_red",
        "pantsId":  "pants_green",
        "hairId":   "hair_blonde",
        "inventory": [],
        "customVars": {
            "level":  3,
            "hostile": false
        },
        "extVars": {}
    }
}
```

Instanz-Overrides (z. B. andere Kleidung) überschreiben Template-Defaults. Nicht gesetzte Felder bleiben Template-Default.

---

## Globale Engine-Einstellungen (`npc_config/npc_settings.cpp`)

Nur Engine-interne Performance-Parameter. Keine Spiel-Logik, keine Kollision.

```cpp
#include "../src/npc_api.h"

NPC_SETTINGS_BEGIN
    PATHFINDING_ENABLED(true)         // Pathfinding global an/aus
    WANDER_INTERVAL(3.0f, 6.0f)       // Sekunden zwischen Wanderzielen (Min, Max)
    MAX_PATHFIND_PER_FRAME(3)         // Max. A*-Berechnungen pro Frame
NPC_SETTINGS_END
```

---

## Dialog-System

Dialoge sind einfach UI-Fenster (bestehendes UI-System). Kein eigenes Dialog-System.

```cpp
// In ui/FarmerDialog.cpp:
UI_WINDOW_BEGIN("farmer_dialog")
    UI_TITLE("Bauer Hans")
    UI_TEXT("Was kann ich für euch tun?")
    UI_SEPARATOR()
    UI_BUTTON("Tschüss", []() { closeCurrentUI(); })
UI_WINDOW_END("farmer_dialog")
```

Mit NPC-Variablen im Dialog:
```cpp
UI_WINDOW_BEGIN("farmer_dialog")
    UI_LIVE_TEXT("greeting_text", "...")

    UI_ON_OPEN([]() {
        PlacedNPC* npc = g_npcManager.getLastProximityNPC();
        int lv = npc_get_var<int>(*npc, "level");
        updateUIText("greeting_text", "Ich bin Level " + std::to_string(lv));
    })

    UI_BUTTON("Schließen", []() { closeCurrentUI(); })
UI_WINDOW_END("farmer_dialog")
```

`getLastProximityNPC()` gibt den NPC zurück, dessen `onProximity` zuletzt `openUI` aufgerufen hat.

---

## Inventar für NPCs

NPCs können optional das Inventar-System nutzen. Kein Handel eingebaut – der Modder baut das selbst mit dem UI-System.

```cpp
npcAddItem(npc, "appleItem", 5);
npcRemoveItem(npc, "appleItem", 1);
npcHasItem(npc, "appleItem");
npcGetInventory(npc);  // std::vector<InventorySlot>&
```

Inventar des NPCs im Trade-UI zugänglich über `g_npcManager.getLastProximityNPC()->inventory`.

---

## Beziehungssystem (komplett Modder-seitig)

Das System stellt KEINE vorgefertigten Beziehungen bereit. Alles ist Modder-Code.

### Beziehungen zwischen NPCs

```cpp
// In npc_config/npc_ext_vars.h (Modder fügt das hinzu):
std::unordered_map<std::string, int> relationships;
```

```cpp
// Modder schreibt Hilfsfunktionen (z. B. in npc_config/relationships.h):
inline int  getRelationship(PlacedNPC& a, const std::string& targetId) {
    auto it = a.relationships.find(targetId);
    return it != a.relationships.end() ? it->second : 0;
}
inline void setRelationship(PlacedNPC& a, const std::string& targetId, int val) {
    a.relationships[targetId] = std::clamp(val, -100, 100);
}
inline void changeRelationship(PlacedNPC& a, const std::string& targetId, int delta) {
    setRelationship(a, targetId, getRelationship(a, targetId) + delta);
}
```

Verwendung:
```cpp
PlacedNPC& guard1 = spawnNPC("guard", 200, 300);
PlacedNPC& guard2 = spawnNPC("guard", 220, 300);
setRelationship(guard1, guard2.instanceId, 80);
setRelationship(guard2, guard1.instanceId, 80);
```

### NPC-Referenz (z. B. Partner)

```cpp
NPC_BEGIN("villager", Villager)
    NPC_REF(partner)  // Sauberer als NPC_VAR(std::string, partnerInstanceId, "")

    void onUpdate() {
        PlacedNPC* p = npcGetRef(npc, "partner");
        if (p) {
            npcGoTo(npc, p->posX, p->posY);  // Folge dem Partner
        }
    }
NPC_END("villager")
```

### Spieler-NPC-Beziehung

```cpp
NPC_BEGIN("villager", Villager)
    NPC_VAR(int, affection, 0)

    void onProximity() {
        setPendingTooltip("[E] Sprechen");
        if (IsKeyPressed(KEY_E)) {
            openUI("villager_chat");
        }
    }
NPC_END("villager")
```

```cpp
// Irgendwo im Modder-Code (z. B. Item-onClick wenn Geschenk übergeben):
PlacedNPC* npc = g_npcManager.getNPC("villager_5_3");
if (npc) NPC_SET(*npc, affection, NPC_GET(*npc, affection) + 10);
```

---

## Rendering-Reihenfolge

NPCs und Spieler werden zusammen nach Y-Position sortiert und gezeichnet.

Rendering-Layers pro Entity:
1. Body-Textur (oder Clothing-Body)
2. Pants-Layer
3. Shirt-Layer
4. Hair-Layer
5. Interaktions-Indikator (z. B. Icon über dem NPC wenn Spieler in Proximity)

---

## Integration in `main.cpp`

```cpp
// Ladereihenfolge (nach Dimensions):
g_npcManager.load("assets/json/Map/npcs.json");

// Update-Schleife:
g_npcManager.update(GetFrameTime());

// Draw-Schleife (innerhalb BeginMode2D):
g_npcManager.draw();

// Speichern:
g_npcManager.save("assets/json/Map/npcs.json");
```

---

## Spawnen von NPCs

```cpp
#include "../src/npc_api.h"

// Minimal:
spawnNPC("farmer", 384.0f, 256.0f);

// Mit Anpassungen (überschreibt JSON-Defaults):
PlacedNPC& npc = spawnNPC("farmer", 384.0f, 256.0f);
npc.shirtId = "shirt_red";
npcSetWander(npc, false);
```

---

## Zusammenfassung: Was der Modder erhält

| Feature | Wie |
|---------|-----|
| NPC erstellen | `NPC_BEGIN` / `NPC_END` in `npcs/*.cpp` |
| Eigene Instanzvariablen | `NPC_VAR(typ, name, default)` |
| NPC-Referenz | `NPC_REF(name)` + `npcGetRef` / `npcSetRef` |
| Globale Variablen für alle NPCs | `npc_config/npc_ext_vars.h` (leer by default) |
| Interaktion | Tasten/Klicks in `onProximity` selbst abfragen |
| Dialog | Vorhandenes UI-System (`UI_WINDOW_BEGIN`) |
| Bewegung/Pathfinding | `npcGoTo()`, `npcSetWander()` usw., Toggle per JSON |
| Kleidung | `bodyId`, `shirtId`, `pantsId`, `hairId` im JSON |
| Kollision | `collision.withTiles/Npcs/Player` im JSON |
| Inventar | `npcAddItem()` usw., zugänglich in UI |
| Beziehungen | Modder baut selbst via `npc_ext_vars.h` + Hilfsfunktionen |
| Spawnen | `spawnNPC("id", x, y)` |
| Entfernen | `removeNPC(instanceId)` |
| NPC finden | `g_npcManager.getNPC(instanceId)` |

Genres: City-Builder, Open-World-RPG, Farmspiel, Dungeon-Crawler, Chill-Game – alles möglich ohne Änderung der Engine.
