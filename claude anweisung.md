# 👗 Clothing System - Detaillierte Anweisung

## 🎨 Überblick

Das Clothing System ermöglicht es Moddern, custom Kleidung für Charaktere zu erstellen. Die Kleidung wird **layered** gerendert und besteht aus separaten PNG-Dateien pro Kleidungsstück und Perspektive.

---

## 📐 Bild-Spezifikationen

### **Alle Sprites: 10×32 Pixel**

Body und alle Kleidungsstücke müssen exakt **10×32 Pixel** groß sein:
- **Breite: 10px** (≈ 1/3 Tile)
- **Höhe: 32px** (= 1 Tile)

```
  |←10px→|
  ┌──────┐  ↑
  │      │  32px (1 Tile hoch)
  │      │
  └──────┘  ↓
```

Alle Layer (Body, Shirt, Pants, Hair) haben **dieselbe Größe** und liegen übereinander.
Jedes Sprite nutzt **PNG-Transparenz** damit nur der relevante Bereich sichtbar ist
(z.B. Hose nur untere Hälfte gefärbt, Rest transparent).

Die Engine zentriert den Charakter automatisch an seiner Position basierend auf der Body-Textur-Größe.
Willst du eine andere Größe, einfach alle Sprites anpassen — der Code passt sich automatisch an.

---

## 📁 Ordnerstruktur

Die Engine lädt Assets über `assetPath()` – alles liegt relativ zum `assets/` Ordner im Projektverzeichnis.

```
assets/
├── json/
│   └── clothing/                    ← JSON-Dateien hier
│       ├── bodies.json              # Body-Varianten (Human, Elf, etc.)
│       ├── pants.json               # Hosen-Varianten
│       ├── shirts.json              # Oberteil-Varianten
│       └── hair.json                # Haar-Varianten
│
└── Images/
    └── clothing/                    ← Alle Kleidungs-PNGs hier
        ├── bodies/
        │   ├── human_frontal.png        (32×96)
        │   ├── human_side.png           (32×96)
        │   ├── elf_frontal.png          (32×96)
        │   └── elf_side.png             (32×96)
        │
        ├── pants/
        │   ├── blue_frontal.png         (32×32)
        │   ├── blue_side.png            (32×32)
        │   ├── red_frontal.png          (32×32)
        │   └── red_side.png             (32×32)
        │
        ├── shirts/
        │   ├── red_frontal.png          (32×32)
        │   ├── red_side.png             (32×32)
        │   ├── blue_frontal.png         (32×32)
        │   └── blue_side.png            (32×32)
        │
        └── hair/
            ├── long_brown_frontal.png   (32×32)
            ├── long_brown_side.png      (32×32)
            ├── long_brown_back.png      (32×32)
            ├── short_black_frontal.png  (32×32)
            ├── short_black_side.png     (32×32)
            └── short_black_back.png     (32×32)
```

**Pfade in den JSON-Dateien** müssen relativ zum `assets/`-Ordner angegeben werden, z.B.:
```
"spriteFrontal": "Images/clothing/pants/blue_frontal.png"
```

---

## 📋 JSON Struktur

### **bodies.json** - Charakter-Basen

```json
{
    "human_base": {
        "name": "Human",
        "spriteFrontal": "Images/clothing/bodies/human_frontal.png",
        "spriteSide": "Images/clothing/bodies/human_side.png",
        "offsetX": 0,
        "offsetY": 0
    },
    "elf_base": {
        "name": "Elf",
        "spriteFrontal": "Images/clothing/bodies/elf_frontal.png",
        "spriteSide": "Images/clothing/bodies/elf_side.png",
        "offsetX": 0,
        "offsetY": 0
    }
}
```

**Felder:**
- `name` (string): Anzeigename im Shop/UI
- `spriteFrontal` (string): Pfad zum Frontal-Bild (oben/unten Perspektive)
- `spriteSide` (string): Pfad zum Seiten-Bild (links/rechts, wird gespiegelt)
- `offsetX` (int): Horizontaler Offset in Pixeln
- `offsetY` (int): Vertikaler Offset in Pixeln

---

### **pants.json** - Hosen

```json
{
    "blue_pants": {
        "name": "Blue Pants",
        "description": "Comfortable blue trousers",
        "spriteFrontal": "Images/clothing/pants/blue_frontal.png",
        "spriteSide": "Images/clothing/pants/blue_side.png",
        "offsetX": 0,
        "offsetY": 2
    },
    "red_pants": {
        "name": "Red Pants",
        "description": "Bold red trousers",
        "spriteFrontal": "Images/clothing/pants/red_frontal.png",
        "spriteSide": "Images/clothing/pants/red_side.png",
        "offsetX": 0,
        "offsetY": 2
    }
}
```

**Felder:**
- `name` (string): Anzeigename
- `description` (string, optional): Beschreibung für Shop
- `spriteFrontal` (string): Frontal-Bild (32×32)
- `spriteSide` (string): Seiten-Bild (32×32, wird für links gespiegelt)
- `offsetX` (int): Horizontaler Versatz vom Body
- `offsetY` (int): Vertikaler Versatz vom Body

---

### **shirts.json** - Oberteil

```json
{
    "red_shirt": {
        "name": "Red Shirt",
        "description": "Classic red shirt",
        "spriteFrontal": "Images/clothing/shirts/red_frontal.png",
        "spriteSide": "Images/clothing/shirts/red_side.png",
        "offsetX": 0,
        "offsetY": 0
    },
    "blue_shirt": {
        "name": "Blue Shirt",
        "spriteFrontal": "Images/clothing/shirts/blue_frontal.png",
        "spriteSide": "Images/clothing/shirts/blue_side.png",
        "offsetX": 0,
        "offsetY": 0
    }
}
```

**Gleiche Struktur wie pants.json**

---

### **hair.json** - Haare (3 Perspektiven!)

```json
{
    "long_brown": {
        "name": "Long Brown Hair",
        "description": "Long brown hair",
        "spriteFrontal": "Images/clothing/hair/long_brown_frontal.png",
        "spriteSide": "Images/clothing/hair/long_brown_side.png",
        "spriteBack": "Images/clothing/hair/long_brown_back.png",
        "offsetX": -1,
        "offsetY": -3
    },
    "short_black": {
        "name": "Short Black Hair",
        "spriteFrontal": "Images/clothing/hair/short_black_frontal.png",
        "spriteSide": "Images/clothing/hair/short_black_side.png",
        "spriteBack": "Images/clothing/hair/short_black_back.png",
        "offsetX": 0,
        "offsetY": -2
    }
}
```

**Felder:**
- `name` (string): Anzeigename
- `description` (string, optional): Beschreibung
- `spriteFrontal` (string): Vorder-Ansicht (oben/unten)
- `spriteSide` (string): Seiten-Ansicht (wird für links gespiegelt)
- `spriteBack` (string): **Rücken-Ansicht** (unique für Haare!)
- `offsetX` (int): Horizontaler Versatz
- `offsetY` (int): Vertikaler Versatz (oft negativ, da Haare oben sind)

---

## 🎨 Pixel-Art Richtlinien

### **Perspektive & Ausrichtung**

#### **Frontal-Bilder (spriteFrontal)**
- Perspektive: **Von vorne schauend** (oben & unten Bewegung)
- Der Body/die Kleidung schauen geradeaus
- Verwendet sowohl für UP-Richtung als auch DOWN-Richtung
- Gleich aussehend von oben und unten

**Beispiel:**
```
Oben (UP):        Unten (DOWN):
  👕                👕
  ▼                 ▲
```
→ **Beide verwenden das gleiche Frontal-Bild**

---

#### **Seiten-Bilder (spriteSide)**
- Perspektive: **Von der Seite schauend** (links & rechts Bewegung)
- Der Body/die Kleidung schauen nach rechts
- Wird **automatisch gespiegelt** für links-Bewegung
- Zeichne nur eine Seite, Engine spiegelt

**Beispiel:**
```
Rechts (RIGHT):     Links (LEFT):
  →👕                👕←
Engine spiegelt automatisch!
```

---

#### **Rücken-Bilder (spriteBack) - NUR Haare**
- Perspektive: **Von hinten schauend**
- Nur für Haare (optional, aber detailreich)
- Unique Ansicht für Rückseite
- Zeigt wie Haare von hinten aussehen

**Beispiel:**
```
Long Hair Back:
  💇
(Seitliches Volumen sichtbar)
```

---

### **Offset-Werte (offsetX, offsetY)**

Die Offset-Werte positionieren die Kleidung relativ zum Body (32×96):

```
Body-Anatomie (32×96):
  0-32px:   Kopf
  32-64px:  Oberkörper
  64-96px:  Unterkörper

offsetX:
  -2 = 2px nach links verschoben
   0 = zentriert
  +2 = 2px nach rechts verschoben

offsetY:
  -64 = nach ganz oben (Kopf/Hair)
  -32 = obere Hälfte (Shirt oben)
    0 = zentriert (Shirt normal)
  +32 = untere Hälfte (Pants)
  +64 = ganz unten
```

**Typische Werte:**
- **Body**: offsetX: 0, offsetY: 0
- **Shirt**: offsetX: 0, offsetY: 0 (überlagert Oberkörper ab 32px)
- **Pants**: offsetX: 0, offsetY: +32 (überlagert Unterkörper ab 64px)
- **Hair**: offsetX: 0, offsetY: -64 (überlagert Kopf ganz oben)

---

### **Farben & Stil**

- **Palette**: Pixel-Art freundliche Farben
- **Linien**: Crisp & clean, keine Anti-Aliasing
- **Details**: Erkennbar auch in 32×32px
- **Konsistenz**: Style passt zu anderen Kleidungsstücken
- **Transparenz**: PNG mit Alpha-Channel für durchsichtige Bereiche

---

## 🎬 Render-Reihenfolge (wichtig!)

Die Kleidung wird **in dieser Reihenfolge gerendert** (von unten nach oben):

```cpp
1. Body (32×96)   ← Komplette Basis (Kopf + Oberkörper + Unterkörper)
   ├─ Kopf (0-32px)
   ├─ Oberkörper (32-64px)
   └─ Unterkörper (64-96px)

2. Shirt (32×32)  ← überlagert Oberkörper (32-64px)
   offsetY = 0

3. Pants (32×32)  ← überlagert Unterkörper (64-96px)
   offsetY = +32

4. Hair (32×32)   ← überlagert Kopf (0-32px)
   offsetY = -64
```

**Wichtig:** Die Reihenfolge ist **fix**, Modder können sie nicht ändern!

---

## 💾 Wie Modder Kleidung erstellt

### **Schritt 1: Bilder zeichnen**

Modder zeichnet in Pixel-Art Editor (z.B. Aseprite, Piskel, Krita):
- `blue_pants_frontal.png` (32×32)
- `blue_pants_side.png` (32×32)

---

### **Schritt 2: JSONs erstellen**

**Manuell oder per Tool:**

**`assets/json/clothing/pants.json` erweitern:**
```json
{
    "blue_pants": {
        "name": "Blue Pants",
        "spriteFrontal": "Images/clothing/pants/blue_frontal.png",
        "spriteSide": "Images/clothing/pants/blue_side.png",
        "offsetX": 0,
        "offsetY": 2
    }
}
```

---

### **Schritt 3: Testen im Spiel**

Modder erstellt neuen Character mit:
```
Body: human_base
Shirt: red_shirt
Pants: blue_pants
Hair: long_brown
```

Engine rendert in Reihenfolge und zeigt layered Character.

---

## ✅ Checkliste für Modder

- [ ] Alle Bilder sind exakt 32×32 Pixel
- [ ] Frontal-Bilder: von vorne gezeichnet
- [ ] Seiten-Bilder: nach rechts schauend
- [ ] PNG-Format mit Alpha-Channel (Transparenz)
- [ ] Dateinamen sind aussagekräftig (z.B. `blue_pants_frontal.png`)
- [ ] JSON Entry hat alle erforderlichen Felder
- [ ] offsetX/Y sind realistisch gewählt (-3 bis +3)
- [ ] Rücken-Bild für Haare (optional aber empfohlen)
- [ ] Stil passt zu anderen Kleidungsstücken
- [ ] Im Spiel getestet und sieht gut aus

---

## 🔧 C++ Engine-Implementierung (für deine SRC)

Die Engine nutzt **raylib** (`Texture2D`, `DrawTexture`, `DrawTextureRec`) und **nlohmann/json**.
Der `direction`-Enum ist in `player.h` definiert: `Up, Down, Right, Left`.
Pfade werden über `assetPath()` aus `main.cpp` aufgelöst.

```cpp
// ── Structs ──────────────────────────────────────────────────────────────────

struct ClothingItem {
    std::string id;
    std::string name;
    std::string spriteFrontalPath;  // 32×32
    std::string spriteSidePath;     // 32×32
    std::string spriteBackPath;     // optional (nur Hair)
    Texture2D   texFrontal = {0};
    Texture2D   texSide    = {0};
    Texture2D   texBack    = {0};
    int offsetX = 0;
    int offsetY = 0;

    void load() {
        if (!spriteFrontalPath.empty()) texFrontal = LoadTexture(assetPath(spriteFrontalPath).c_str());
        if (!spriteSidePath.empty())    texSide    = LoadTexture(assetPath(spriteSidePath).c_str());
        if (!spriteBackPath.empty())    texBack    = LoadTexture(assetPath(spriteBackPath).c_str());
    }
    void unload() {
        if (texFrontal.id) UnloadTexture(texFrontal);
        if (texSide.id)    UnloadTexture(texSide);
        if (texBack.id)    UnloadTexture(texBack);
    }
};

struct BodyData {
    std::string id;
    std::string name;
    std::string spriteFrontalPath;  // 32×96
    std::string spriteSidePath;     // 32×96
    Texture2D   texFrontal = {0};
    Texture2D   texSide    = {0};
    int offsetX = 0;
    int offsetY = 0;

    void load() {
        if (!spriteFrontalPath.empty()) texFrontal = LoadTexture(assetPath(spriteFrontalPath).c_str());
        if (!spriteSidePath.empty())    texSide    = LoadTexture(assetPath(spriteSidePath).c_str());
    }
    void unload() {
        if (texFrontal.id) UnloadTexture(texFrontal);
        if (texSide.id)    UnloadTexture(texSide);
    }
};

// ── JSON laden ───────────────────────────────────────────────────────────────
// Beispiel: bodies aus assets/json/clothing/bodies.json laden

#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;
extern std::string assetPath(const std::string&);

BodyData loadBodyFromJson(const std::string& id, const json& j) {
    BodyData b;
    b.id               = id;
    b.name             = j.value("name", id);
    b.spriteFrontalPath = j.value("spriteFrontal", "");
    b.spriteSidePath   = j.value("spriteSide", "");
    b.offsetX          = j.value("offsetX", 0);
    b.offsetY          = j.value("offsetY", 0);
    b.load();
    return b;
}

// ── Zeichnen ─────────────────────────────────────────────────────────────────

static void drawTexFlipped(Texture2D tex, Vector2 pos) {
    // raylib: negatives width in sourceRec spiegelt horizontal
    Rectangle src  = { 0, 0, -(float)tex.width, (float)tex.height };
    Rectangle dest = { pos.x, pos.y, (float)tex.width, (float)tex.height };
    DrawTexturePro(tex, src, dest, {0,0}, 0.0f, WHITE);
}

void drawBody(const BodyData& body, direction dir, Vector2 pos) {
    pos.x += body.offsetX;
    pos.y += body.offsetY;
    if (dir == Up || dir == Down) {
        if (body.texFrontal.id) DrawTexture(body.texFrontal, (int)pos.x, (int)pos.y, WHITE);
    } else if (dir == Right) {
        if (body.texSide.id)    DrawTexture(body.texSide, (int)pos.x, (int)pos.y, WHITE);
    } else {  // Left – gespiegelt
        if (body.texSide.id)    drawTexFlipped(body.texSide, pos);
    }
}

void drawClothing(const ClothingItem& item, direction dir, Vector2 basePos) {
    Vector2 pos = { basePos.x + item.offsetX, basePos.y + item.offsetY };
    if (dir == Up || dir == Down) {
        if (item.texFrontal.id) DrawTexture(item.texFrontal, (int)pos.x, (int)pos.y, WHITE);
    } else if (dir == Right) {
        if (item.texSide.id)    DrawTexture(item.texSide, (int)pos.x, (int)pos.y, WHITE);
    } else {  // Left – gespiegelt
        if (item.texSide.id)    drawTexFlipped(item.texSide, pos);
    }
}

// Wird in drawPlayer() aufgerufen – Reihenfolge: Body → Shirt → Pants → Hair
void drawCharacter(const BodyData& body,
                   const ClothingItem& shirt,
                   const ClothingItem& pants,
                   const ClothingItem& hair,
                   direction dir,
                   Vector2 basePos) {
    drawBody(body, dir, basePos);
    drawClothing(shirt, dir, basePos);
    drawClothing(pants, dir, basePos);
    drawClothing(hair,  dir, basePos);
}
```

---

## 📝 Format-Zusammenfassung

| Kleidungsstück | Bilder | Größe | Offset-Bereich | Besonderheit |
|---|---|---|---|---|
| **Body** | 2 (Frontal + Seite) | **32×96** | 0, 0 | Komplette Basis (Kopf+Oberkörper+Unterkörper) |
| **Shirt** | 2 (Frontal + Seite) | 32×32 | -32 bis +32 | Überlagert Oberkörper (32-64px) |
| **Pants** | 2 (Frontal + Seite) | 32×32 | +0 bis +32 | Überlagert Unterkörper (64-96px) |
| **Hair** | 3 (Frontal + Seite + Rücken) | 32×32 | -96 bis -32 | Überlagert Kopf (0-32px), 3 Perspektiven |

---

## 🚀 Nächste Schritte

1. **Basis-Bodies erstellen** (Human, Elf, etc.)
2. **Modder können damit beginnen**, custom Kleidung zu erstellen
3. **WPF App erweitern** für Clothing-Verwaltung
4. **Animation Layer** (später, wenn Basis stabil)

---

**Viel Spaß beim Erstellen! 👗✨**