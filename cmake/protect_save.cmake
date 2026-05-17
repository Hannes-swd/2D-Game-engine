# cmake/protect_save.cmake
# Wird von CMake POST_BUILD aufgerufen.
#
# Parameter (via -D übergeben):
#   SRC_ASSETS  – Quell-Assets-Ordner  (z.B. C:/Projekt/assets)
#   DST_ASSETS  – Ziel-Assets-Ordner   (z.B. C:/Projekt/build/bin/Debug/assets)
#
# Logik:
#   Alle Assets werden in den Build-Ordner kopiert (Texturen, Configs usw.),
#   damit das Binary sie zur Laufzeit findet (z.B. für Texture-Pfade die
#   relativ zur .exe aufgelöst werden).
#
#   Spielstand-Dateien (welt.json, player.json) werden NICHT mehr kopiert –
#   das Spiel liest/schreibt diese jetzt direkt im Projektordner über den
#   zur Compile-Zeit eingebauten ASSETS_PATH.

cmake_minimum_required(VERSION 3.20)

# ── Sicherheitscheck ──────────────────────────────────────────────────────────
if(NOT DEFINED SRC_ASSETS OR NOT DEFINED DST_ASSETS)
    message(FATAL_ERROR "protect_save.cmake: SRC_ASSETS und DST_ASSETS muessen gesetzt sein!")
endif()

# ── Spielstand-Dateien aus der Kopie ausschliessen ───────────────────────────
# Diese werden vom Spiel direkt im Projektordner (SRC_ASSETS) verwaltet.
set(SAVE_FILES
    "json/Map/welt.json"
    "json/player/player.json"
    "json/player.json"
)

# ── Schritt 1: Alle Assets in den Build-Ordner kopieren ──────────────────────
message(STATUS "[protect_save] Kopiere Assets: ${SRC_ASSETS} -> ${DST_ASSETS}")
file(COPY "${SRC_ASSETS}/" DESTINATION "${DST_ASSETS}")

# ── Schritt 2: Spielstand-Dateien aus dem Build-Ordner wieder loeschen ───────
# Sie wurden gerade mitgekopiert – wir entfernen sie damit nicht
# aus Versehen ein alter Build-Stand die Quelldateien ueberschreibt.
foreach(SAVE_FILE ${SAVE_FILES})
    set(DST_PATH "${DST_ASSETS}/${SAVE_FILE}")
    if(EXISTS "${DST_PATH}")
        file(REMOVE "${DST_PATH}")
        message(STATUS "[protect_save] Entfernt aus Build (liegt im Projekt): ${SAVE_FILE}")
    endif()
endforeach()

message(STATUS "[protect_save] Fertig.")