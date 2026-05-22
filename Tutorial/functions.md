# Functions Reference

---

## setBuildMode

```cpp
setBuildMode(bool active)
```

Enables or disables build mode. When active, the engine shows a placement grid and a preview of the item under the cursor.

> Call this inside `onHand()` so the grid shows automatically while the item is held. The engine resets build mode to `false` every frame, so switching items turns it off automatically.

```cpp
void onHand() {
    setBuildMode(true);
}
```

---

## isBuildMode

```cpp
bool isBuildMode()
```

Returns `true` if build mode is currently active.

```cpp
if (isBuildMode()) {
    // build mode is on
}
```

---

## setTile

```cpp
setTile(int x, int y, std::string type)
```

Places a tile on the map at the given tile coordinates.

- `x` — tile column
- `y` — tile row
- `type` — tile type ID (must exist in `ground.json`)

```cpp
setTile(3, 5, "grass");
```

---

## getTileMouse

```cpp
Vector2 getTileMouse()
```

Returns the tile coordinates currently under the mouse cursor as a `Vector2`.

```cpp
Vector2 tile = getTileMouse();
// tile.x = column, tile.y = row
```

---

## IsMouseOnUi

```cpp
bool IsMouseOnUi()
```

Returns `true` if the mouse is currently over a UI element (hotbar or inventory).

> `onClick()` already skips execution automatically when the mouse is over the UI, so you usually don't need to call this manually inside `onClick()`.

```cpp
if (IsMouseOnUi()) {
    // mouse is over the UI
}
```

---

## leftClick

```cpp
bool leftClick()
```

Returns `true` every frame the left mouse button is held down.

```cpp
if (leftClick()) {
    // runs every frame the button is held
}
```

---

## leftClickPressed

```cpp
bool leftClickPressed()
```

Returns `true` only on the first frame the left mouse button is pressed.

```cpp
if (leftClickPressed()) {
    // runs once per click
}
```

---

## rightClick

```cpp
bool rightClick()
```

Returns `true` every frame the right mouse button is held down.

```cpp
if (rightClick()) {
    // ...
}
```

---

## addToInventory

```cpp
g_player->addToInventory(std::string itemId, int amount)
```

Adds an item to the player's inventory. If the player already has the item, the amount is added to the existing stack.

- `itemId` — item ID (must exist in `item.json`)
- `amount` — how many to add

```cpp
g_player->addToInventory("grassItem", 5);
```

---

## removeFromInventory

```cpp
g_player->removeFromInventory(std::string itemId, int amount)
```

Removes an item from the player's inventory. If the count reaches zero the slot is cleared automatically.

- `itemId` — item ID
- `amount` — how many to remove

```cpp
g_player->removeFromInventory("grassItem", 1);
```

---

## hasItem

```cpp
bool g_player->hasItem(std::string itemId)
```

Returns `true` if the player has at least one of the given item in their inventory.

```cpp
if (g_player->hasItem("grassItem")) {
    // player is carrying grass
}
```

---

## getTile

```cpp
std::string world.getTile(int x, int y)
```

Returns the tile type ID at the given coordinates. Returns the default tile type if no tile has been placed there.

```cpp
std::string type = world.getTile(3, 5);
if (type == "grass") {
    // tile is grass
}
```