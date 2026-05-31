# Neue Ideen – Zu implementieren

---

## 1. Objekte (Objects / Props)

Objekte sind Dinge die über dem Boden liegen – zwischen Boden-Tiles und Spieler gerendert.
Beispiele: Möbel, Deko, Truhen, Bäume, Felsen, Schalter, Schilder.

**Unterschied zu Gebäuden:** Objekte sind kleiner, einfacher, kein Interior-System nötig.
**Unterschied zu Items:** Objekte liegen in der Welt, nicht im Inventar.

### Modder-API (Makro-Stil wie NPC/Building):
```cpp
OBJECT_BEGIN("truhe", Truhe)

    void onInteract() {        // Spieler drückt E in der Nähe
        openUI("truhe_inhalt");
    }

    void onUpdate() { }        // Jeden Frame

    void onDestroy() {         // Wenn Objekt entfernt wird
        addItemToPlayer("holz", 2);
    }

OBJECT_END("truhe")
```

### Eigenschaften:
- Position (x, y) in der Welt gespeichert
- Optional: Kollision (blockiert Spieler/NPCs)
- Optional: Interaktions-Radius (E-Taste Prompt erscheint)
- Optional: Zerstörbar (HP, Schadens-Callback)
- Optional: Animiert (mehrere Sprite-Frames)
- Rendering-Layer: über Boden, unter Spieler (oder über Spieler konfigurierbar)
- Gespeichert in der Welt-JSON wie Gebäude

### JSON-Definition (assets/json/objects/):
```json
{
  "truhe": {
    "texture": "truhe.png",
    "width": 1,
    "height": 1,
    "collidable": true,
    "interact_radius": 40,
    "layer": "below_player"
  }
}
```

### Platzieren:
- Per Item-Callback: `placeObject("truhe", tileX, tileY)`
- Oder direkt in der Welt-JSON vorplatziert

---

## 2. Sound-System

Raylib hat Audio bereits eingebaut – es fehlt eine saubere Modder-API.

### Globale Funktionen (überall nutzbar):
```cpp
playSound("schritte");              // Einmalig abspielen
playMusic("wald_thema");            // Looping Hintergrundmusik
stopMusic();
fadeMusic("dorf_thema", 1.5f);      // Crossfade in 1.5 Sekunden
setSoundVolume("schritte", 0.5f);   // 0.0 bis 1.0
playRandomSound({"schlag1", "schlag2", "schlag3"});  // Zufällig einen
```

### In Callbacks nutzbar:
- Items: `onHand`, `onClick` → Schritt-Sounds, Platzier-Sounds
- Buildings: `onEnter`, `onLeave` → Tür-Sound
- NPCs: `onProximity`, `onDeath` → Sprach-Sounds, Tod-Sound
- Dimensions: `onEnter` → Musik wechseln
- Objekte: `onInteract`, `onDestroy` → Kisten-Sound, Zerstörungs-Sound

### Assets:
- Sound-Dateien in `assets/sounds/` (WAV, OGG, MP3)
- Musik in `assets/music/`
- JSON-Registrierung in `assets/json/sounds.json`:
```json
{
  "schritte": "sounds/schritte.wav",
  "wald_thema": "music/wald.ogg"
}
```

### Räumlicher Sound (optional):
- `playSoundAt("explosion", x, y)` – wird leiser je weiter der Spieler weg ist

---

## 3. Partikel-System

Visuelle Effekte für Kampf, Magie, Wetter, Feuer, Explosionen usw.

### Einfache API:
```cpp
// Schnell-Spawn (für einfache Effekte):
spawnParticles(x, y, {
    .count     = 20,
    .color     = RED,
    .speed     = 150.0f,
    .lifetime  = 0.8f,
    .size      = 4.0f,
    .spread    = 360.0f,    // Grad, 0 = nur eine Richtung
    .gravity   = 200.0f,    // Pixel/sec² nach unten
    .fade      = true       // Alpha nimmt ab
});
```

### Partikel-Templates (Makro):
```cpp
PARTICLE_BEGIN("blut", Blut)

    void onSpawn(Particle& p) {
        p.color    = RED;
        p.size     = randomFloat(2, 5);
        p.speed    = randomFloat(80, 200);
        p.lifetime = randomFloat(0.3f, 0.7f);
        p.gravity  = 300.0f;
    }

    void onUpdate(Particle& p, float dt) {
        p.color.a = (uint8_t)(255 * p.lifeRatio());  // Fade out
    }

PARTICLE_END("blut")

// Nutzen:
spawnParticleTemplate("blut", x, y, count);
```

### Emitter (dauerhaft):
```cpp
// Feuer, Rauch, Regen – laufen kontinuierlich
ParticleEmitter* feuer = createEmitter("feuer", x, y);
feuer->setRate(30);         // 30 Partikel pro Sekunde
destroyEmitter(feuer);
```

### Eingebaut nutzbar in:
- NPC `onDeath` → Blut-Effekt
- Item `onClick` → Magie-Effekt
- Dimension `onUpdate` → Regen/Schnee-Overlay
- Objekt `onDestroy` → Holzsplitter

---

## 4. Kamera-Modi

Aktuell folgt die Kamera immer dem Spieler. Es soll möglich sein den Modus zu wechseln.

### Kamera-Modi:
```cpp
setCameraMode(CAMERA_FOLLOW_PLAYER);        // Standard
setCameraMode(CAMERA_FOLLOW_NPC, npcId);   // Folgt einem NPC
setCameraMode(CAMERA_FOLLOW_OBJECT, objId); // Folgt einem Objekt
setCameraMode(CAMERA_FREE);                 // Spieler bewegt Kamera frei (WASD/Pfeiltasten)
setCameraMode(CAMERA_FIXED, x, y);          // Kamera steht still an Position
setCameraMode(CAMERA_CINEMATIC);            // Kamera wird per Code gesteuert
```

### Kamera-Bewegung (für Cutscenes / cinematische Szenen):
```cpp
cameraMoveToSmooth(x, y, dauer);    // Fährt in 'dauer' Sekunden zu Punkt
cameraMoveToInstant(x, y);          // Springt sofort
cameraShake(staerke, dauer);        // Schütteln (Explosion, Treffer)
cameraZoomTo(zielZoom, dauer);      // Zoom-Animation
```

### Follow-Einstellungen (für weicheres Folgen):
```cpp
setCameraLerp(0.1f);        // 0.0 = sofort, 1.0 = nie – wie weich die Kamera folgt
setCameraDeadzone(20.0f);   // Spieler kann sich 20px bewegen bevor Kamera reagiert
setCameraOffset(0, -50);    // Kamera etwas oberhalb des Spielers zentrieren
```

### Grenzen:
```cpp
setCameraBounds(0, 0, weltBreite, weltHoehe);  // Kamera verlässt nie diese Zone
clearCameraBounds();
```

### Nutzen in Callbacks:
- Dimension `onEnter` → Kamera auf Mittelpunkt des Raumes fixieren
- Cutscene → Kamera auf NPC schwenken während er spricht
- Boss-Raum → `setCameraMode(CAMERA_FIXED)` damit man die Arena sieht
- Strategie-Modus → `setCameraMode(CAMERA_FREE)`
