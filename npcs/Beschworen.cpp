#include "npc_api.h"

NPC_BEGIN("beschworen", Beschworen)

    NPC_VAR(bool, following, false)

    void onSpawn() {
        NPC_SET(following, false);
        npcSetWander(npc, true);
    }

    void onUpdate() { }

    void onProximity() {
        bool isFollowing = NPC_GET(following);
        setPendingTooltip(isFollowing ? "[N] Folgen stoppen" : "[N] Folge mir");

        if (IsKeyPressed(KEY_N)) {
            if (isFollowing) {
                NPC_SET(following, false);
                npcSetWander(npc, true);
            } else {
                NPC_SET(following, true);
                npcFollowPlayer(npc);
            }
        }
    }

    void onLeaveProximity() { }

NPC_END("beschworen")
