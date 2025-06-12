// MultipleTricks.cpp – Pulsar
// ----------------------------------------------------
// This implementation allows performing tricks ANY time
// while airborne, as many times as the player wants,
// by pressing the trick button.
// ----------------------------------------------------

#include <kamek.hpp>
#include <MarioKartWii/Kart/KartManager.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <MarioKartWii/Kart/KartPlayer.hpp>
#include <MarioKartWii/Kart/KartPointers.hpp>
#include <MarioKartWii/Kart/KartStatus.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <MarioKartWii/Input/InputManager.hpp>
#include <PulsarSystem.hpp>
#include <Race/multipletricks.hpp>

namespace Pulsar {
namespace Race {

/*---------------- Tunables ----------------*/
// No tunables needed for this simpler implementation

/*------------ Per-player state ------------*/
struct MultiTrickState {
    bool awaitingLandingBoost[12];
    bool lastTrickButtonState[12];
};
static MultiTrickState gState;

/*-------------- Init on race load ---------*/
static void InitMultiTrickState() {
    for (int i = 0; i < 12; ++i) {
        gState.awaitingLandingBoost[i] = false;
        gState.lastTrickButtonState[i] = false;
    }
}
static RaceLoadHook _sInitHook(InitMultiTrickState);

/*-------------- Main per-frame hook -------*/
static void ManageMultipleTricks() {
    Racedata* race = Racedata::sInstance;
    Kart::Manager* karts = Kart::Manager::sInstance;
    if (!race || !karts) return;

    const u8 playerCount = race->racesScenario.playerCount;

    for (u8 i = 0; i < playerCount; ++i) {
        Kart::Player* player = karts->GetKartPlayer(i);
        if (!player) continue;

        Kart::Status* status = player->pointers.kartStatus;
        Kart::Movement* movement = player->pointers.kartMovement;
        if (!status || !movement) continue;

        Kart::Trick* trick = movement->trick;
        if (!trick) continue;
        
        const bool inTrick = (status->bitfield1 & 0x40) != 0;
        const bool isAirborne = (status->airtime > 0);
        
        // Always enable trickable state while airborne
        if (isAirborne) {
            // Get controller and check trick button
            Input::ControllerHolder& controller = player->GetControllerHolder();
            bool trickButtonPressed = false;
            
            // Check if trick button is pressed (different for each controller type)
            ControllerType controllerType = controller.curController->GetType();
            switch (controllerType) {
                case WHEEL:
                    trickButtonPressed = (controller.inputStates[0].buttonRaw & WPAD::WPAD_BUTTON_B) != 0;
                    break;
                case NUNCHUCK:
                    trickButtonPressed = (controller.inputStates[0].buttonRaw & WPAD::WPAD_BUTTON_B) != 0;
                    break;
                case CLASSIC:
                    trickButtonPressed = (controller.inputStates[0].buttonRaw & WPAD::WPAD_CL_TRIGGER_R) != 0;
                    break;
                case GCN:
                    trickButtonPressed = (controller.inputStates[0].buttonRaw & PAD::PAD_BUTTON_R) != 0;
                    break;
                default:
                    trickButtonPressed = (controller.inputStates[0].buttonRaw & WPAD::WPAD_BUTTON_B) != 0;
                    break;
            }
            
            // Detect button press (not held)
            bool trickButtonJustPressed = trickButtonPressed && !gState.lastTrickButtonState[i];
            gState.lastTrickButtonState[i] = trickButtonPressed;
            
            // If not in trick and button is pressed, enable trickable
            if (!inTrick && trickButtonJustPressed) {
                // Set trickable timer to ensure the game recognizes we can do a trick
                status->trickableTimer = 4;
                
                // Force trick state - set "first frame of trick" and "in a trick" flags
                status->bitfield1 |= 0x20;
                
                // Mark that we'll want a boost when we land
                gState.awaitingLandingBoost[i] = true;
            }
            
            // If a trick just finished, reset state
            if (inTrick && trick->nextTimer <= 0) {
                // End the trick
                trick->End();
                
                // Clear the "in trick" flag
                status->bitfield1 &= ~0x40;
                
                // Ensure no cooldown prevents the next trick
                trick->cooldown = 0;
            }
        } else {
            // Reset button state when on ground
            gState.lastTrickButtonState[i] = false;
            
            // Check if just landed
            bool justLanded = (status->bitfield0 & 0x200) != 0;
            
            if (justLanded) {
                if (gState.awaitingLandingBoost[i]) {
                    // Apply boost when landing
                    movement->boost.Activate(4, 30);
                    gState.awaitingLandingBoost[i] = false;
                }
            }
        }
    }
}
static RaceFrameHook _sFrameHook(ManageMultipleTricks);

} // namespace Race
} // namespace Pulsar