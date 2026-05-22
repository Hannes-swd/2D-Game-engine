# How to Modify the Game

> This guide uses a **Grass Item** as a practical example to walk you through the process.

---

## Step 1 — Register your item in `item.json`

Open [`assets/json/items/item.json`](assets/json/items/item.json) and add a new entry using the following template:

```json
"grasItem": {
    "name": "gras",
    "textur": "assets/Images/Map/gras.png",
    "onKlick": "grasItemklick",
    "inInventar": "grasItemInInventar",
    "inHand": "grasItemInHand"
}
```

| Field | Description |
|---|---|
| `name` | Display name of the item |
| `textur` | Path to the item's sprite/image |
| `onKlick` | Name of the function called when the player clicks with this item |
| `inInventar` | Name of the function called every tick while the item is in the inventory |
| `inHand` | Name of the function called every tick while the item is held |

> **Important:** The values of `onKlick`, `inInventar`, and `inHand` are the **exact function names** the engine will look for in your `.cpp` file. They must match character for character.

---

## Step 2 — Create the item source file

Inside the [`items/`](items/) folder, create a new `.cpp` file for your item (e.g. `Gras.cpp`).

The three callback functions **must be named exactly** as defined in `item.json`:

```cpp
#include "item_api.h"

ITEM_BEGIN("grasItem", grasItem)

    // Called every tick while the item is held.
    // Name must match "inHand" in item.json.
    void grasItemInHand() {

    }

    // Called when the player clicks with this item selected.
    // Name must match "onKlick" in item.json.
    void grasItemklick() {

    }

    // Called every tick while the item is in the inventory.
    // Name must match "inInventar" in item.json.
    void grasItemInInventar() {

    }

ITEM_END("grasItem")
```

> **Important:** The ID string in `ITEM_BEGIN` and `ITEM_END` must **exactly match** the key in `item.json` — here `"grasItem"`. The second argument of `ITEM_BEGIN` (here `grasItem`) must be a valid C++ identifier with no spaces or special characters.

> **Important:** The function names are **not fixed** — you define them yourself in `item.json`. Whatever you write as the value of `onKlick`, `inHand`, and `inInventar` is what your `.cpp` functions must be named.

---

## How it connects

```
item.json                          Gras.cpp
──────────────────────────────     ──────────────────────────────
"inHand":    "grasItemInHand"   ──► void grasItemInHand()    { }
"onKlick":   "grasItemklick"    ──► void grasItemklick()     { }
"inInventar":"grasItemInInventar"──► void grasItemInInventar(){ }
```

The engine reads the callback names from `item.json` and binds them to the matching functions in your `.cpp` file at startup.

---

## Full example — Grass placement item

```cpp
#include "item_api.h"

ITEM_BEGIN("grasItem", grasItem)

    void grasItemInHand() {
        setBuildMode(true);  // show placement grid while held
    }

    void grasItemklick() {
        if (!isBuildMode()) return;
        if (leftClickPressed()) {
            Vector2 t = getTileMouse();
            setTile((int)t.x, (int)t.y, "gras");
            g_player->removeFromInventory("grasItem", 1);
        }
    }

    void grasItemInInventar() {
        // nothing
    }

ITEM_END("grasItem")
```

---

That's it! Once both files are in place, the engine will automatically register and load your new item.