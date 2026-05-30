#include "ui_api.h"
#include "building_api.h"

UI_WINDOW_BEGIN("house_entrance")

    UI_TEXT("Willst du das Gebäude betreten?")
    UI_SEPARATOR()

    UI_BUTTON("Ja", []() {
        switchDimension(g_pendingInteriorDimId);
        closeCurrentUI();
    })

    UI_BUTTON("Nein", []() {
        closeCurrentUI();
    })

UI_WINDOW_END("house_entrance")
