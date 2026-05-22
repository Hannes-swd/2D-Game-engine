# How to Modify the Game

> This guide uses a **Grass Item** as a practical example to walk you through the process.

---

## Step 1 — Register your item in `item.json`

Open [`assets/json/items/item.json`](assets/json/items/item.json) and add a new entry using the following template:

```json
"grassItem": {
    "name": "grass",
    "texture": "assets/Images/Map/grass.png",
    "onClick":     "grassItemClick",
    "inInventory": "grassItemInInventory",
    "inHand":      "grassItemInHand"
}
```

| Field | Description |
|---|---|
| `name` | Display name of the item |
| `texture` | Path to the item's sprite/image |
| `onClick` | The name of the function in your `.cpp` file that runs when the player clicks with this item selected |
| `inInventory` | The name of the function in your `.cpp` file that runs every tick while the item is in the inventory |
| `inHand` | The name of the function in your `.cpp` file that runs every tick while the item is held/selected |

> **Important:** The function names you write here (e.g. `"grassItemClick"`) must **exactly match** the function names in your `.cpp` file. This is how the engine knows which code to call.

---

## Step 2 — Create the item source file

Inside the [`items/`](items/) folder, create a new `.cpp` file for your item (e.g. `grassItem.cpp`).

The function names here **must match** what you wrote in `item.json`:

```cpp
#include "item_api.h"
#include "items.h"

ITEM_BEGIN("grassItem", grassItem)

    // Matches "inHand": "grassItemInHand" in item.json
    void grassItemInHand() {
        // Runs every tick while the item is held/selected
    }

    // Matches "onClick": "grassItemClick" in item.json
    void grassItemClick() {
        // Runs when the player clicks with this item selected
    }

    // Matches "inInventory": "grassItemInInventory" in item.json
    void grassItemInInventory() {
        // Runs every tick while the item is in the inventory
    }

ITEM_END("grassItem")
```

> **Note:** The ID string passed to `ITEM_BEGIN` and `ITEM_END` must also **exactly match** the key in `item.json` — in this case `"grassItem"`.

---

## How it connects

```
item.json                        grassItem.cpp
─────────────────────────────    ─────────────────────────────
"onClick":    "grassItemClick" ──► void grassItemClick()    { }
"inInventory":"grassItemInInventory"──► void grassItemInInventory() { }
"inHand":     "grassItemInHand"──► void grassItemInHand()   { }
```

The engine reads the function names from `item.json` and calls the matching functions in your `.cpp` file. If the names don't match, the functions won't be called.

---

That's it! Once both files are in place, the engine will automatically register and load your new item.