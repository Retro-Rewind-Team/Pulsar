// FroomKickPage.cpp
#include <UI/FroomKickPage.hpp>
#include <Debug/Debug.hpp>            // For OS::Report or logging if you like
#include <core/rvl/OS/OS.hpp>
#include <PulsarSystem.hpp>           // If you have a custom System class
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <MarioKartWii/mii/MiiGroup.hpp>
#include <MarioKartWii/UI/Ctrl/Menu/CtrlMenuTeamConfirmTeam.hpp>
#include <MarioKartWii/UI/Page/Other/FriendRoom.hpp>
#include <SlotExpansion/CupsConfig.hpp> // For BMG references, if needed

namespace Pulsar {
namespace UI {

FroomKickPage::FroomKickPage() {
    OS::Report("FroomKickPage::FroomKickPage()\n");
    // This page has a back button by default
    this->hasBackButton = true;
    // Set up push-button handler pointers
    this->onKickClickHandler.subject     = this;
    this->onKickClickHandler.ptmf       = &FroomKickPage::OnKickClick;
    this->onKickSelectHandler.subject    = this;
    this->onKickSelectHandler.ptmf       = &FroomKickPage::OnKickSelect;
    this->onKickDeselectHandler.subject  = this;
    this->onKickDeselectHandler.ptmf     = &FroomKickPage::OnKickDeselect;
    this->isLocked = false;
    OS::Report("FroomKickPage::FroomKickPage() - End\n");
}

void FroomKickPage::OnInit() {
    OS::Report("FroomKickPage::OnInit()\n");
    // We want 24 controls: 12 Mii icons + 12 "Kick" buttons
    this->InitControlGroup(24);
    OS::Report("FroomKickPage::OnInit() - InitControlGroup(24) done\n");
    MenuInteractable::OnInit(); // calls default page init, etc.
    OS::Report("FroomKickPage::OnInit() - MenuInteractable::OnInit() done\n");

    // Create & add our 12 Kick buttons and 12 Mii UI controls
    for (int i = 0; i < 24; i++) {
        OS::Report("FroomKickPage::OnInit() - loop i = %d\n", i);
        // Kick button
        this->AddControl(i, this->kickButtons[i], 0);
        OS::Report("FroomKickPage::OnInit() - Added kick button at control index: %d\n", i);
        this->kickButtons[i].Load(
            UI::buttonFolder,         // e.g. "button"
            "CommonButton",              // or "FroomButton" if you prefer that style
            "Confirm",                   // variant name
            1,                        // localPlayerBitfield
            0,                        // r8 (unused?)
            false                     // start hidden?
        );
        OS::Report("FroomKickPage::OnInit() - Kick button loaded for i = %d\n", i);
        // Attach event handlers
        this->kickButtons[i].SetOnClickHandler(this->onKickClickHandler, 0);
        OS::Report("FroomKickPage::OnInit() - set OnClickHandler for i = %d\n", i);
        this->kickButtons[i].SetOnSelectHandler(this->onKickSelectHandler);
        OS::Report("FroomKickPage::OnInit() - set OnSelectHandler for i = %d\n", i);
        this->kickButtons[i].SetOnDeselectHandler(this->onKickDeselectHandler);
        OS::Report("FroomKickPage::OnInit() - set OnDeselectHandler for i = %d\n", i);
        this->kickButtons[i].buttonId = i;
        OS::Report("FroomKickPage::OnInit() - buttonId set to %d for i = %d\n", i, i);

        this->miiGroup = &SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>()->miiGroup;
        OS::Report("FroomKickPage::OnInit() - miiGroup pointer obtained for i = %d\n", i);
        
        // Mii icon
        this->AddControl(i + 24, this->miis[i], 0);
        OS::Report("FroomKickPage::OnInit() - added Mii control at index: %d\n", i + 12);
        ControlLoader loader(&this->miis[i]);
        OS::Report("FroomKickPage::OnInit() - ControlLoader constructed for i = %d\n", i);
        // Reuse a BRCTR you have or a generic Mii-suitable one. For example:
        loader.Load("control", "TeamMii", "TeamMii", nullptr); // Reusing TeamMii BRCTR directly
        OS::Report("FroomKickPage::OnInit() - Mii control loaded for i = %d\n", i);
        // The above line depends on you having "TeamMii" in "control" folder
        // (like how TeamSelect uses Mii).
        
        // We'll initially hide them all; we'll show them in BeforeControlUpdate
        this->kickButtons[i].isHidden = true;
        OS::Report("FroomKickPage::OnInit() - Kick button hidden for i = %d\n", i);
        this->miis[i].isHidden        = true;
        OS::Report("FroomKickPage::OnInit() - Mii hidden for i = %d\n", i);
    }
    OS::Report("FroomKickPage::OnInit() - End\n");
}

void FroomKickPage::BeforeEntranceAnimations() {
    OS::Report("FroomKickPage::BeforeEntranceAnimations()\n");
    MenuInteractable::BeforeEntranceAnimations();
    OS::Report("FroomKickPage::BeforeEntranceAnimations() - MenuInteractable::BeforeEntranceAnimations() done\n");
    // If we have any special logic upon page entrance
    // e.g. isLocked = false;
    OS::Report("FroomKickPage::BeforeEntranceAnimations() - End\n");
}

void FroomKickPage::BeforeControlUpdate() {
    OS::Report("FroomKickPage::BeforeControlUpdate()\n");
    MenuInteractable::BeforeControlUpdate();
    OS::Report("FroomKickPage::BeforeControlUpdate() - MenuInteractable::BeforeControlUpdate() done\n");

    // We'll read the friend room sub, see who is in the room, and show Mii + Kick for them
    RKNet::ControllerSub& sub = RKNet::Controller::sInstance->subs[RKNet::Controller::sInstance->currentSub];
    OS::Report("FroomKickPage::BeforeControlUpdate() - RKNet::ControllerSub obtained\n");
    bool isHost = (sub.localAid == sub.hostAid);
    OS::Report("FroomKickPage::BeforeControlUpdate() - isHost: %d (sub.localAid: %d, sub.hostAid: %d)\n", isHost, sub.localAid, sub.hostAid);

    // If for some reason we're not host, we can just close
    if (!isHost) {
        OS::Report("FroomKickPage::BeforeControlUpdate() - Not host, closing page.\n");
        // This page is only for the host; just exit if we are not
        this->EndStateAnimated(0, 0.f);
        OS::Report("FroomKickPage::BeforeControlUpdate() - End - Not host.\n");
        return;
    }

    // Show or hide up to 12 players
    // We'll keep an index for how many we displayed so far
    int displayCount = 0;
    OS::Report("FroomKickPage::BeforeControlUpdate() - displayCount initialized to 0\n");
    for (int aid = 0; aid < 12; ++aid) {
        OS::Report("FroomKickPage::BeforeControlUpdate() - loop aid = %d\n", aid);
        // Check if this AID is in the friend room
        bool isInLobby = (sub.availableAids & (1 << aid)) != 0;
        OS::Report("FroomKickPage::BeforeControlUpdate() - isInLobby for aid %d: %d\n", aid, isInLobby);
        // Skip if not in the lobby, or if it's the host itself
        if (!isInLobby || aid == sub.hostAid) {
            OS::Report("FroomKickPage::BeforeControlUpdate() - skipping aid %d (not in lobby or is host)\n", aid);
             continue;
        }

        // Display the Mii
        // If you maintain a global MiiGroup or if you have the friend room's MiiGroup:
        if (this->miiGroup != nullptr) { // Use this->miiGroup here
             OS::Report("FroomKickPage::BeforeControlUpdate() - miiGroup not null, displaying Mii for aid %d, displayCount %d\n", aid, displayCount);
                this->miis[displayCount].SetMiiPane("chara",
                    *this->miiGroup, aid * 2, 2); // Use this->miiGroup here
                OS::Report("FroomKickPage::BeforeControlUpdate() - Mii chara pane set for aid %d, displayCount %d\n", aid, displayCount);
                this->miis[displayCount].SetMiiPane("chara_shadow",
                    *this->miiGroup, aid * 2, 2); // Use this->miiGroup here
                OS::Report("FroomKickPage::BeforeControlUpdate() - Mii chara_shadow pane set for aid %d, displayCount %d\n", aid, displayCount);
            }
        this->miis[displayCount].isHidden        = false;
        OS::Report("FroomKickPage::BeforeControlUpdate() - Mii is now visible for displayCount: %d\n", displayCount);
        this->kickButtons[displayCount].isHidden = false;
        OS::Report("FroomKickPage::BeforeControlUpdate() - Kick button is now visible for displayCount: %d\n", displayCount);
        this->kickButtons[displayCount].manipulator.inaccessible = false;
        OS::Report("FroomKickPage::BeforeControlUpdate() - Kick button is now accessible for displayCount: %d\n", displayCount);


        // Store the AID in arrowMiiIdx for reference
        arrowMiiIdx[displayCount] = static_cast<u8>(aid);
        OS::Report("FroomKickPage::BeforeControlUpdate() - stored aid %d in arrowMiiIdx[%d]\n", aid, displayCount);
        displayCount++;
        OS::Report("FroomKickPage::BeforeControlUpdate() - displayCount incremented to %d\n", displayCount);
    }

    // Hide the rest
    for (int i = displayCount; i < 24; i++) {
        OS::Report("FroomKickPage::BeforeControlUpdate() - Hiding row %d, displayCount is %d\n", i, displayCount);
        this->miis[i].isHidden = true;
        OS::Report("FroomKickPage::BeforeControlUpdate() - Hidden Mii at row: %d\n", i);
        this->kickButtons[i].isHidden = true;
         OS::Report("FroomKickPage::BeforeControlUpdate() - Hidden Kick Button at row: %d\n", i);
        this->kickButtons[i].manipulator.inaccessible = true;
         OS::Report("FroomKickPage::BeforeControlUpdate() - Kick button made inaccessible at row: %d\n", i);
    }
     OS::Report("FroomKickPage::BeforeControlUpdate() - End\n");
}

int FroomKickPage::GetActivePlayerBitfield() const {
    OS::Report("FroomKickPage::GetActivePlayerBitfield()\n");
    // Typically 1 => single local HUD slot
    OS::Report("FroomKickPage::GetActivePlayerBitfield() - return 1\n");
    return 1;
}

int FroomKickPage::GetPlayerBitfield() const {
    OS::Report("FroomKickPage::GetPlayerBitfield()\n");
    // Also usually 1 => "player 0"
    OS::Report("FroomKickPage::GetPlayerBitfield() - return 1\n");
    return 1;
}

ManipulatorManager& FroomKickPage::GetManipulatorManager() {
    OS::Report("FroomKickPage::GetManipulatorManager()\n");
    // Return the manager from our base class
    OS::Report("FroomKickPage::GetManipulatorManager() - Returning controlsManipulatorManager\n");
    return this->controlsManipulatorManager;
}

UIControl* FroomKickPage::CreateExternalControl(u32 id) {
    OS::Report("FroomKickPage::CreateExternalControl(id: %u)\n", id);
    // We aren't creating any external controls by ID in this page,
    // so just return nullptr
    OS::Report("FroomKickPage::CreateExternalControl() - return nullptr\n");
    return nullptr;
}

UIControl* FroomKickPage::CreateControl(u32 id) {
     OS::Report("FroomKickPage::CreateControl(id: %u)\n", id);
    // Similarly, we add all controls manually in OnInit,
    // so we do not handle them here
     OS::Report("FroomKickPage::CreateControl() - return nullptr\n");
    return nullptr;
}

void FroomKickPage::SetButtonHandlers(PushButton& button) {
    OS::Report("FroomKickPage::SetButtonHandlers()\n");
    // Not used; we attached handlers in OnInit directly
    OS::Report("FroomKickPage::SetButtonHandlers() - End\n");
}

//---------------------------------
//  Button Handlers
//---------------------------------

void FroomKickPage::OnKickClick(PushButton& button, u32 hudSlotId) {
     OS::Report("FroomKickPage::OnKickClick(button.buttonId: %d, hudSlotId: %u)\n", button.buttonId, hudSlotId);
    // The user pressed "Kick" on a certain row
    s32 idx = button.buttonId;
    OS::Report("FroomKickPage::OnKickClick - idx: %d\n", idx);
    if (idx < 0 || idx >= 12) {
        OS::Report("FroomKickPage::OnKickClick - idx out of bounds, return.\n");
        return;
    }

    u8 aid = arrowMiiIdx[idx];
    OS::Report("FroomKickPage::OnKickClick => Attempting to remove AID %d\n", aid);

    // Forcibly remove them from sub
    RKNet::Controller* netController = RKNet::Controller::sInstance;
    OS::Report("FroomKickPage::OnKickClick - netController pointer obtained\n");
    RKNet::ControllerSub& sub = netController->subs[netController->currentSub];
    OS::Report("FroomKickPage::OnKickClick - sub pointer obtained\n");

    // Clear them from the friend room
    sub.availableAids &= ~(1 << aid);
    OS::Report("FroomKickPage::OnKickClick - availableAids updated for aid: %d\n", aid);
    sub.connectionUserDatas[aid].playersAtConsole = 0;
    OS::Report("FroomKickPage::OnKickClick - connectionUserDatas updated for aid: %d\n", aid);


    // We can forcibly call e.g. netController->ProcessPlayerDisconnect(aid)
    // That will cause them to see a disconnection
    netController->ProcessPlayerDisconnect(aid);
    OS::Report("FroomKickPage::OnKickClick - ProcessPlayerDisconnect called for aid: %d\n", aid);


    // Optionally hide the row
    this->kickButtons[idx].isHidden = true;
    OS::Report("FroomKickPage::OnKickClick - Kick button hidden at row: %d\n", idx);
    this->miis[idx].isHidden        = true;
    OS::Report("FroomKickPage::OnKickClick - Mii hidden at row: %d\n", idx);
    OS::Report("FroomKickPage::OnKickClick - End\n");
}

void FroomKickPage::OnKickSelect(PushButton& button, u32 hudSlotId) {
    OS::Report("FroomKickPage::OnKickSelect(button.buttonId: %d, hudSlotId: %u)\n", button.buttonId, hudSlotId);
    // If you want to show some bottom text
    this->bottomText->SetMessage(BMG_ROOM_DENY);
    OS::Report("FroomKickPage::OnKickSelect - Bottom text set to BMG_ROOM_DENY\n");
     OS::Report("FroomKickPage::OnKickSelect - End\n");
}

void FroomKickPage::OnKickDeselect(PushButton& button, u32 hudSlotId) {
     OS::Report("FroomKickPage::OnKickDeselect(button.buttonId: %d, hudSlotId: %u)\n", button.buttonId, hudSlotId);
    // Potentially do nothing
     OS::Report("FroomKickPage::OnKickDeselect - End\n");
}

void FroomKickPage::OnBackPress(u32 hudSlotId) {
    OS::Report("FroomKickPage::OnBackPress(hudSlotId: %u)\n", hudSlotId);
    // If user hits B or Back, leave the page
    // This calls EndStateAnimated with some direction, 0 or 1, your choice
    this->EndStateAnimated(0, 0.f);
    OS::Report("FroomKickPage::OnBackPress - EndStateAnimated called\n");
    OS::Report("FroomKickPage::OnBackPress - End\n");
}

} // namespace UI
} // namespace Pulsar