# Ideen

## Modder UI-System

Jedes UI-Element ist eine eigene Datei (wie Items/Buildings).
Die Engine findet sie automatisch per GLOB_RECURSE.

---

### Button

```cpp
UI_BUTTON_BEGIN(attack_button)
    UI_POS(100, 50)
    UI_SIZE(80, 30)
    UI_TEXT("Angriff")

    UI_ON_PRESS([]{ 
        // einmal ausgeführt beim ersten Drücken
        player.startAttack();
    })

    UI_ON_HOLD([]{ 
        // jeden Frame ausgeführt solange gedrückt
        player.chargeAttack();
    })
UI_BUTTON_END
```

---

### Bild anzeigen

```cpp
UI_IMAGE_BEGIN(hud_portrait)
    UI_POS(10, 10)
    UI_SIZE(64, 64)
    UI_TEXTURE("assets/player_portrait.png")  // Pfad direkt hier, keine JSON
UI_IMAGE_END
```

Button mit Textur statt Text:

```cpp
UI_BUTTON_BEGIN(attack_button)
    UI_POS(100, 50)
    UI_SIZE(64, 64)
    UI_TEXTURE("assets/attack_icon.png")      // Textur direkt im cpp

    UI_ON_PRESS([]{
        player.startAttack();
    })

    UI_ON_HOLD([]{
        player.chargeAttack();
    })
UI_BUTTON_END
```

---

### Variable anzeigen

```cpp
UI_LABEL_BEGIN(hp_display)
    UI_POS(80, 10)
    UI_VALUE([]{ return "HP: " + std::to_string(player.hp); })
UI_LABEL_END
```

Der Lambda wird jeden Frame aufgerufen — zeigt immer den aktuellen Wert.

---

### Sichtbarkeit

Alle Elemente können per Bedingung ein-/ausgeblendet werden:

```cpp
UI_VISIBLE_IF([]{ return player.inCombat; })
```
