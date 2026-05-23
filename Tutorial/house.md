# Gebäude-System – Designbeschreibung

> Wie das Item-System, aber für Gebäude mit eigenen Callbacks, Weltintegration und Kollision.

---

## 1. Datenstruktur (`Building`)

Ähnlich wie `Item` brauchst du eine `Building`-Struct mit:

- `id`, `name`, `texturPfad`, `Texture2D textur`
- `width`, `height` (in Tiles – das sind deine Gebäudemaße)
- Callbacks: `onPlace`, `onClick`, `onEnter`, `onLeave`, `onUpdate`
- Optional: `isWalkable` (blockiert der Boden darunter?)
- Optional: `isSolid` (kollidiert der Spieler damit?)

---

## 2. Manager (`BuildingManager`)

Exakt wie `ItemManager`, mit:

- `unordered_map<string, unique_ptr<Building>>` für alle geladenen Gebäude
- `functionRegistry` für Callback-Binding (gleiche Logik wie bei Items)
- `scanAndLoadBuildings()` liest aus `assets/json/Buildings/houses.json` (und weiteren JSON-Dateien im Ordner)
- `getBuilding(id)` gibt einen Pointer zurück
- Singleton via `getBuildingManager()`

---

## 3. JSON-Loader

Beim Laden einer `.json`-Datei aus dem `Buildings`-Ordner liest du:

- `name`, `textur` (mit `normalizePath` wie bei Grounds/Items)
- `width` und `height` — Hinweis: im aktuellen JSON steht `"with"`, das ist ein Tippfehler, entscheid dich für `"width"`
- Optional: `walkable`, `solid`, Callback-Namen als Strings (wie bei Items)

Beispiel-JSON (korrigiert):

```json
{
    "House": {
        "name": "haus1",
        "textur": "assets/Images/Map/concrete.png",
        "height": 3,
        "width": 4
    }
}
```

---

## 4. Callback-Makro (`BUILDING_BEGIN` / `BUILDING_END`)

Du erstellst eine `building_api.h` analog zu `item_api.h`. Sie enthält:

- `BUILDING_BEGIN("House", House)` → öffnet einen eigenen Namespace `_building_impl_House`
- `BUILDING_END("House")` → registriert den Auto-Registrar
- Fünf fest vorgegebene Methoden die der Modder befüllt:

| Methode | Wann sie aufgerufen wird |
|---|---|
| `onPlace()` | Gebäude wird in der Welt platziert |
| `onClick()` | Spieler klickt auf das Gebäude |
| `onEnter()` | Spieler betritt das Gebäude |
| `onLeave()` | Spieler verlässt das Gebäude |
| `onUpdate()` | Jeden Frame (Animationen, Logik) |

Eine Modder-Datei sieht dann so aus:

```cpp
// items/House.cpp
#include "building_api.h"

BUILDING_BEGIN("House", House)

    void onPlace() {
        // z.B. Sound abspielen
    }

    void onClick() {
        // z.B. Tür öffnen
    }

    void onEnter() {
        // z.B. Innenraum laden
    }

    void onLeave() {
        // z.B. Außenwelt wieder einblenden
    }

    void onUpdate() {
        // leer lassen wenn nicht gebraucht
    }

BUILDING_END("House")
```

---

## 5. Karte / Welt-Integration

Gebäude sind **keine Tiles** – sie sind eigene Objekte auf der Karte.

### `PlacedBuilding`-Struct

Eine neue Struct die ein platziertes Gebäude in der Welt beschreibt:

- `buildingId` (z.B. `"House"`)
- `x`, `y` (Ursprungs-Tile oben-links)

### Erweiterung der `Map`-Struct

- Neue `unordered_map<string, PlacedBuilding> placedBuildings` mit Key `"x,y"`
- `placeBuilding(id, x, y)` – fügt ein Gebäude ein, prüft ob der Platz frei ist
- `removeBuilding(x, y)` – entfernt ein Gebäude
- `getBuildingAt(x, y)` – gibt Pointer auf `PlacedBuilding` zurück (oder `nullptr`)

### Erweiterung der `world.json`

`Map::save()` und `Map::load()` bekommen einen zusätzlichen `"buildings"`-Block:

```json
{
    "tiles": { ... },
    "buildings": {
        "5,3": { "id": "House", "x": 5, "y": 3 },
        "12,7": { "id": "House", "x": 12, "y": 7 }
    }
}
```

---

## 6. Zeichnen

Eine neue Funktion `draw_buildings(const Map& world, const BuildingDatabase& db, int tileSize)` – analog zu `draw_ground()`:

- Iteriert über `placedBuildings`
- Zeichnet die Textur skaliert auf `width * tileSize` × `height * tileSize` Pixel
- Wird in der Haupt-Schleife **nach** `draw_ground()` aufgerufen, damit Gebäude über dem Boden liegen

---

## 7. Kollision & Betreten

### Kollision

Wenn der Spieler ein Tile betritt das von einem Gebäude belegt ist:

- `isSolid == true` → Spieler kann nicht rein (Wand), Bewegung blockieren
- `isSolid == false` → Spieler kann hindurch (Innenraum, Tor)

Dafür prüfst du in `updatePlayer()` nach der Bewegung: liegt die neue Spielerposition auf einem Tile das ein solides Gebäude belegt? Wenn ja, Bewegung zurücksetzen.

### Betreten / Verlassen

- Im `player` merkst du dir `currentBuildingId` (leer = draußen)
- Jeden Frame in `updatePlayer()`: Spielerposition → Tile-Koordinate → `world.getBuildingAt(tx, ty)`
- Wenn das Ergebnis sich vom vorherigen Frame unterscheidet → `onLeave()` des alten, `onEnter()` des neuen Gebäudes auslösen

---

## 8. Platzier-System (Hotbar-Integration)

Ein Gebäude wird über ein ganz normales **Item** in der Hotbar platziert, z.B. `"HouseItem"`. Die Item-Datei (`HouseItem.cpp`) nutzt das bestehende Item-System:

- `onHand()` → `setBuildMode(true)` (wie beim Gras-Item)
- `onClick()` → `world.placeBuilding("House", tileX, tileY)`, danach Item aus Inventar entfernen

Das hält beide Systeme sauber getrennt: **Items** sind das Werkzeug, **Buildings** sind das Ergebnis.

### Baumodus-Vorschau

In `drawBuildModeGrid()` (in `Mouse_tile.h`) gibt es bereits TODO-Kommentare für `tileBreite`/`tileHoehe`. Dort liest du die Gebäudegröße aus dem aktiven Item (oder direkt aus dem `BuildingManager`) und zeichnest den Vorschau-Rahmen entsprechend breiter:

- Rahmen: `width * tileSize` × `height * tileSize`
- Textur-Vorschau: skaliert auf dieselbe Größe, halbtransparent

---

## 9. Reihenfolge der Implementierung

Schritt für Schritt, damit jede Phase testbar ist:

1. **`Building`-Struct** und **`BuildingManager`** schreiben (analog zu `Item` / `ItemManager`)
2. **JSON-Loader** für den `Buildings`-Ordner (analog zu `Items.cpp`)
3. **`building_api.h`** mit `BUILDING_BEGIN` / `BUILDING_END` Makros schreiben
4. **`PlacedBuilding`** in die `Map`-Struct einbauen + `world.json` erweitern
5. **`draw_buildings()`** Funktion schreiben und in die Hauptschleife einbinden
6. **`HouseItem.cpp`** als Platzier-Item schreiben (nutzt normales Item-System)
7. **`House.cpp`** mit Gebäude-Callbacks schreiben (nutzt `building_api.h`)
8. **Kollision** in `updatePlayer()` einbauen
9. **`onEnter`/`onLeave`** in `updatePlayer()` einbauen

---

## Zusammenfassung

```
Item-System          Building-System
──────────────────   ──────────────────────────────
Item                 Building
ItemManager          BuildingManager
item_api.h           building_api.h
ITEM_BEGIN/END       BUILDING_BEGIN/END
inventory slot       PlacedBuilding in Map
Hotbar → onClick()   HouseItem.cpp → placeBuilding()
```

Das Schlüsselprinzip: Items **platzieren** Gebäude, aber Gebäude haben ihr **eigenes Callback-System**.