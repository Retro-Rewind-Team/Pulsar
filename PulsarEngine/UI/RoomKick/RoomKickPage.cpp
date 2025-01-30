#include <UI/RoomKick/RoomKickPage.hpp>

namespace Pulsar {
namespace UI {

RoomKickPage::RoomKickPage() {
    hasBackButton = true;
    externControlCount = 0;
    internControlCount = 13;
    extraControlNumber = 0;
    controlSources = 2;
    titleBmg = BMG_KICK_BOTTOM;
    nextPageId = PAGE_NONE;
    prevPageId = PAGE_FRIEND_ROOM;
    nextSection = SECTION_NONE;
    movieStartFrame = -1;
    isLocked = false;
    activePlayerBitfield = 1;

    playerCount = 0;
    selectedIdx = -1;

    onButtonClickHandler.subject = this;
    onButtonClickHandler.ptmf = &RoomKickPage::OnButtonClick;
    onButtonSelectHandler.subject = this;
    onButtonSelectHandler.ptmf = &RoomKickPage::OnButtonSelect;
    onButtonDeselectHandler.subject = this;
    onButtonDeselectHandler.ptmf = &RoomKickPage::OnButtonDeselect;
    onBackPressHandler.subject = this;
    onBackPressHandler.ptmf = &RoomKickPage::OnBackPress;

    this->controlsManipulatorManager.Init(1, false);
    this->SetManipulatorManager(controlsManipulatorManager);
    this->controlsManipulatorManager.SetGlobalHandler(START_PRESS, onStartPressHandler, false, false);
    this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, onBackPressHandler, false, false);
    activePlayerBitfield = 1;
}

void RoomKickPage::OnInit() {
    this->miiGroup = &SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>()->miiGroup;
    Menu::OnInit();
}

void RoomKickPage::BeforeControlUpdate() {
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub* sub = &controller->subs[0];
    if(sub->connectionUserDatas[sub->localAid].playersAtConsole == 0)
        sub = &controller->subs[1];

    int idx = 0;
    for(int aid = 0; aid < 12; ++aid) {
        if(sub->availableAids & (1 << aid)) {
            for(int player = 0; player < sub->connectionUserDatas[aid].playersAtConsole; ++player) {
                // LayoutUIControl& mii = this->miis[idx];
                // mii.SetMiiPane("chara", *this->miiGroup, aid * 2 + player, 2);
                // mii.SetMiiPane("chara_shadow", *this->miiGroup, aid * 2 + player, 2);
                // mii.isHidden = false;

                this->miiIdx[idx] = aid * 2 + player;
                this->aidIdx[idx] = aid;

                LayoutUIControl& mii = this->miis[idx];

                mii.isHidden = false;
                mii.SetMiiPane("chara", *this->miiGroup, this->miiIdx[idx], 2);
                mii.SetMiiPane("chara_shadow", *this->miiGroup, this->miiIdx[idx], 2);
                mii.SetMiiPane("chara_c_down", *this->miiGroup, this->miiIdx[idx], 2);
                mii.SetMiiPane("chara_light_01", *this->miiGroup, this->miiIdx[idx], 2);
                mii.SetMiiPane("chara_light_02", *this->miiGroup, this->miiIdx[idx], 2);

                this->miis[idx].status = 3;
                mii.SetPaneVisibility("w_null", true);

                ++idx;
            }
        }
    }

    this->playerCount = idx;

    for(int remIdx = idx; remIdx < 12; ++remIdx) {
        this->miis[remIdx].isHidden = true;
    }
}

UIControl* RoomKickPage::CreateControl(u32 id) {
    const u32 count = this->controlCount;
    char variant[0x40];
    if(id < 12) {
        this->AddControl(count, this->miis[id], 0);
        snprintf(variant, 0x40, "ButtonMii%1d", id % 5);

        const char *anims[] = {
            "State", "Offline", "Online", "RandomMatching", "FriendParent", nullptr,
            nullptr,
        };

        this->miis[id].LoadWithAnims(anims, "button", "FriendListMii", variant, 1, 0);
        this->miis[id].miiGroup = this->miiGroup;
        this->miis[id].status = 3; // Friends and online
        
        this->miis[id].animator.GetAnimationGroupById(4).PlayAnimationAtFrame(1, 0.0f);
        this->miis[id].buttonId = id;
        this->miis[id].SetOnClickHandler(this->onButtonClickHandler, 0);
        this->miis[id].SetOnSelectHandler(this->onButtonSelectHandler);
        this->controlCount++;
        return &this->miis[id];
    }
    else if(id == 12) {
        this->AddControl(count, this->name, 0);
        ControlLoader loader(&this->name);
        const char* brctr = "TeamName";
        loader.Load(UI::controlFolder, brctr, brctr, nullptr);
        
        this->controlCount++;
        return &this->name;
    }

    return nullptr;
}

int RoomKickPage::GetActivePlayerBitfield() const {
    return this->activePlayerBitfield;
}

int RoomKickPage::GetPlayerBitfield() const {
    return this->playerBitfield;
}

ManipulatorManager& RoomKickPage::GetManipulatorManager() {
    return this->controlsManipulatorManager;
}

UIControl* RoomKickPage::CreateExternalControl(u32 id) {
    return nullptr;
}

void RoomKickPage::SetButtonHandlers(PushButton& button) {
    button.SetOnClickHandler(this->onButtonClickHandler, 0);
    button.SetOnSelectHandler(this->onButtonSelectHandler);
    button.SetOnDeselectHandler(this->onButtonDeselectHandler);
}

void RoomKickPage::OnBackPress(u32 hudSlotId) {
    this->EndStateAnimated(1, 0.0f);
}

void RoomKickPage::OnButtonClick(PushButton& button, u32 hudSlotId) {
    const u32 btnIdx = button.buttonId;
    if (btnIdx < this->playerCount) {
        // Pages::YesNoMessageBox* msgBox = SectionMgr::sInstance->curSection->Get<Pages::YesNoMessageBox>();

        // Text::Info info;
        // info.miis[0] = this->miiGroup->GetMii(this->miiIdx);
        // msgBox->Reset();
        // msgBox->SetMessageWindowText(BMG_KICK_CONFIRM, &info);
        // msgBox->ConfigureButton(0, BMG_YES, nullptr, 0, &this->onYesNoYesClickHandler);
        // msgBox->ConfigureButton(1, BMG_NO, nullptr, 0, nullptr);

        // msgBox->unknown_0x27c = 1; // default choice (???)

        // this->AddPageLayer(Pages::YesNoMessageBox::id, 0);
        DWC::CloseConnectionHard(this->aidIdx[this->selectedIdx]);
    }
    this->EndStateAnimated(1, 0.0f);
}

void RoomKickPage::OnButtonSelect(PushButton& button, u32 hudSlotId) {
    const u32 btnIdx = button.buttonId;
    if (btnIdx < this->playerCount) {
        Text::Info info;
        info.miis[0] = this->miiGroup->GetMii(this->miiIdx[btnIdx]);
        this->name.SetMessage(BMG_MII_NAME, &info);
        this->selectedIdx = btnIdx;
    }
}

}//namespace UI
}//namespace Pulsar