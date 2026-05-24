# How to Create a Building

> This guide uses a **House** as a practical example to walk you through the process.

---

## Step 1 — Register your building in `houses.json`

Open [`assets/json/Buildings/houses.json`](assets/json/Buildings/houses.json) and add a new entry:

```json
"House": {
    "name": "Small House",
    "textur": "assets/Images/Buildings/Haus1.png",
    "width": 2,
    "height": 1,
    "solid": true
}
```

| Field | Description |
|---|---|
| `name` | Display name of the building |
| `textur` | Path to the building's sprite |
| `width` | Width in tiles |
| `height` | Height in tiles |
| `solid` | Whether the player collides with it |

> **Note:** The engine automatically binds all six callbacks from your `.cpp` file as long as the `BUILDING_BEGIN` ID matches the key in `houses.json`.

---

## Step 2 — Create the building source file

Inside the [`buildings/`](buildings/) folder, create a new `.cpp` file (e.g. `House.cpp`).

All six callback functions must always be named exactly as shown:

```cpp
#include "building_api.h"

BUILDING_BEGIN("House", House)

    // Called every frame while the mouse is over this building.
    void onHover() {

    }

    // Called when the player clicks on this building.
    void onClick() {

    }

    // Called once when this building is placed in the world.
    void onPlace() {

    }

    // Called when the player walks onto the building's tiles.
    void onEnter() {

    }

    // Called when the player walks off the building's tiles.
    void onLeave() {

    }

    // Called every frame for every placed instance of this building type.
    void onUpdate() {

    }

BUILDING_END("House")
```

> **Important:** The ID string in `BUILDING_BEGIN` and `BUILDING_END` must **exactly match** the key in `houses.json` — here `"House"`. The second argument (here `House`) must be a valid C++ identifier with no spaces or special characters.

---

## Step 3 — Create an item that places the building

Buildings are placed through items. Open [`items/`](items/) and create or edit an item `.cpp` file.

Use `setBuildMode(true)` in `onHand()` to show the placement grid, then call `placeBuilding()` in `onClick()`:

```cpp
#include "building_api.h"

ITEM_BEGIN("HouseItem", HouseItem)

    void onHand() {
        setBuildMode(true);
    }

    void onClick() {
        if (!isBuildMode()) return;
        if (leftClickPressed()) {
            Vector2 t = getTileMouse();
            placeBuilding("House", (int)t.x, (int)t.y);
        }
    }

    void onInventory() {
    }

ITEM_END("HouseItem")
```

Don't forget to register the item in [`assets/json/items/item.json`](assets/json/items/item.json):

```json
"HouseItem": {
    "name": "House",
    "textur": "assets/Images/Buildings/Haus1.png"
}
```

---

## How it connects

```
houses.json        BUILDING_BEGIN id     House.cpp
───────────────    ─────────────────     ─────────────────────────
"House"        ──► "House"          ──► void onHover()  { }
                                    ──► void onClick()  { }
                                    ──► void onPlace()  { }
                                    ──► void onEnter()  { }
                                    ──► void onLeave()  { }
                                    ──► void onUpdate() { }
```

---

## Callback reference

| Callback | When it runs |
|---|---|
| `onHover()` | Every frame the mouse is over the building |
| `onClick()` | When the player left-clicks the building |
| `onPlace()` | Once, at the moment the building is placed |
| `onEnter()` | When the player walks onto the building's tiles |
| `onLeave()` | When the player walks off the building's tiles |
| `onUpdate()` | Every frame, for every placed instance of this type |

---

## Functions available inside callbacks

| Function | Description |
|---|---|
| `setBuildMode(bool)` | Enable / disable the placement grid |
| `isBuildMode()` | Returns `true` if build mode is active |
| `getTileMouse()` | Tile coordinates under the mouse cursor |
| `leftClickPressed()` | `true` on the first frame of a left click |
| `leftClick()` | `true` every frame the left button is held |
| `rightClick()` | `true` every frame the right button is held |
| `IsMouseOnUi()` | `true` if the mouse is over the hotbar or inventory |
| `setTile(x, y, type)` | Place a ground tile at the given coordinates |
| `getState()` | Returns the instance's current state string |
| `setState(value)` | Sets the instance's state string |
| `getInstanceId()` | Returns the unique ID of this placed instance |
| `placeBuilding(id, x, y)` | Place a building at the given tile coordinates |

---

## Full example — House with hover tooltip

```cpp
#include "building_api.h"

BUILDING_BEGIN("House", House)

    void onHover() {
        // draw a tooltip above the building
        DrawText("[Click] Enter", 10, 10, 20, WHITE);
    }

    void onClick() {
        // switch to the interior dimension when clicked
        // (interior system not yet implemented)
    }

    void onPlace()  { }
    void onEnter()  { }
    void onLeave()  { }
    void onUpdate() { }

BUILDING_END("House")
```

---

That's it! Once `houses.json` and your `.cpp` file are in place, the engine will automatically register and load your new building.
