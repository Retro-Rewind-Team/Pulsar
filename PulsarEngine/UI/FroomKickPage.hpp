#ifndef _FROOMKICKPAGE_
#define _FROOMKICKPAGE_

#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Menu/Menu.hpp>
#include <MarioKartWii/UI/Page/Menu/MiiSelect.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/MII/MiiGroup.hpp>
#include <UI/UI.hpp>
#include <UI/ToggleControls.hpp>

namespace Pulsar {
namespace UI {

class FroomKickPage : public Pages::MenuInteractable {
public:
    static const PulPageId id = PULPAGE_FROOMKICK;

    FroomKickPage();

    // Standard Page vfunc overrides
    void OnInit() override;
    void BeforeEntranceAnimations() override;
    void BeforeControlUpdate() override;
    int GetActivePlayerBitfield() const override;
    int GetPlayerBitfield() const override;
    ManipulatorManager& GetManipulatorManager() override;
    UIControl* CreateExternalControl(u32 externControlId) override;
    UIControl* CreateControl(u32 controlId) override;
    void SetButtonHandlers(PushButton& button) override;

private:
    // PTMF Handlers for the "Kick" buttons and the "Back" press
    void OnKickClick(PushButton& button, u32 hudSlotId);
    void OnKickSelect(PushButton& button, u32 hudSlotId);
    void OnKickDeselect(PushButton& button, u32 hudSlotId);
    void OnBackPress(u32 hudSlotId);

    // Helper to find which AID/Slot is associated with the clicked index
    static u8 CalcAIDFromIdx(u8 idx);

    // Actually do the "kick"
    static void KickPlayer(u8 aid);

    // UI elements
    PushButton kickButtons[12];   // up to 12 potential players
    LayoutUIControl miis[12];     // show Mii heads
    MiiName name;                 // top “Mii name” text
    MiiGroup *miiGroup;           // from friend-room manager
    u8 arrowMiiIdx[12];           // store (aid*2+player) or 0xFF if invalid
    Text::Info text;
    bool isLocked;
};

} // namespace UI
} // namespace Pulsar

#endif
