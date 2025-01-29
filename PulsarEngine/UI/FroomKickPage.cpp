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
}

void FroomKickPage::OnInit() {
    // We want 24 controls: 12 Mii icons + 12 "Kick" buttons
    this->InitControlGroup(24);
    MenuInteractable::OnInit(); // calls default page init, etc.

    // Create & add our 12 Kick buttons and 12 Mii UI controls
    for (int i = 0; i < 12; i++) {
        // Kick button
        this->AddControl(i, this->kickButtons[i], 0);
        this->kickButtons[i].Load(
            UI::buttonFolder,         // e.g. "button"
            "CommonButton",              // or "FroomButton" if you prefer that style
            "Confirm",                   // variant name
            1,                        // localPlayerBitfield
            0,                        // r8 (unused?)
            false                     // start hidden?
        );
        // Attach event handlers
        this->kickButtons[i].SetOnClickHandler(this->onKickClickHandler, 0);
        this->kickButtons[i].SetOnSelectHandler(this->onKickSelectHandler);
        this->kickButtons[i].SetOnDeselectHandler(this->onKickDeselectHandler);
        this->kickButtons[i].buttonId = i;

        this->miiGroup = &SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>()->miiGroup;
        
        // Mii icon
        this->AddControl(i + 12, this->miis[i], 0);
        ControlLoader loader(&this->miis[i]);
        // Reuse a BRCTR you have or a generic Mii-suitable one. For example:
        loader.Load("control", "TeamMii", "TeamMii", nullptr); // Reusing TeamMii BRCTR directly
        // The above line depends on you having "TeamMii" in "control" folder
        // (like how TeamSelect uses Mii).
        
        // We'll initially hide them all; we'll show them in BeforeControlUpdate
        this->kickButtons[i].isHidden = true;
        this->miis[i].isHidden        = true;
    }
}

void FroomKickPage::BeforeEntranceAnimations() {
    MenuInteractable::BeforeEntranceAnimations();
    // If we have any special logic upon page entrance
    // e.g. isLocked = false;
}

void FroomKickPage::BeforeControlUpdate() {
    MenuInteractable::BeforeControlUpdate();

    // We'll read the friend room sub, see who is in the room, and show Mii + Kick for them
    RKNet::ControllerSub& sub = RKNet::Controller::sInstance->subs[RKNet::Controller::sInstance->currentSub];
    bool isHost = (sub.localAid == sub.hostAid);

    // If for some reason we're not host, we can just close
    if (!isHost) {
        // This page is only for the host; just exit if we are not
        this->EndStateAnimated(0, 0.f);
        return;
    }

    // Show or hide up to 12 players
    // We'll keep an index for how many we displayed so far
    int displayCount = 0;
    for (int aid = 0; aid < 12; ++aid) {
        // Check if this AID is in the friend room
        bool isInLobby = (sub.availableAids & (1 << aid)) != 0;
        // Skip if not in the lobby, or if it's the host itself
        if (!isInLobby || aid == sub.hostAid) continue;

        // Display the Mii
        // If you maintain a global MiiGroup or if you have the friend room's MiiGroup:
        if (this->miiGroup != nullptr) { // Use this->miiGroup here
                this->miis[displayCount].SetMiiPane("chara",
                    *this->miiGroup, aid * 2, 2); // Use this->miiGroup here
                this->miis[displayCount].SetMiiPane("chara_shadow",
                    *this->miiGroup, aid * 2, 2); // Use this->miiGroup here
            }
        this->miis[displayCount].isHidden        = false;
        this->kickButtons[displayCount].isHidden = false;
        this->kickButtons[displayCount].manipulator.inaccessible = false;

        // Store the AID in arrowMiiIdx for reference
        arrowMiiIdx[displayCount] = static_cast<u8>(aid);
        displayCount++;
    }

    // Hide the rest
    for (int i = displayCount; i < 12; i++) {
        this->miis[i].isHidden = true;
        this->kickButtons[i].isHidden = true;
        this->kickButtons[i].manipulator.inaccessible = true;
    }
}

int FroomKickPage::GetActivePlayerBitfield() const {
    // Typically 1 => single local HUD slot
    return 1;
}

int FroomKickPage::GetPlayerBitfield() const {
    // Also usually 1 => "player 0"
    return 1;
}

ManipulatorManager& FroomKickPage::GetManipulatorManager() {
    // Return the manager from our base class
    return this->controlsManipulatorManager;
}

UIControl* FroomKickPage::CreateExternalControl(u32 id) {
    // We aren't creating any external controls by ID in this page,
    // so just return nullptr
    return nullptr;
}

UIControl* FroomKickPage::CreateControl(u32 id) {
    // Similarly, we add all controls manually in OnInit,
    // so we do not handle them here
    return nullptr;
}

void FroomKickPage::SetButtonHandlers(PushButton& button) {
    // Not used; we attached handlers in OnInit directly
}

//---------------------------------
//  Button Handlers
//---------------------------------

void FroomKickPage::OnKickClick(PushButton& button, u32 hudSlotId) {
    // The user pressed "Kick" on a certain row
    s32 idx = button.buttonId;
    if (idx < 0 || idx >= 12) return;

    u8 aid = arrowMiiIdx[idx];
    OS::Report("FroomKickPage::OnKickClick => Attempting to remove AID %d\n", aid);

    // Forcibly remove them from sub
    RKNet::Controller* netController = RKNet::Controller::sInstance;
    RKNet::ControllerSub& sub = netController->subs[netController->currentSub];

    // Clear them from the friend room
    sub.availableAids &= ~(1 << aid);
    sub.connectionUserDatas[aid].playersAtConsole = 0;

    // We can forcibly call e.g. netController->ProcessPlayerDisconnect(aid)
    // That will cause them to see a disconnection
    netController->ProcessPlayerDisconnect(aid);

    // Optionally hide the row
    this->kickButtons[idx].isHidden = true;
    this->miis[idx].isHidden        = true;
}

void FroomKickPage::OnKickSelect(PushButton& button, u32 hudSlotId) {
    // If you want to show some bottom text
    this->bottomText->SetMessage(BMG_ROOM_DENY);
}

void FroomKickPage::OnKickDeselect(PushButton& button, u32 hudSlotId) {
    // Potentially do nothing
}

void FroomKickPage::OnBackPress(u32 hudSlotId) {
    // If user hits B or Back, leave the page
    // This calls EndStateAnimated with some direction, 0 or 1, your choice
    this->EndStateAnimated(0, 0.f);
}

} // namespace UI
} // namespace Pulsar
