// FroomKickPage.hpp
#ifndef _FROOMKICKPAGE_HPP_
#define _FROOMKICKPAGE_HPP_

#include <kamek.hpp>
#include <UI/UI.hpp>
#include <MarioKartWii/UI/Page/Menu/Menu.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>
#include <MarioKartWii/UI/Ctrl/PushButton.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>

namespace Pulsar {
namespace UI {

class FroomKickPage : public Pages::MenuInteractable {
public:
    static const PulPageId id = PULPAGE_FROOMKICK;  // Our unique page ID

    FroomKickPage();

    // Page / MenuInteractable virtuals
    void OnInit() override;
    void BeforeEntranceAnimations() override;
    void BeforeControlUpdate() override;

    int GetActivePlayerBitfield() const override;
    int GetPlayerBitfield() const override;
    ManipulatorManager& GetManipulatorManager() override;
    UIControl* CreateExternalControl(u32 id) override;
    UIControl* CreateControl(u32 id) override;
    void SetButtonHandlers(PushButton& button) override;
    MiiGroup* miiGroup;

private:
    // Handlers for the “Kick” buttons
    void OnKickClick(PushButton& button, u32 hudSlotId);
    void OnKickSelect(PushButton& button, u32 hudSlotId);
    void OnKickDeselect(PushButton& button, u32 hudSlotId);

    // We override back-press to exit
    void OnBackPress(u32 hudSlotId);

    // PtmfHolders for push-button actions
    PtmfHolder_2A<FroomKickPage, void, PushButton&, u32> onKickClickHandler;
    PtmfHolder_2A<FroomKickPage, void, PushButton&, u32> onKickSelectHandler;
    PtmfHolder_2A<FroomKickPage, void, PushButton&, u32> onKickDeselectHandler;

    // Arrays for up to 12 possible external players
    LayoutUIControl miis[24];     // Mii icon (like in TeamSelect)
    PushButton kickButtons[24];   // Kick arrow/button
    u8 arrowMiiIdx[12];           // AID stored for each slot

    bool isLocked;                // If we want to lock the page from changes
};

} // namespace UI
} // namespace Pulsar

#endif
