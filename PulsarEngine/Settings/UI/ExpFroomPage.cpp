#include <MarioKartWii/UI/Page/Other/Globe.hpp>
#include <MarioKartWii/3D/GlobeMgr.hpp>
#include <PulsarSystem.hpp>
#include <Settings/UI/ExpFroomPage.hpp>
#include <UI/FroomKickPage.hpp>
#include <UI/TeamSelect/TeamSelect.hpp>
#include <UI/UI.hpp>

namespace Pulsar {
namespace UI {

kmWrite32(0x805d8260, 0x60000000); //nop initcontrolgroup
void ExpFroom::OnInit() {
    OS::Report("ExpFroom::OnInit() - Entry\n");

    this->InitControlGroup(8); //5 usually + settings button + teams button
    OS::Report("ExpFroom::OnInit() - InitControlGroup(8) called\n");
    FriendRoom::OnInit();
    OS::Report("ExpFroom::OnInit() - FriendRoom::OnInit() called\n");

    this->AddControl(5, settingsButton, 0);
    this->settingsButton.Load(UI::buttonFolder, "FroomButton", "Settings", 1, 0, false);
    this->settingsButton.buttonId = 5;
    this->settingsButton.SetOnClickHandler(this->onSettingsClickHandler, 0);
    this->settingsButton.SetOnSelectHandler(this->onButtonSelectHandler);
    this->topSettingsPage = SettingsPanel::id;
    OS::Report("ExpFroom::OnInit() - Settings button added\n");

    this->AddControl(6, teamsButton, 0);
    this->teamsButton.Load(UI::buttonFolder, "FroomButton", "Teams", 1, 0, false);
    this->teamsButton.buttonId = 6;
    this->teamsButton.SetOnClickHandler(this->onTeamsClickHandler, 0);
    this->teamsButton.SetOnSelectHandler(this->onButtonSelectHandler);
    OS::Report("ExpFroom::OnInit() - Teams button added\n");

    RKNet::ControllerSub& sub = RKNet::Controller::sInstance->subs[RKNet::Controller::sInstance->currentSub];
    bool isHost = (sub.localAid == sub.hostAid);
    OS::Report("ExpFroom::OnInit() - isHost: %d, localAid: %d, hostAid: %d\n", isHost, sub.localAid, sub.hostAid);

    this->AddControl(7, kickPlayersButton, 0); // Add the button with ID 7
    this->kickPlayersButton.Load(UI::buttonFolder, "PULiMemberConfirmButton", "KickPlayers", 1, 0, false);
    this->kickPlayersButton.buttonId = 7; // Set buttonId to 7
    this->kickPlayersButton.SetOnClickHandler(this->onKickPlayersClickHandler, 0);
    this->kickPlayersButton.SetOnSelectHandler(this->onButtonSelectHandler);
    this->kickPlayersButton.isHidden = false;
    this->kickPlayersButton.manipulator.inaccessible = !isHost;
    OS::Report("ExpFroom::OnInit() - Kick Players button added, inaccessible: %d\n", this->kickPlayersButton.manipulator.inaccessible);

    OS::Report("ExpFroom::OnInit() - Exit\n");
}

void ExpFroom::OnKickPlayersButtonClick(PushButton& button, u32 hudSlotId) {
    OS::Report("ExpFroom::OnKickPlayersButtonClick() - Entry\n");

    // Check if you are the host:
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    if (!controller) {
        OS::Report("ExpFroom::OnKickPlayersButtonClick() - ERROR: controller is NULL\n");
        return;
    }
    OS::Report("ExpFroom::OnKickPlayersButtonClick() - controller is valid: %p\n", controller);

    RKNet::ControllerSub& sub = controller->subs[controller->currentSub]; // Get ControllerSub
    OS::Report("ExpFroom::OnKickPlayersButtonClick() - isHost: %d, localAid: %d, hostAid: %d\n", (sub.localAid == sub.hostAid), sub.localAid, sub.hostAid);

    if(sub.localAid == sub.hostAid) {
        OS::Report("ExpFroom::OnKickPlayersButtonClick() - User is host, proceeding to add KickPage\n");

        // Before calling EndStateAnimated
        OS::Report("ExpFroom::OnKickPlayersButtonClick() - Calling EndStateAnimated\n");
        this->EndStateAnimated(0, button.GetAnimationFrameSize());
        OS::Report("ExpFroom::OnKickPlayersButtonClick() - EndStateAnimated called successfully\n");

        // Before calling AddPageLayer
        OS::Report("ExpFroom::OnKickPlayersButtonClick() - Calling AddPageLayer with PageId: %d\n", FroomKickPage::id);
        if (!SectionMgr::sInstance || !SectionMgr::sInstance->curSection) {
            OS::Report("ExpFroom::OnKickPlayersButtonClick() - ERROR: SectionMgr::sInstance or curSection is NULL\n");
            return;
        }
        SectionMgr::sInstance->curSection->AddPageLayer(static_cast<PageId>(FroomKickPage::id));
        OS::Report("ExpFroom::OnKickPlayersButtonClick() - AddPageLayer(FroomKickPage::id) called\n");
    } else {
        OS::Report("ExpFroom::OnKickPlayersButtonClick() - User is not host, skipping AddPageLayer\n");
    }
    OS::Report("ExpFroom::OnKickPlayersButtonClick() - Exit\n");
}

void ExpFroom::OnResume() {
    OS::Report("ExpFroom::OnResume() - Entry, areControlsHidden: %d\n", this->areControlsHidden);
    if(this->areControlsHidden) GlobeMgr::sInstance->DisplayMii();
    this->areControlsHidden = false;
    FriendRoom::OnResume();
    OS::Report("ExpFroom::OnResume() - Exit\n");
}

void ExpFroom::ExtOnButtonSelect(PushButton& button, u32 hudSlotId) {
    OS::Report("ExpFroom::ExtOnButtonSelect() - Entry, buttonId: %d\n", button.buttonId);
    if(button.buttonId == 5) {
        u32 bmgId = BMG_SETTINGS_BOTTOM + 1;
        if(this->topSettingsPage == PAGE_VS_TEAMS_VIEW) bmgId += 1;
        else if(this->topSettingsPage == PAGE_BATTLE_MODE_SELECT) bmgId += 2;
        this->bottomText.SetMessage(bmgId, 0);
        OS::Report("ExpFroom::ExtOnButtonSelect() - Settings button selected, bmgId: %d\n", bmgId);
    }
    else if(button.buttonId == 6) {
        this->bottomText.SetMessage(BMG_TEAMS_BOTTOM, 0);
        OS::Report("ExpFroom::ExtOnButtonSelect() - Teams button selected\n");
    }
    else {
        this->OnButtonSelect(button, hudSlotId);
        OS::Report("ExpFroom::ExtOnButtonSelect() - OnButtonSelect called for buttonId: %d\n", button.buttonId);
    }
    OS::Report("ExpFroom::ExtOnButtonSelect() - Exit\n");
}

void ExpFroom::OnSettingsButtonClick(PushButton& button, u32 hudSlotId) {
    OS::Report("ExpFroom::OnSettingsButtonClick() - Entry\n");
    this->areControlsHidden = true;
    ExpSection::GetSection()->GetPulPage<SettingsPanel>()->prevPageId = PAGE_FRIEND_ROOM;
    this->AddPageLayer(static_cast<PageId>(this->topSettingsPage), 0);
    OS::Report("ExpFroom::OnSettingsButtonClick() - AddPageLayer(topSettingsPage) called, topSettingsPage: %d\n", this->topSettingsPage);
    OS::Report("ExpFroom::OnSettingsButtonClick() - Exit\n");
}

void ExpFroom::OnTeamsButtonClick(PushButton& button, u32 hudSlotId) {
    OS::Report("ExpFroom::OnTeamsButtonClick() - Entry\n");
    this->areControlsHidden = true;
    this->AddPageLayer(static_cast<PageId>(PULPAGE_TEAMSELECT), 0);
    OS::Report("ExpFroom::OnTeamsButtonClick() - AddPageLayer(PULPAGE_TEAMSELECT) called\n");
    OS::Report("ExpFroom::OnTeamsButtonClick() - Exit\n");
}

void ExpFroom::AfterControlUpdate() {
    OS::Report("ExpFroom::AfterControlUpdate() - Entry, areControlsHidden: %d\n", this->areControlsHidden);
    FriendRoom::AfterControlUpdate();
    OS::Report("ExpFroom::AfterControlUpdate() - FriendRoom::AfterControlUpdate() called\n");
    const Section* section = SectionMgr::sInstance->curSection;
    Pages::Globe* globe = section->Get<Pages::Globe>();
    Pages::FriendRoomWaiting* waiting = section->Get<Pages::FriendRoomWaiting>();
    Pages::FriendRoomManager* mgr = section->Get<Pages::FriendRoomManager>();
    OS::Report("ExpFroom::AfterControlUpdate() - Got section pages\n");

    const bool hidden = this->areControlsHidden;
    this->messagesButton.isHidden = hidden;
    this->backButton.isHidden = hidden;
    this->bottomText.isHidden = hidden;
    this->settingsButton.isHidden = hidden;
    this->teamsButton.isHidden = hidden;
    globe->message.isHidden = hidden;
    globe->miiName.isHidden = hidden;
    for(FriendMatchingPlayer* player = &mgr->miiIcons[0]; player < &mgr->miiIcons[24]; player++) player->isHidden = hidden;
    mgr->titleText.isHidden = hidden;
    OS::Report("ExpFroom::AfterControlUpdate() - Basic controls visibility updated, hidden: %d\n", hidden);

    if(hidden) { //these get updated by the game too, so only need to update their isHidden when they should be forced hidden
        this->startButton.isHidden = hidden;
        this->addFriendsButton.isHidden = hidden;
        waiting->messageWindow.isHidden = hidden;
        mgr->busySymbol.isHidden = hidden;
        GlobeMgr* globeMgr = GlobeMgr::sInstance;
        globeMgr->earthmodel->isMiiShown = false;
        globeMgr->ResetGlobeMii();
        OS::Report("ExpFroom::AfterControlUpdate() - Hidden controls updated and GlobeMii reset\n");

    }
    else { //if controls are enabled, teamsButton is only visible for hosts when >2players in room
        const RKNet::Controller* controller = RKNet::Controller::sInstance;
        const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
        bool teamHidden = true;
        if(sub.hostAid == sub.localAid && sub.playerCount >= 2) teamHidden = false;
        this->teamsButton.isHidden = teamHidden;
        this->teamsButton.manipulator.inaccessible = teamHidden;
        this->kickPlayersButton.isHidden = false; // ADD THIS LINE
        OS::Report("ExpFroom::AfterControlUpdate() - Teams button visibility updated, teamHidden: %d, kickPlayersButton visible\n");
    }
    OS::Report("ExpFroom::AfterControlUpdate() - Exit\n");
}

void ExpFroom::OnMessageBoxClick(Pages::MessageBoxTransparent* msgBoxPage) {
    OS::Report("ExpFroom::OnMessageBoxClick() - Entry\n");
    Section* section = SectionMgr::sInstance->curSection;
    section->activePages[section->layerCount - 1] = msgBoxPage;
    section->layerCount--;
    OS::Report("ExpFroom::OnMessageBoxClick() - Exit, layerCount decremented\n");
}

void FixLayerCountOnMsgBoxClick(Pages::MessageBoxTransparent* msgBoxPage) {
    OS::Report("FixLayerCountOnMsgBoxClick() - Entry\n");
    Section* section = SectionMgr::sInstance->curSection;
    if(section->layerCount == 9) {
        section->activePages[section->layerCount] = nullptr;
        section->layerCount--;
        OS::Report("FixLayerCountOnMsgBoxClick() - Layer count was 9, decremented\n");
    }
    msgBoxPage->Reset();
    OS::Report("FixLayerCountOnMsgBoxClick() - msgBoxPage->Reset() called, Exit\n");
}
kmCall(0x805d860c, FixLayerCountOnMsgBoxClick);

}//namespace UI
}//namespace Pulsar