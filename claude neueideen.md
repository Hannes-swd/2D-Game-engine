# Ideen für Voxira – Systeme die es noch nicht gibt

## Raycast-Licht & Schatten
Echte Lichtquellen die Schatten werfen. Kein simples Alpha-Overlay,
sondern pro Lichtquelle einen Sichtbereich berechnen (Shadow Casting mit
Raymarching oder Polygon-Clipping). Objekte/Gebäude werfen Schatten.
Braucht einen eigenen Render-Pass mit RenderTexture2D + Shader.

## Lua-Scripting
Lua (z.B. sol2 als Binding) in Objekte/NPCs einbetten.
Statt alles in C++ zu hardcoden: `"onInteract": "scripts/feuer.lua"` im JSON.
Ermöglicht Spiellogik ohne Neukompilierung. Besonders nützlich für
Quest-Trigger, Fallen, spezielle NPC-Reaktionen.

## Autotiling (Wang-Tiles)
Tiles kennen derzeit keine Nachbarn. Mit Autotiling wählt das System
automatisch das richtige Tile-Sprite (Ecken, Kanten, Mitte) basierend
auf den 8 Nachbar-Tiles. Braucht ein Bitmask-System im GroundDatabase.
Ergebnis: Grass-/Wasser-/Weg-Ränder sehen automatisch korrekt aus.

## Prozedurale Weltgenerierung
Neue Welten per Knopfdruck generieren statt alles von Hand in JSON zu legen.
Perlin Noise (ist in raylib als `stb_perlin` bereits enthalten!) für Terrain,
dann Regeln drüberlegen: Wasser bei niedrigem Wert, Gras mittig, Berg oben.
Gebäude und NPCs dann zufällig in gültigen Zonen platzieren.

## Shader-Effekte (GLSL)
raylib unterstützt Custom Shaders. Ideen:
- Wasser-Shader: Wellenbewegung per UV-Verzerrung
- Outline-Shader: Ausgewählte Objekte im Editor hervorheben
- Retro-CRT-Effekt: Scanlines als Post-Processing

## Hot-Reload für Assets
JSON-Dateien zur Laufzeit neu laden ohne Neustart. FileSystemWatcher-Loop
der `GetFileModTime()` (raylib) überwacht. Änderung erkannt → nur den
betroffenen Manager neu laden (z.B. nur `g_itemManager`).
Riesige Zeitersparnis beim Entwickeln.

## Chunk-basierte Welt
Derzeit wird die ganze Welt auf einmal gehalten. Bei größeren Welten
wird das zum Problem. Lösung: Welt in Chunks (z.B. 16×16 Tiles) aufteilen,
nur Chunks im Sichtbereich + 1 Ring drum herum laden. Braucht neues
Map-Format und einen ChunkManager der Laden/Entladen/Speichern handhabt.

## Navmesh statt Grid-Pathfinding
Falls der aktuelle NPC-Pathfinder Grid-basiert ist: ein echter Navmesh
(navigierbares Polygon-Netz) erlaubt NPCs natürlicher zu laufen.
Alternativ: Flow Fields für viele NPCs gleichzeitig (alle teilen sich
ein berechnetes Richtungsfeld → viel performanter als einzelne A*-Pfade).

## Replay-System
Alle Spieler-Inputs mit Timestamp aufzeichnen (Input + delta).
Replay abspielbar durch deterministisches Wiederholen der Inputs.
Nützlich für Debugging, Cutscenes, oder "Geist"-NPCs die
eine aufgezeichnete Route ablaufen.
