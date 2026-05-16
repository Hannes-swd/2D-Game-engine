#include "player.h"
#include "map.h"
#include "mouse tile.h"
#include <iostream>

extern Map welt;

void grasItemklick() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 Mousepos = getTileMousePos(32);
        welt.setTile((int)Mousepos.x, (int)Mousepos.y, "gras");
    }
    
}

void grasItemInInventar() {
    //std::cout << "[grasItem] Im Inventar" << std::endl;
}

void grasItemInHand() {
    //std::cout << "[grasItem] In der Hand" << std::endl;
}