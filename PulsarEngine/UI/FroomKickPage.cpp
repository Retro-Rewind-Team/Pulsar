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
    OS::Report("FroomKickPage::FroomKickPage() - Entry\n");
    this->hasBackButton = true;            // Provide a back button if desired
    this->externControlCount = 0;
    this->internControlCount = 25;
    this->extraControlNumber = 0;
    this->controlSources = 2;
    this->titleBmg = BMG_TEXT; // or any BMG you want for “Kick Players Title”
    this->nextPageId = PAGE_FRIEND_ROOM;   // or PAGE_FRIEND_ROOM_MESSAGES
    this->prevPageId = PAGE_FRIEND_ROOM;   // or PAGE_FRIEND_ROOM_MESSAGES
    this->nextSection = SECTION_NONE;
    this->movieStartFrame = -1;
    this->isLocked = false;

    // PTMF handlers
    this->onButtonClickHandler.subject = this;
    this->onButtonClickHandler.ptmf = &FroomKickPage::OnKickClick;

    this->onButtonSelectHandler.subject = this;
    this->onButtonSelectHandler.ptmf = &FroomKickPage::OnKickSelect;

    this->onButtonDeselectHandler.subject = this;
    this->onButtonDeselectHandler.ptmf = &FroomKickPage::OnKickDeselect;

    this->onBackPressHandler.subject = this;
    this->onBackPressHandler.ptmf = &FroomKickPage::OnBackPress;
    OS::Report("FroomKickPage::FroomKickPage() - Exit\n");
}

void FroomKickPage::OnInit() {
    OS::Report("FroomKickPage::OnInit() - Entry\n");
    // We have 12 + 12 + 1 = 25 “internal” controls
    this->InitControlGroup(this->internControlCount);
    OS::Report("FroomKickPage::OnInit() - InitControlGroup done\n");

    // Typically we get the MiiGroup from the FriendRoomManager:
    Pages::FriendRoomManager *frm = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    OS::Report("FroomKickPage::OnInit() - Got FriendRoomManager: %p\n", frm);
    if (frm != nullptr) {
        this->miiGroup = &frm->miiGroup;
        OS::Report("FroomKickPage::OnInit() - miiGroup set: %p\n", this->miiGroup);
    } else {
        // fallback in case it's not found
        this->miiGroup = nullptr;
        OS::Report("FroomKickPage::OnInit: FriendRoomManager not found!\n"); // Add error logging
    }

    for(int i = 0; i < 12; ++i) {
        char variant[64];
        snprintf(variant, 64, "KickBtn_%d", i);
        this->kickButtons[i].Load(UI::buttonFolder, "PULiMemberConfirmButton", variant, 1, 0, true);
        this->kickButtons[i].SetOnClickHandler(this->onButtonClickHandler, 0);
        this->kickButtons[i].SetOnSelectHandler(this->onButtonSelectHandler);
        this->kickButtons[i].SetOnDeselectHandler(this->onButtonDeselectHandler);

        this->kickButtons[i].buttonId = i;
        OS::Report("FroomKickPage::OnInit() - KickButton %d loaded\n", i);

        ControlLoader loader(&this->miis[i]);
        char miiVariant[64];
        snprintf(miiVariant, 64, "Mii%d", i);
        static const char* miiAnim[5] ={ "Translate", "TranslateRight", "TranslateLeft", nullptr, nullptr };
        loader.Load("control", "TeamMii", miiVariant, miiAnim);
        
        OS::Report("FroomKickPage::OnInit() - Mii %d loaded\n", i);
    }

    ControlLoader nameLoader(&this->name);
    nameLoader.Load("control", "TeamName", "TeamName", nullptr);
    OS::Report("FroomKickPage::OnInit() - Name loader done\n");

    // End with calling the parent's OnInit
    Pages::MenuInteractable::OnInit();
    OS::Report("FroomKickPage::OnInit() - Pages::MenuInteractable::OnInit() done\n");
    OS::Report("FroomKickPage::OnInit() - Exit\n");
}

void FroomKickPage::BeforeEntranceAnimations() {
    OS::Report("FroomKickPage::BeforeEntranceAnimations() - Entry\n");
    Pages::MenuInteractable::BeforeEntranceAnimations();
    OS::Report("FroomKickPage::BeforeEntranceAnimations() - Pages::MenuInteractable::BeforeEntranceAnimations() done\n");
    // Clear out any old animations or placeholders
    this->isLocked = false;

    // Hide or reset all
    for(int i = 0; i < 12; i++){
        this->kickButtons[i].isHidden = true;
        this->kickButtons[i].manipulator.inaccessible = true;
        this->miis[i].isHidden = true;
        OS::Report("FroomKickPage::BeforeEntranceAnimations() - Hiding button and mii %d\n", i);
    }
    OS::Report("FroomKickPage::BeforeEntranceAnimations() - Exit\n");
}

void FroomKickPage::BeforeControlUpdate() {
    OS::Report("FroomKickPage::BeforeControlUpdate() - Entry\n");
    // Now show each valid occupant
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    OS::Report("FroomKickPage::BeforeControlUpdate() - Got RKNet::Controller: %p\n", controller);
    if (!controller) {
        OS::Report("FroomKickPage::BeforeControlUpdate() - Controller is NULL, exiting early!\n");
        return;
    }
    RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    OS::Report("FroomKickPage::BeforeControlUpdate() - Got RKNet::ControllerSub, currentSub: %d, localAid: %d, hostAid: %d, availableAids: %x\n", controller->currentSub, sub.localAid, sub.hostAid, sub.availableAids);


    // fill from availableAids
    int idx = 0;
    for (int aid = 0; aid < 12; ++aid) {
        if (sub.availableAids & (1 << aid)) {
            u8 localCount = sub.connectionUserDatas[aid].playersAtConsole;
            OS::Report("FroomKickPage::BeforeControlUpdate() - AID %d is available, localCount: %d\n", aid, localCount);
            for(int player=0; player < localCount; ++player) {
                // fill the Mii
                if (idx < 12) {
                    OS::Report("FroomKickPage::BeforeControlUpdate() - Processing player index: %d for AID: %d, player: %d\n", idx, aid, player);
                    this->miis[idx].isHidden = false;
                    OS::Report("FroomKickPage::BeforeControlUpdate() - Mii %d visibility set to false\n", idx);
                    if(miiGroup) {
                        OS::Report("FroomKickPage::BeforeControlUpdate() - miiGroup is valid, setting MiiPane for index: %d, aid: %d, player: %d\n", idx, aid, player);
                        this->miis[idx].SetMiiPane("chara", *miiGroup, aid*2 + player, 2);
                        this->miis[idx].SetMiiPane("chara_shadow", *miiGroup, aid*2 + player, 2);
                        OS::Report("FroomKickPage::BeforeControlUpdate() - MiiPane set for index: %d\n", idx);
                    } else {
                        OS::Report("FroomKickPage::BeforeControlUpdate() - miiGroup is NULL, skipping MiiPane setting for index: %d\n", idx);
                    }
                    this->arrowMiiIdx[idx] = aid*2 + player;
                    OS::Report("FroomKickPage::BeforeControlUpdate() - arrowMiiIdx[%d] set to %d\n", idx, this->arrowMiiIdx[idx]);

                    // Kick button
                    this->kickButtons[idx].isHidden = false;
                    this->kickButtons[idx].manipulator.inaccessible = false;
                    OS::Report("FroomKickPage::BeforeControlUpdate() - KickButton %d visibility and accessibility set\n", idx);

                    // If aid == sub.hostAid, or if local (like you are trying to kick yourself), disable the button
                    if(aid == sub.hostAid) {
                        this->kickButtons[idx].manipulator.inaccessible = true; // can't kick the host
                        OS::Report("FroomKickPage::BeforeControlUpdate() - KickButton %d made inaccessible because AID is host\n", idx);
                    }

                    // Additional check if it is your own local console
                    if(aid == sub.localAid) {
                        // e.g. if that occupant is YOU or your local guest
                        this->kickButtons[idx].manipulator.inaccessible = true;
                        OS::Report("FroomKickPage::BeforeControlUpdate() - KickButton %d made inaccessible because AID is local\n", idx);
                    }
                    idx++;
                } else {
                    OS::Report("FroomKickPage::BeforeControlUpdate() - Index %d >= 12, breaking inner loop\n", idx);
                    break; // Prevent out-of-bounds access
                }
            }
        } else {
            OS::Report("FroomKickPage::BeforeControlUpdate() - AID %d is NOT available\n", aid);
        }
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
    OS::Report("FroomKickPage::CreateControl() - Entry, controlId: %d\n", controlId);
    UIControl* control = nullptr;
    if(controlId < 12) {
        control = &this->kickButtons[controlId];
        OS::Report("FroomKickPage::CreateControl() - Returning kickButtons[%d]\n", controlId);
    } else if(controlId < 24) {
        control = &this->miis[controlId - 12];
        OS::Report("FroomKickPage::CreateControl() - Returning miis[%d]\n", controlId - 12);
    } else if(controlId == 24) {
        control = &this->name;
        OS::Report("FroomKickPage::CreateControl() - Returning name\n");
    }
    OS::Report("FroomKickPage::CreateControl() - Exit\n");
    return control;
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
    OS::Report("FroomKickPage::OnBackPress() - Entry, hudSlotId: %d\n", hudSlotId);
    this->EndStateAnimated(0.0f, 1);
    OS::Report("FroomKickPage::OnBackPress() - Exit\n");
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