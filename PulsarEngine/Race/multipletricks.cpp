#include <kamek.hpp>
#include <MarioKartWii/Kart/KartManager.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <MarioKartWii/Kart/KartPlayer.hpp>
#include <MarioKartWii/Kart/KartPointers.hpp>
#include <MarioKartWii/Kart/KartStatus.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <PulsarSystem.hpp>
#include <Race/multipletricks.hpp>

namespace Pulsar {
namespace Race {

// This structure holds the state for our custom trick handling logic.
struct MultiTrickState {
    // True if a player has performed a trick and is owed a boost.
    // This is necessary because we manually clear the game's trick flag.
    bool awaitingLandingBoost[12];

    // Stores the previous frame's airtime for each player to detect the moment of landing.
    u32 prevAirtime[12];
};
static MultiTrickState trickState;

// This hook runs once at the beginning of a race to initialize our state.
static void InitMultiTrickState() {
    for (int i = 0; i < 12; ++i) {
        trickState.awaitingLandingBoost[i] = false;
        trickState.prevAirtime[i] = 0;
    }
}
static RaceLoadHook InitStateHook(InitMultiTrickState);

// This frame hook manages the trick state while airborne and upon landing.
static void ManageMultipleTricks() {
    Racedata* racedata = Racedata::sInstance;
    Kart::Manager* kartManager = Kart::Manager::sInstance;
    if (!racedata || !kartManager) return;

    const u8 playerCount = racedata->racesScenario.playerCount;
    for (u8 i = 0; i < playerCount; ++i) {
        Kart::Player* player = kartManager->GetKartPlayer(i);
        if (!player) continue;

        Kart::Status* status = player->pointers.kartStatus;
        Kart::Movement* movement = player->pointers.kartMovement;
        if (!status || !movement) continue;

        Kart::Trick* trick = movement->trick;
        if (!trick) continue;
        
        bool isInTrickState = (status->bitfield1 & 0x40) != 0;

        // Part A: Handle airborne logic
        if (status->airtime > 0) {
            // Check if the player is in the post-trick "limbo" state (trick done, but flag still set).
            if (isInTrickState && trick->cooldown <= 0) {
                // Manually clear the game's trick flag to allow another trick.
                status->bitfield1 &= ~0x40;
                
                // Since we cleared the flag, the game won't give a boost. We must do it.
                trickState.awaitingLandingBoost[i] = true;
                
                // Re-evaluate the flag now that we've cleared it.
                isInTrickState = false;
            }

            // If the player is in a neutral airborne state (not in a trick, cooldown is over),
            // make them trickable. This allows the first trick and any subsequent tricks.
            if (!isInTrickState && trick->cooldown <= 0 && status->trickableTimer <= 0) {
                status->trickableTimer = 2;
            }
        }
        // Part B: Handle landing logic
        else if (status->airtime == 0 && trickState.prevAirtime[i] > 0) {
            // The player has just landed (airtime was > 0 last frame, is 0 now).
            // Give them a boost if our flag is set. The vanilla boost from the last trick is also
            // given here, but since we manage the state, we ensure only one boost is applied.
            if (trickState.awaitingLandingBoost[i]) {
                movement->boost.Activate(4, 30); // Standard trick boost (type 4, 30 frames).
                trickState.awaitingLandingBoost[i] = false;
            }
        }
        
        // Store the current airtime to detect landing on the next frame.
        trickState.prevAirtime[i] = status->airtime;
    }
}

static RaceFrameHook MultipleTricksHook(ManageMultipleTricks);

} // namespace Race
} // namespace Pulsar