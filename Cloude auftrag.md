# UI-System für Modder – Anforderungsdokument

## Ziel

Ein flexibles UI-System, das es Moddern ermöglicht, eigene Popup-Fenster mit interaktiven Elementen (Buttons, Texteingaben, Slider etc.) zu erstellen – vollständig in C++, analog zu Items, Buildings und Dimensions.

Das UI kann aus jedem Callback heraus geöffnet werden (z.B. `onClick()` eines Gebäudes, `onUpdate()` einer Dimension, `onClick()` eines Items).

---

## 1. Dateistruktur

### Engine-interne Dateien (werden von Claude implementiert, nicht vom Modder)

```
src/
├── UI.h              // UIWindow, UIElement, UIManager Klassen
├── UI.cpp            // Implementierung (Zeichnen, Event-Handling, Auto-Registrierung)
└── ui_api.h          // Modder-API: Makros (UI_WINDOW_BEGIN/END, UI_BUTTON, ...) und Hilfsfunktionen
```

Diese Dateien entsprechen `src/Buildings.h`, `src/Buildings.cpp` und `src/building_api.h` – der Modder inkludiert nur `ui_api.h`.

### Modder-Dateien (Root-Level, wie items/, buildings/, dimensions/)

```
ui/
├── house_entrance.cpp
├── blacksmith_shop.cpp
└── ...
```

Das CMake-Build-System findet `.cpp`-Dateien in `ui/` automatisch per `GLOB_RECURSE` (genau wie `items/` und `buildings/`).

### JSON-Registrierung

```
assets/json/ui/
└── popups.json
```

---

## 2. Schritt-für-Schritt – Workflow für Modder

### Schritt 1 — Popup in `popups.json` registrieren

Öffne [`assets/json/ui/popups.json`](assets/json/ui/popups.json) und füge einen neuen Eintrag hinzu:

```json
"house_entrance": {
    "title": "Gebäude betreten?",
    "width": 300,
    "height": 200,
    "modal": true,
    "closeOnEsc": true,
    "backgroundColor": [30, 30, 30, 220]
}
```

| Feld | Beschreibung |
|---|---|
| `title` | Fenstertitel (optional – leer lassen für keinen Titel) |
| `width` | Fensterbreite in Pixeln |
| `height` | Fensterhöhe in Pixeln |
| `modal` | `true` → Mausklicks auf die Welt darunter werden blockiert |
| `closeOnEsc` | `true` → ESC schließt das Fenster automatisch |
| `backgroundColor` | Hintergrundfarbe als `[R, G, B, A]` (0–255) |

> **Hinweis:** Die Engine bindet das Popup automatisch an die `.cpp`-Datei, solange `UI_WINDOW_BEGIN` ID mit dem Key in `popups.json` übereinstimmt.

---

### Schritt 2 — Popup-Quelldatei erstellen

Im [`ui/`](ui/) Ordner eine neue `.cpp`-Datei anlegen (z.B. `house_entrance.cpp`).

Anders als bei Items, Buildings und Dimensions gibt es hier **keine Standard-Callbacks** (`onHover`, `onClick`, etc.). Stattdessen beschreibt die Datei das **Layout des Fensters** über Makros:

```cpp
#include "ui_api.h"

UI_WINDOW_BEGIN("house_entrance")

    UI_TEXT("Willst du das Gebäude betreten?")

    UI_BUTTON("Ja", []() {
        switchDimension("house_interior");
        closeCurrentUI();
    })

    UI_BUTTON("Nein", []() {
        closeCurrentUI();
    })

UI_WINDOW_END("house_entrance")
```

> **Wichtig:** Die ID-Zeichenkette in `UI_WINDOW_BEGIN` und `UI_WINDOW_END` muss **exakt** mit dem Key in `popups.json` übereinstimmen – hier `"house_entrance"`.

---

### Schritt 3 — Popup aufrufen

`openUI("id")` kann aus jedem bestehenden Callback aufgerufen werden:

```cpp
// In buildings/House.cpp
void onClick() {
    openUI("house_entrance");
}
```

```cpp
// In items/ShopItem.cpp
void onClick() {
    openUI("blacksmith_shop");
}
```

```cpp
// In dimensions/house_interior.cpp
void onUpdate() {
    if (IsKeyPressed(KEY_E)) {
        openUI("exit_house_menu");
    }
}
```

---

## 3. Wie es verbunden ist

```
popups.json            UI_WINDOW_BEGIN id      house_entrance.cpp
────────────────       ──────────────────      ──────────────────────────────
"house_entrance"   ──► "house_entrance"    ──► UI_TEXT(...)
width: 300                                 ──► UI_BUTTON("Ja",   [](){...})
height: 200                                ──► UI_BUTTON("Nein", [](){...})
modal: true
```

Die Engine liest Größe, Farbe und Optionen aus `popups.json` und verknüpft die Elemente automatisch mit der passenden `UI_WINDOW_BEGIN`-Datei.

---

## 4. UI-Elemente – Vollständige Liste

| Makro | Beschreibung | Parameter |
|---|---|---|
| `UI_WINDOW_BEGIN(id)` | Startet ein neues UI-Fenster | `id` (string) |
| `UI_WINDOW_END(id)` | Beendet das UI-Fenster | `id` (string) |
| `UI_TITLE(text)` | Titel (zentriert, fett) | `text` (string) |
| `UI_TEXT(text)` | Normaler Text | `text` (string) |
| `UI_TEXT_LIVE(id, initial_text)` | Dynamisch aktualisierbarer Text | `id`, `initial_text` (string) |
| `UI_SEPARATOR()` | Trennlinie | – |
| `UI_BUTTON(label, callback)` | Button mit Lambda-Callback | `label` (string), `callback` (lambda) |
| `UI_TEXT_INPUT(id, max_len, callback)` | Texteingabefeld | `id` (string), `max_len` (int), `callback(string)` |
| `UI_NUMBER_INPUT(id, min, max, default, callback)` | Zahlen-Eingabefeld | `id` (string), `min`/`max`/`default` (int), `callback(int)` |
| `UI_SLIDER(id, min, max, default, callback)` | Schieberegler | `id` (string), `min`/`max`/`default` (int), `callback(int)` |
| `UI_CHECKBOX(label, default, callback)` | Checkbox | `label` (string), `default` (bool), `callback(bool)` |
| `UI_DROPDOWN(id, options, default_index, callback)` | Dropdown-Menü | `id` (string), `options` (vector\<string\>), `default_index` (int), `callback(int)` |

---

## 5. Verfügbare Funktionen für Modder

### Fenstersteuerung

| Funktion | Beschreibung |
|---|---|
| `openUI(id)` | Öffnet ein registriertes UI-Fenster |
| `closeCurrentUI()` | Schließt das aktuell geöffnete Fenster |
| `isUIOpen()` | `true` wenn ein UI-Fenster geöffnet ist |
| `getCurrentUIId()` | ID des aktuell geöffneten UI (`""` wenn keins offen) |

### Dynamische UI-Updates

| Funktion | Beschreibung |
|---|---|
| `updateUIText(elementId, newText)` | Aktualisiert einen `UI_TEXT_LIVE`-Text |
| `getUIInputValue(inputId)` | Aktueller Wert eines `UI_TEXT_INPUT` |
| `getUINumberValue(inputId)` | Aktueller Wert eines `UI_NUMBER_INPUT` |
| `getUISliderValue(sliderId)` | Aktueller Wert eines `UI_SLIDER` |
| `getUICheckboxValue(checkboxId)` | Aktueller Wert einer `UI_CHECKBOX` |
| `addUITextLine(text)` | Fügt eine Zeile zu einem Log-Bereich hinzu |

### Globale Zugriffe (bereits vorhanden)

| Variable/Funktion | Beschreibung |
|---|---|
| `g_player` | Spieler-Instanz (Inventar, Position, etc.) |
| `switchDimension(id)` | Wechselt die Dimension (`""` = zurück zur Welt) |
| `leaveCurrentDimension()` | Verlässt die aktuelle Dimension |
| `setTile(x, y, type)` | Setzt einen Tile |
| `IsMouseOnUi()` | `true` wenn Maus über Hotbar/Inventar |

---

## 6. Beispiele

### Beispiel 1: Gebäude-Eingangsfenster

`popups.json`:
```json
"house_entrance": {
    "title": "",
    "width": 300,
    "height": 200,
    "modal": true,
    "closeOnEsc": true,
    "backgroundColor": [30, 30, 30, 220]
}
```

`ui/house_entrance.cpp`:
```cpp
#include "ui_api.h"

UI_WINDOW_BEGIN("house_entrance")

    UI_TEXT("Willst du das Gebäude betreten?")

    UI_BUTTON("Ja", []() {
        switchDimension("house_interior");
        closeCurrentUI();
    })

    UI_BUTTON("Nein", []() {
        closeCurrentUI();
    })

UI_WINDOW_END("house_entrance")
```

---

### Beispiel 2: Shop mit Nachrichten-Log

`popups.json` (zusätzlicher Eintrag):
```json
"blacksmith_shop": {
    "title": "Schmiede",
    "width": 400,
    "height": 350,
    "modal": true,
    "closeOnEsc": true,
    "backgroundColor": [25, 20, 15, 230]
}
```

`ui/blacksmith_shop.cpp`:
```cpp
#include "ui_api.h"

UI_WINDOW_BEGIN("blacksmith_shop")

    UI_TEXT("Was möchtest du kaufen?")
    UI_SEPARATOR()

    UI_BUTTON("Schwert kaufen (10 Gold)", []() {
        if (g_player->hasItem("gold", 10)) {
            g_player->removeFromInventory("gold", 10);
            g_player->addToInventory("iron_sword", 1);
            addUITextLine("Du hast ein Schwert gekauft!");
        } else {
            addUITextLine("Nicht genug Gold!");
        }
    })

    UI_BUTTON("Schließen", []() {
        closeCurrentUI();
    })

UI_WINDOW_END("blacksmith_shop")
```

---

### Beispiel 3: Slider für Lautstärke

`ui/settings_volume.cpp`:
```cpp
#include "ui_api.h"

UI_WINDOW_BEGIN("settings_volume")

    UI_TEXT("Lautstärke:")
    UI_SLIDER("volume_slider", 0, 100, 75, [](int value) {
        setMasterVolume(value / 100.0f);
        updateUIText("volume_label", "Aktuell: " + std::to_string(value) + "%");
    })

    UI_TEXT_LIVE("volume_label", "Aktuell: 75%")

    UI_BUTTON("Schließen", []() {
        closeCurrentUI();
    })

UI_WINDOW_END("settings_volume")
```

---

## 7. UI-Manager / Verhalten

- Nur **ein Fenster** kann gleichzeitig geöffnet sein. `openUI()` schließt ein bereits offenes Fenster automatisch.
- Das UI wird **über allem anderen** gezeichnet (nach Inventar/Hotbar).
- Bei `modal: true` blockiert das Fenster alle Mausklicks auf die darunterliegende Welt.
- Bei `closeOnEsc: true` schließt ESC das Fenster (ruft `closeCurrentUI()` auf).
- Das Fenster wird **automatisch zentriert** auf dem Bildschirm.

---

## 8. Implementierungsnotizen (für Claude)

### src/UI.h

- Klasse `UIElement` (Basis) mit Unterklassen: `UIButtonElement`, `UITextElement`, `UISliderElement`, `UITextInputElement`, `UINumberInputElement`, `UICheckboxElement`, `UIDropdownElement`, `UISeparatorElement`, `UILiveTextElement`
- Klasse `UIWindow` mit: `id`, Referenz auf `popups.json`-Eintrag (Größe, Farbe, Modal, ESC), `std::vector<UIElement*> elements`
- Klasse `UIManager` (Singleton): `registerWindow()`, `openUI()`, `closeCurrentUI()`, `draw()`, `update()`

### src/UI.cpp

- `draw()`: Fenster-Hintergrund + Rand + alle Elemente von oben nach unten
- `update()`: Mausinteraktion mit jedem Element, ESC-Check, Modal-Blockierung
- Raylib-Funktionen: `DrawRectangle()`, `DrawRectangleLines()`, `DrawText()`, `GetMousePosition()`, `IsMouseButtonPressed()`, `CheckCollisionPointRec()`

### src/ui_api.h

Makro-Pattern (analog zu `building_api.h`):

```cpp
struct _UIAutoRegistrar_##cppId {
    _UIAutoRegistrar_##cppId() {
        getUIManager().registerWindow(id, []() {
            // Builder-Lambda wird einmalig beim Start aufgerufen
            // und fügt alle Elemente dem aktuellen Fenster hinzu
        });
    }
};

#define UI_WINDOW_BEGIN(id) \
    static _UIAutoRegistrar_... _reg_##id; \
    // Builder-Lambda startet hier

#define UI_WINDOW_END(id) \
    // Builder-Lambda endet hier
```

### CMakeLists.txt

`ui/` per `GLOB_RECURSE` einbinden (analog zu `items/` und `buildings/`):
```cmake
file(GLOB_RECURSE UI_SOURCES "ui/*.cpp")
target_sources(2DGameEngine PRIVATE ${UI_SOURCES})
```

### main.cpp Integration

```cpp
// Im Update-Loop:
getUIManager().update();

// Im Draw-Loop (nach allem anderen):
getUIManager().draw();
```

### popups.json einlesen

Beim Start liest `UIManager` `assets/json/ui/popups.json` und speichert für jede ID: width, height, title, modal, closeOnEsc, backgroundColor. Diese Werte werden beim `registerWindow()`-Aufruf des Auto-Registrars abgerufen.

---

## 9. Zusammenfassung – Nächste Schritte

1. `src/UI.h` und `src/UI.cpp` implementieren (UIManager, UIWindow, UIElement-Klassen)
2. `src/ui_api.h` mit Makros und Hilfsfunktionen implementieren
3. `assets/json/ui/popups.json` anlegen (leer: `{}`)
4. `ui/` Ordner anlegen (Root-Level)
5. CMakeLists.txt: `ui/*.cpp` per GLOB_RECURSE einbinden
6. `main.cpp`: `getUIManager().update()` und `.draw()` einbinden
7. Tutorial-Dokumentation in `Tutorial/ui.md` für Modder schreiben (analog zu `house.md`, `dimension.md`)

---

**Ende des Anforderungsdokuments**
