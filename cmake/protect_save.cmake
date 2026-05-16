# cmake/protect_save.cmake
# Wird von CMake POST_BUILD aufgerufen.
#
# Parameter (via -D übergeben):
#   SRC_ASSETS  – Quell-Assets-Ordner  (z.B. C:/Projekt/assets)
#   DST_ASSETS  – Ziel-Assets-Ordner   (z.B. C:/Projekt/build/bin/Debug/assets)
#
# Logik:
#   1. Alle Assets werden mit copy_directory kopiert
#      (überschreibt Texturen, JSONs, Configs usw. → immer aktuell)
#   2. Spielstand-Dateien werden danach WIEDERHERGESTELLT wenn sie
#      vorher schon existiert haben (Backup-/Restore-Prinzip)
#
# Geschützte Spielstand-Dateien:
#   - assets/json/Map/welt.json
#   - assets/json/player/player.json
#   - assets/json/player.json        (alter Pfad, Fallback)

cmake_minimum_required(VERSION 3.20)

# ── Sicherheitscheck ──────────────────────────────────────────────────────────
if(NOT DEFINED SRC_ASSETS OR NOT DEFINED DST_ASSETS)
    message(FATAL_ERROR "protect_save.cmake: SRC_ASSETS und DST_ASSETS muessen gesetzt sein!")
endif()

# ── Liste der zu schützenden Spielstand-Dateien ───────────────────────────────
# Pfade relativ zu DST_ASSETS
set(SAVE_FILES
    "json/Map/welt.json"
    "json/player/player.json"
    "json/player.json"
)

# ── Schritt 1: Spielstände sichern (in Memory – als CMake-Variablen) ──────────
# Wir merken uns welche Dateien existiert haben und lesen ihren Inhalt
foreach(SAVE_FILE ${SAVE_FILES})
    set(DST_PATH "${DST_ASSETS}/${SAVE_FILE}")
    if(EXISTS "${DST_PATH}")
        file(READ "${DST_PATH}" SAVE_CONTENT_${SAVE_FILE})
        set(SAVE_EXISTS_${SAVE_FILE} TRUE)
        message(STATUS "[protect_save] Sichere: ${SAVE_FILE}")
    else()
        set(SAVE_EXISTS_${SAVE_FILE} FALSE)
    endif()
endforeach()

# ── Schritt 2: Alle Assets kopieren (überschreibt alles) ─────────────────────
message(STATUS "[protect_save] Kopiere Assets: ${SRC_ASSETS} -> ${DST_ASSETS}")
file(COPY "${SRC_ASSETS}/" DESTINATION "${DST_ASSETS}")

# ── Schritt 3: Spielstände wiederherstellen ───────────────────────────────────
foreach(SAVE_FILE ${SAVE_FILES})
    if(SAVE_EXISTS_${SAVE_FILE})
        set(DST_PATH "${DST_ASSETS}/${SAVE_FILE}")
        # Verzeichnis anlegen falls nötig
        get_filename_component(DST_DIR "${DST_PATH}" DIRECTORY)
        file(MAKE_DIRECTORY "${DST_DIR}")
        # Gespeicherten Inhalt zurückschreiben
        file(WRITE "${DST_PATH}" "${SAVE_CONTENT_${SAVE_FILE}}")
        message(STATUS "[protect_save] Wiederhergestellt: ${SAVE_FILE}")
    else()
        message(STATUS "[protect_save] Neu angelegt (kein alter Stand): ${SAVE_FILE}")
    endif()
endforeach()

message(STATUS "[protect_save] Fertig.")