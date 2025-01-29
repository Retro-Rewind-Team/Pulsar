#include <Gamemodes/KO/KOMgr.hpp> // or any other includes you may need
#include <Network/PacketExpansion.hpp>
#include <Network/Network.hpp>
#include <UI/FroomKickPage.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <MarioKartWii/Race/Raceinfo/Raceinfo.hpp>
#include <MarioKartWii/UI/Page/Other/FriendRoom.hpp>
#include <core/rvl/OS/OS.hpp>

#include <UI/UI.hpp>

namespace Pulsar {
namespace UI {

FroomKickPage::FroomKickPage() {
    hasBackButton      = true;
    externControlCount = 0;   // No “external” controls
    internControlCount = 25;
    extraControlNumber = 0;
    controlCount = 0; // Ensure controlCount starts correctly
    controlSources     = 2;   // Typically 2 means "local + net" or so
    titleBmg           = BMG_TEXT;   // or any BMG you like
    nextPageId         = PAGE_FRIEND_ROOM;
    prevPageId         = PAGE_FRIEND_ROOM;
    nextSection        = SECTION_NONE;
    movieStartFrame    = -1;
    isLocked           = false;
    activePlayerBitfield = 1; // Typically 1 means “player 1 only,” etc.

    // If you want the “Start” or “Back” global handlers, set up manipulator manager
    controlsManipulatorManager.Init(1, false);
    SetManipulatorManager(controlsManipulatorManager);

    // e.g. set global Start/Back press if you want them
    // controlsManipulatorManager.SetGlobalHandler(START_PRESS, onStartPressHandler, false, false);
    controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, onBackPressHandler, false, false);

    // PTMF handler objects
    onButtonClickHandler.subject    = this;
    onButtonClickHandler.ptmf       = &FroomKickPage::OnKickClick;

    onButtonSelectHandler.subject   = this;
    onButtonSelectHandler.ptmf      = &FroomKickPage::OnKickSelect;

    onButtonDeselectHandler.subject = this;
    onButtonDeselectHandler.ptmf    = &FroomKickPage::OnKickDeselect;

    onBackPressHandler.subject      = this;
    onBackPressHandler.ptmf         = &FroomKickPage::OnBackPress;

    // The array arrowMiiIdx is used to track who is at each button index
    for (int i = 0; i < 12; i++) {
        arrowMiiIdx[i] = 0xFF;
    }
}

void FroomKickPage::OnInit() {
    OS::Report("FroomKickPage::OnInit() - Entry\n");

    // This sets up an internal array of size = internControlCount = 25
    // and calls FroomKickPage::CreateControl(i) for each i in [0..24].
    // this->InitControlGroup(internControlCount);  
    Pages::MenuInteractable::OnInit();

    // Now do any additional post‐loading logic, e.g. MiiGroup
    // Pages::FriendRoomManager *frm = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    // if (frm) {
    //     this->miiGroup = &frm->miiGroup;
    // } else {
    //     this->miiGroup = nullptr;
    // }

    OS::Report("FroomKickPage::OnInit() - Exit\n");
}

void FroomKickPage::BeforeEntranceAnimations() {
    Pages::MenuInteractable::BeforeEntranceAnimations();
    OS::Report("FroomKickPage::BeforeEntranceAnimations()\n");

    // Clear out any old animations or placeholders
    this->isLocked = false;

    // Hide everything to start
    for (int i = 0; i < 12; i++) {
        kickButtons[i].isHidden = true;
        kickButtons[i].manipulator.inaccessible = true;
        miis[i].isHidden = true;
    }
}

void FroomKickPage::BeforeControlUpdate() {
    OS::Report("FroomKickPage::BeforeControlUpdate() - Entry\n");

    Pages::FriendRoomManager *frmPtr = static_cast<Pages::FriendRoomManager*>(SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>());


    // Fetch the RKNet::Controller instance
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    OS::Report("FroomKickPage::BeforeControlUpdate() - RKNet::Controller instance: %p\n", controller);

    // Update miiGroup each time to ensure it's current
    

    if (!controller) {
        OS::Report("FroomKickPage::BeforeControlUpdate() - Controller is NULL, exiting early!\n");
        return;
    }

    // Validate currentSub index
    if (controller->currentSub >= 2) { // i think because its either 0 or 1
        OS::Report("FroomKickPage::BeforeControlUpdate() - Invalid currentSub index: %d (MAX_SUBS: %d)\n", controller->currentSub, 2);
        return;
    }

    RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    OS::Report("FroomKickPage::BeforeControlUpdate() - ControllerSub details: currentSub=%d, localAid=%d, hostAid=%d, availableAids=0x%X\n",
              controller->currentSub, sub.localAid, sub.hostAid, sub.availableAids);

    // Check if connectionUserDatas is valid
    if (!sub.connectionUserDatas) {
        OS::Report("FroomKickPage::BeforeControlUpdate() - connectionUserDatas is NULL!\n");
        return;
    }

    // Iterate over all possible AIDs
    int idx = 0;
    for (int aid = 0; aid < 12; aid++) {
        OS::Report("FroomKickPage::BeforeControlUpdate() - Checking AID %d\n", aid);
        if (sub.availableAids & (1 << aid)) {
            OS::Report("FroomKickPage::BeforeControlUpdate() - AID %d is available\n", aid);

            // Validate aid index
            if (aid >= 12) { // Define MAX_AIDS appropriately (typically 12)
                OS::Report("FroomKickPage::BeforeControlUpdate() - AID %d is out of bounds (MAX_AIDS: %d)\n", aid, 12);
                continue;
            }

            // Fetch playersAtConsole
            u8 playersAtConsole = sub.connectionUserDatas[aid].playersAtConsole;
            OS::Report("FroomKickPage::BeforeControlUpdate() - AID %d has %d players at console\n", aid, playersAtConsole);

            // Validate playersAtConsole
            if (playersAtConsole > 2) { // Define MAX_PLAYERS_PER_AID appropriately
                OS::Report("FroomKickPage::BeforeControlUpdate() - AID %d has invalid playersAtConsole: %d (MAX_PLAYERS_PER_AID: %d)\n",
                          aid, playersAtConsole, 2);
                playersAtConsole = 2; // Clamp to maximum
            }

            for(int player = 0; player < playersAtConsole; player++) {
                OS::Report("FroomKickPage::BeforeControlUpdate() - Processing player %d for AID %d (idx=%d)\n", player, aid, idx);
                if (idx < 12) {
                    // Show Mii
                    miis[idx].isHidden = false;
                    OS::Report("FroomKickPage::BeforeControlUpdate() - miis[%d].isHidden set to false\n", idx);

                    // Validate miiGroup
                    if (miiGroup) {
                        OS::Report("FroomKickPage::BeforeControlUpdate() - miiGroup is valid\n");

                        // Validate arrowMiiIdx array
                        if (idx >= sizeof(arrowMiiIdx)/sizeof(arrowMiiIdx[0])) {
                            OS::Report("FroomKickPage::BeforeControlUpdate() - idx %d is out of bounds for arrowMiiIdx array\n", idx);
                        } else {
                            // Validate aid*2 + player
                            u32 miiIdx = aid * 2 + player;
                            if (miiIdx >= miiGroup->miiCount) { // Assuming GetMiiCount() exists
                                OS::Report("FroomKickPage::BeforeControlUpdate() - miiIdx %d is out of range for miiGroup (MiiCount: %d)\n", miiIdx, miiGroup->miiCount);
                            } else {
                                miis[idx].SetMiiPane("chara",        *miiGroup, miiIdx, 2);
                                miis[idx].SetMiiPane("chara_shadow", *miiGroup, miiIdx, 2);
                                OS::Report("FroomKickPage::BeforeControlUpdate() - Set MiiPane for miis[%d] with miiIdx=%d\n", idx, miiIdx);
                            }
                            arrowMiiIdx[idx] = miiIdx;
                            OS::Report("FroomKickPage::BeforeControlUpdate() - arrowMiiIdx[%d] set to %d\n", idx, arrowMiiIdx[idx]);
                        }
                    } else {
                        OS::Report("FroomKickPage::BeforeControlUpdate() - miiGroup is NULL, skipping SetMiiPane\n");
                    }

                    // Show Kick Button
                    kickButtons[idx].isHidden = false;
                    kickButtons[idx].manipulator.inaccessible = false;
                    OS::Report("FroomKickPage::BeforeControlUpdate() - kickButtons[%d].isHidden set to false and accessible\n", idx);

                    // Don’t let host or local AIDs kick themselves
                    if (aid == sub.hostAid || aid == sub.localAid) {
                        kickButtons[idx].manipulator.inaccessible = true;
                        OS::Report("FroomKickPage::BeforeControlUpdate() - kickButtons[%d] made inaccessible (aid=%d is host or local)\n", idx, aid);
                    }
                    idx++;
                } else {
                    OS::Report("FroomKickPage::BeforeControlUpdate() - idx %d >= 12, breaking inner loop\n", idx);
                    break; // Prevent out-of-bounds access
                }
            }
        } else {
            OS::Report("FroomKickPage::BeforeControlUpdate() - AID %d is NOT available\n", aid);
        }
    }

    // Hide the leftover slots
    OS::Report("FroomKickPage::BeforeControlUpdate() - Hiding leftover slots from idx=%d to 12\n", idx);
    for (int leftover = idx; leftover < 12; leftover++) {
        kickButtons[leftover].isHidden = true;
        kickButtons[leftover].manipulator.inaccessible = true;
        miis[leftover].isHidden = true;
        OS::Report("FroomKickPage::BeforeControlUpdate() - kickButtons[%d].isHidden=true, manipulator.inaccessible=true; miis[%d].isHidden=true\n", leftover, leftover);
    }

    OS::Report("FroomKickPage::BeforeControlUpdate() - Exit\n");
}


int FroomKickPage::GetActivePlayerBitfield() const {
    OS::Report("FroomKickPage::GetActivePlayerBitfield() - Entry/Exit\n");
    return this->activePlayerBitfield;
}

int FroomKickPage::GetPlayerBitfield() const {
    OS::Report("FroomKickPage::GetPlayerBitfield() - Entry/Exit\n");
    return this->playerBitfield;
}

ManipulatorManager& FroomKickPage::GetManipulatorManager() {
    OS::Report("FroomKickPage::GetManipulatorManager() - Entry/Exit\n");
    return this->controlsManipulatorManager;
}

UIControl* FroomKickPage::CreateExternalControl(u32 externControlId) {
    OS::Report("FroomKickPage::CreateExternalControl() - Entry, externControlId: %d, Exit (returning nullptr)\n", externControlId);
    return nullptr; // no external controls
}

UIControl* FroomKickPage::CreateControl(u32 controlId) {
    u32 idx = this->controlCount;
    this->controlCount++;

    // 0..11 => Kick Buttons
    if (controlId < 12)
    {
        AddControl(idx, kickButtons[controlId], /*zIndex=*/0);

        // // Construct variant name e.g. KickBtn_0, KickBtn_1, etc.
        char variant[64];
        snprintf(variant, 64, "KickBtn_%d", controlId);

        // (Optional) Actually load the button from your BRCTR or "template" SZS
        // or if you want to do it purely in code:
        //   loader.Load("button", "SomeKickButton", variant, animGroups);
        //
        // But if you prefer direct:
        kickButtons[controlId].Load(UI::buttonFolder, "PULiMemberConfirmButton", variant, 1, 0, false);
        kickButtons[controlId].buttonId = controlId;

        // Hook up PTMF
        kickButtons[controlId].SetOnClickHandler(onButtonClickHandler, 0);
        kickButtons[controlId].SetOnSelectHandler(onButtonSelectHandler);
        kickButtons[controlId].SetOnDeselectHandler(onButtonDeselectHandler);

        return &kickButtons[controlId];
    }
    // 12..23 => Mii Controls
    else if (controlId < 24)
    {
        u32 miiIdx = controlId - 12;
        AddControl(idx, miis[miiIdx], 0);

        // If you have a BRCTR for the Mii layout, do e.g.:
        ControlLoader loader(&miis[miiIdx]);
        char variant[64];
        snprintf(variant, 64, "Mii%d", miiIdx);
        static const char* miiAnim[5] = { "Translate", "TranslateRight", "TranslateLeft", nullptr, nullptr };
        loader.Load("control", "TeamMii", variant, miiAnim);

        return &miis[miiIdx];
    }
    // 24 => MiiName text box
    else if (controlId == 24)
    {
        AddControl(idx, name, 0);

        // Possibly load "TeamName"
        ControlLoader loader(&name);
        loader.Load("control", "TeamName", "TeamName", nullptr);

        return &name;
    }
    // fallback
    return nullptr;
}


void FroomKickPage::SetButtonHandlers(PushButton& button) {
    OS::Report("FroomKickPage::SetButtonHandlers() - Entry, buttonId: %d, Exit (doing nothing)\n", button.buttonId);
    // not used - we do per-button handlers in CreateControl (OnInit)
}

void FroomKickPage::OnKickClick(PushButton& button, u32 hudSlotId) {
    OS::Report("FroomKickPage::OnKickClick() - Entry, buttonId: %d, hudSlotId: %d\n", button.buttonId, hudSlotId);
    // Actually do the “kick”
    const int id = button.buttonId;
    const u8 aidSlot = CalcAIDFromIdx(id);
    OS::Report("FroomKickPage::OnKickClick() - CalcAIDFromIdx returned: %d\n", aidSlot);

    if(aidSlot != 0xFF) {
        KickPlayer(aidSlot);
    }
    // Typically end the page after you do the kick, or not:
    this->EndStateAnimated(0, button.GetAnimationFrameSize());
    OS::Report("FroomKickPage::OnKickClick() - Exit\n");
}

void FroomKickPage::OnKickSelect(PushButton& button, u32 hudSlotId){
    OS::Report("FroomKickPage::OnKickSelect() - Entry, buttonId: %d, hudSlotId: %d\n", button.buttonId, hudSlotId);
    // Show Mii name
    this->text.miis[0] = (this->miiGroup)
        ? this->miiGroup->GetMii(this->arrowMiiIdx[button.buttonId])
        : static_cast<Mii*>(nullptr);;
    this->name.SetMessage(BMG_MII_NAME, &this->text);
    OS::Report("FroomKickPage::OnKickSelect() - Exit\n");
}

void FroomKickPage::OnKickDeselect(PushButton& button, u32 hudSlotId){
    OS::Report("FroomKickPage::OnKickDeselect() - Entry, buttonId: %d, hudSlotId: %d\n", button.buttonId, hudSlotId);
    // optionally clear name
    this->name.ResetTextBoxMessage("mii_name");
    OS::Report("FroomKickPage::OnKickDeselect() - Exit\n");
}

void FroomKickPage::OnBackPress(u32 hudSlotId) {
    OS::Report("FroomKickPage::OnBackPress() => hudSlotId=%d\n", hudSlotId);
    // Correct: Passing u32 as expected
    this->EndStateAnimated(0, 1.0f); 
}

// Called from OnKickClick:
void FroomKickPage::KickPlayer(u8 aid) {
    OS::Report("FroomKickPage::KickPlayer() - Entry, aid: %d\n", aid);
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    OS::Report("FroomKickPage::KickPlayer() - Got RKNet::Controller: %p\n", controller);
    if(!controller) {
        OS::Report("FroomKickPage::KickPlayer() - Controller is NULL, exiting early!\n");
        return;
    }

    // Force a “deny” packet to that AID so they are disconnected
    // controller->SendRoomDeny(aid, 0x284b); // BMG_ROOM_DENY, or a new code //ERROR!

    // Use ROOMHandler to send a ROOM packet to deny the player
    RKNet::ROOMHandler* roomHandler = RKNet::ROOMHandler::sInstance;
    OS::Report("FroomKickPage::KickPlayer() - Got RKNet::ROOMHandler: %p\n", roomHandler);
    if (roomHandler) {
        RKNet::ROOMPacket packet;
        packet.messageType = 0; // RKNet::ROOMMessageType::ROOM_NORMAL_MESSAGE;
        packet.message = 0x284b; // BMG_ROOM_DENY code

        roomHandler->SetToSendPacket(aid, packet);
        OS::Report("FroomKickPage::KickPlayer() - Sent ROOMPacket to AID: %d with message 0x284b\n", aid);

        // Potentially add logic in RKNetController or ROOMHandler to process this deny packet and disconnect the AID.
        // For now, let's assume setting the packet to send is enough for your network logic to handle disconnection based on message 0x284b

    }


    // Also remove from availableAids for us (so we see them gone immediately)
    RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    sub.availableAids &= ~(1 << aid);
    sub.connectionUserDatas[aid].playersAtConsole = 0;
    OS::Report("FroomKickPage::KickPlayer() - Removed AID %d from availableAids and set playersAtConsole to 0\n", aid);
    OS::Report("FroomKickPage::KickPlayer() - Exit\n");
}

// This helps find the correct AID from the 0..11 UI index
u8 FroomKickPage::CalcAIDFromIdx(u8 idx) {
    OS::Report("FroomKickPage::CalcAIDFromIdx() - Entry, idx: %d\n", idx);
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    OS::Report("FroomKickPage::CalcAIDFromIdx() - Got RKNet::Controller: %p\n", controller);
    if (!controller) {
        OS::Report("FroomKickPage::CalcAIDFromIdx() - Controller is NULL, returning 0xFF\n");
        return 0xFF;
    }
    RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    OS::Report("FroomKickPage::CalcAIDFromIdx() - Got RKNet::ControllerSub, currentSub: %d, availableAids: %x\n", controller->currentSub, sub.availableAids);
    int found = 0;
    for (int aid = 0; aid < 12; ++aid) {
        if((sub.availableAids & (1 << aid)) != 0) {
            OS::Report("FroomKickPage::CalcAIDFromIdx() - AID %d is available\n", aid);
            for (int player=0; player < sub.connectionUserDatas[aid].playersAtConsole; ++player) {
                OS::Report("FroomKickPage::CalcAIDFromIdx() - Checking player %d for AID %d, found count: %d, target idx: %d\n", player, aid, found, idx);
                if(found == idx) {
                    OS::Report("FroomKickPage::CalcAIDFromIdx() - Found AID %d for index %d, returning %d\n", aid, idx, aid);
                    return static_cast<u8>(aid);
                }
                found++;
            }
        }
    }
    OS::Report("FroomKickPage::CalcAIDFromIdx() - No AID found for index %d, returning 0xFF\n", idx);
    return 0xFF; // invalid
}

} // namespace UI
} // namespace Pulsar