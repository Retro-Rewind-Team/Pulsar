// MultipleTricks.cpp – Pulsar
// ----------------------------------------------------
// This version has been redesigned to be more robust.
// It constantly forces the "trickable" state while airborne,
// allowing for multiple tricks to be performed back-to-back.
// ----------------------------------------------------

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

/*---------------- Tunables ----------------*/
// No tunables needed for this simpler implementation

/*------------ Per-player state ------------*/
struct MultiTrickState {
    bool awaitingLandingBoost[12];
};
static MultiTrickState gState;

/*-------------- Init on race load ---------*/
static void InitMultiTrickState() {
    for (int i = 0; i < 12; ++i) {
        gState.awaitingLandingBoost[i] = false;
    }
}
static RaceLoadHook _sInitHook(InitMultiTrickState);

/*-------------- Main per-frame hook -------*/
static void ManageMultipleTricks() {

    Racedata* race  = Racedata::sInstance;
    Kart::Manager* karts = Kart::Manager::sInstance;
    if (!race || !karts) return;

    const u8 playerCount = race->racesScenario.playerCount;

    for (u8 i = 0; i < playerCount; ++i) {

        Kart::Player* player = karts->GetKartPlayer(i);
        if (!player) continue;

        Kart::Status*   status   = player->pointers.kartStatus;
        Kart::Movement* movement = player->pointers.kartMovement;
        if (!status || !movement) continue;

        Kart::Trick* trick = movement->trick;
        if (!trick) continue;

        const bool inTrick = (status->bitfield1 & 0x40) != 0;

        /*============  WHILE AIRBORNE  ============*/
        if (status->airtime > 0) {
            
            // If we are airborne but NOT currently in a trick animation,
            // we will FORCE the game to allow a trick on every single frame.
            // This is the key to allowing tricks at any time.
            if (!inTrick) {
                status->trickableTimer = 2;
            }

            // A trick animation has just finished mid-air.
            if (inTrick && trick->nextTimer <= 0) {
                // 1. Clean up the state of the just-finished trick (e.g. reset kart rotation).
                // We use the lower-level trick->End() to avoid vanilla's landing logic.
                trick->End();

                // 2. Clear the "in trick" flag. On the very next frame, the `!inTrick`
                // block above will execute, re-enabling `trickableTimer` immediately.
                status->bitfield1 &= ~0x40;

                // 3. Ensure no cooldown prevents the next trick.
                trick->cooldown = 0;

                // 4. Flag that a boost should be given when the kart lands. This will
                // accumulate for every trick performed in the air.
                gState.awaitingLandingBoost[i] = true;
            }
        }
        
        // This check is outside the airtime > 0 block to correctly detect landing.
        bool justLanded = (status->bitfield0 & 0x200) != 0;

        if (justLanded) {
            if (gState.awaitingLandingBoost[i]) {
                // The vanilla boost (type 4) has a length of 30 frames.
                movement->boost.Activate(4, 30);
                gState.awaitingLandingBoost[i] = false;
            }
        }
    }
}
static RaceFrameHook _sFrameHook(ManageMultipleTricks);

} // namespace Race
} // namespace Pulsar