#include <MarioKartWii/UI/Page/Other/GlobeSearch.hpp>
#include <MarioKartWii/RKSYS/RKSYSMgr.hpp>
#include <Settings/UI/ExpWFCMainPage.hpp>
#include <UI/UI.hpp>
#include <UI/PlayerCount.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace UI {
// EXPANDED WFC, keeping WW button and just hiding it in case it is ever needed...

kmWrite32(0x8064b984, 0x60000000);  // nop the InitControl call in the init func
kmWrite24(0x80899a36, 'PUL');  // 8064ba38
kmWrite24(0x80899a5B, 'PUL');  // 8064ba90

void ExpWFCMain::OnInit() {
    this->InitControlGroup(9);  // 5 controls usually + settings button + player count + retro and custom buttons
    WFCMainMenu::OnInit();
    this->AddControl(5, settingsButton, 0);

    this->settingsButton.Load(UI::buttonFolder, "Settings1P", "Settings", 1, 0, false);
    this->settingsButton.buttonId = 5;
    this->settingsButton.SetOnClickHandler(this->onSettingsClick, 0);
    this->settingsButton.SetOnSelectHandler(this->onButtonSelectHandler);

    this->AddControl(6, playerCount, 0);
    ControlLoader loader(&this->playerCount);
    loader.Load(UI::buttonFolder, "VRButton", "VRButton", nullptr);

    this->AddControl(7, retroButton, 0);
    this->retroButton.Load(UI::buttonFolder, "RetrosButton", "ButtonRetros", 1, 0, 0);
    this->retroButton.buttonId = 6;
    this->retroButton.SetMessage(BMG_RETRO_MODES);
    this->retroButton.SetOnClickHandler(this->onRetroClick, 0);
    this->retroButton.SetOnSelectHandler(this->onButtonSelectHandler);

    this->AddControl(8, customButton, 0);
    this->customButton.Load(UI::buttonFolder, "CustomsButton", "ButtonCustoms", 1, 0, 0);
    this->customButton.buttonId = 7;
    this->customButton.SetMessage(BMG_CUSTOM_MODES);
    this->customButton.SetOnClickHandler(this->onCustomClick, 0);
    this->customButton.SetOnSelectHandler(this->onButtonSelectHandler);

    this->regionalButton.manipulator.inaccessible = true;
    this->worldwideButton.manipulator.inaccessible = true;
    this->regionalButton.isHidden = true;
    this->worldwideButton.isHidden = true;

    this->topSettingsPage = SettingsPanel::id;

    // Set retro button as default selected
    this->retroButton.Select(0);

    // this->manipulatorManager.SetGlobalHandler(START_PRESS, this->onStartPress, false, false);
}

u32 Pulsar::UI::ExpWFCMain::lastClickedMainMenuButton = 6;
void ExpWFCMain::OnRetroButtonClick(PushButton& pushButton, u32 hudSlotId) {
    ExpWFCMain::lastClickedMainMenuButton = 6;  // retros
    this->OnRegionalButtonClick(pushButton, hudSlotId);
}

void ExpWFCMain::OnCustomButtonClick(PushButton& pushButton, u32 hudSlotId) {
    ExpWFCMain::lastClickedMainMenuButton = 7;  // customs
    this->OnRegionalButtonClick(pushButton, hudSlotId);
}

void ExpWFCMain::OnSettingsButtonClick(PushButton& pushButton, u32 r5) {
    ExpSection::GetSection()->GetPulPage<SettingsPanel>()->prevPageId = PAGE_WFC_MAIN;
    this->nextPageId = static_cast<PageId>(this->topSettingsPage);
    this->EndStateAnimated(0, pushButton.GetAnimationFrameSize());
}

void ExpWFCMain::ExtOnButtonSelect(PushButton& button, u32 hudSlotId) {
    if (button.buttonId == 5) {
        u32 bmgId = BMG_SETTINGS_BOTTOM + 1;
        if (this->topSettingsPage == PAGE_VS_TEAMS_VIEW)
            bmgId += 1;
        else if (this->topSettingsPage == PAGE_BATTLE_MODE_SELECT)
            bmgId += 2;
        this->bottomText.SetMessage(bmgId, 0);
    } else
        this->OnButtonSelect(button, hudSlotId);
}

void ExpWFCMain::BeforeControlUpdate() {
    WFCMainMenu::BeforeControlUpdate();

    int RR_num150cc, RR_num200cc, RR_numOTT, RR_numIR;
    int CT_num150cc, CT_num200cc, CT_numOTT, CT_numIR;
    int numRegulars;

    PlayerCount::GetNumbersRR(RR_num150cc, RR_num200cc, RR_numOTT, RR_numIR);
    PlayerCount::GetNumbersCT(CT_num150cc, CT_num200cc, CT_numOTT, CT_numIR);
    PlayerCount::GetNumbersRegular(numRegulars);

    Text::Info info;
    info.intToPass[0] = RR_numOTT + RR_num200cc + RR_num150cc + RR_numIR + CT_num150cc + CT_num200cc + CT_numOTT + CT_numIR + numRegulars;
    this->playerCount.SetTextBoxMessage("go", BMG_PLAYER_COUNT, &info);

    if (!Dolphin::IsEmulator()) {
        this->playerCount.SetPaneVisibility("capsul_null", false);
    }
}

// ExpWFCModeSel
kmWrite32(0x8064c284, 0x38800001);  // distance func

void ExpWFCModeSel::OnInit() {
    WFCModeSelect::OnInit();
    // this->manipulatorManager.SetGlobalHandler(START_PRESS, this->onStartPress, false, false);
}

u32 Pulsar::UI::ExpWFCModeSel::lastClickedButton = 0;

void ExpWFCModeSel::InitButton(ExpWFCModeSel& self) {
    self.InitControlGroup(13);

    self.AddControl(5, self.ottButton, 0);
    self.ottButton.Load(UI::buttonFolder, "XifiMenuModeSelect", "RTOTTButton", 1, 0, 0);
    self.ottButton.buttonId = ottButtonId;
    self.ottButton.SetMessage(BMG_OTT_BUTTON);
    self.ottButton.SetOnClickHandler(self.onModeButtonClickHandler, 0);
    self.ottButton.SetOnSelectHandler(self.onButtonSelectHandler);

    self.AddControl(6, self.twoHundredButton, 0);
    self.twoHundredButton.Load(UI::buttonFolder, "XifiMenuModeSelect", "RT200Button", 1, 0, 0);
    self.twoHundredButton.buttonId = twoHundredButtonId;
    self.twoHundredButton.SetMessage(BMG_200_BUTTON);
    self.twoHundredButton.SetOnClickHandler(self.onModeButtonClickHandler, 0);
    self.twoHundredButton.SetOnSelectHandler(self.onButtonSelectHandler);

    self.AddControl(7, self.ctButton, 0);
    self.ctButton.Load(UI::buttonFolder, "XifiMenuModeSelect", "CTVSButton", 1, 0, 0);
    self.ctButton.buttonId = ctButtonId;
    self.ctButton.SetMessage(BMG_VS_BUTTON);
    self.ctButton.SetOnClickHandler(self.onModeButtonClickHandler, 0);
    self.ctButton.SetOnSelectHandler(self.onButtonSelectHandler);

    self.AddControl(8, self.ottButtonCT, 0);
    self.ottButtonCT.Load(UI::buttonFolder, "XifiMenuModeSelect", "CTOTTButton", 1, 0, 0);
    self.ottButtonCT.buttonId = ottButtonIdCT;
    self.ottButtonCT.SetMessage(BMG_OTT_BUTTON);
    self.ottButtonCT.SetOnClickHandler(self.onModeButtonClickHandler, 0);
    self.ottButtonCT.SetOnSelectHandler(self.onButtonSelectHandler);

    self.AddControl(9, self.twoHundredButtonCT, 0);
    self.twoHundredButtonCT.Load(UI::buttonFolder, "XifiMenuModeSelect", "CT200Button", 1, 0, 0);
    self.twoHundredButtonCT.buttonId = twoHundredButtonIdCT;
    self.twoHundredButtonCT.SetMessage(BMG_200_BUTTON);
    self.twoHundredButtonCT.SetOnClickHandler(self.onModeButtonClickHandler, 0);
    self.twoHundredButtonCT.SetOnSelectHandler(self.onButtonSelectHandler);

    self.AddControl(11, self.itemRainButton, 0);
    self.itemRainButton.Load(UI::buttonFolder, "XifiMenuModeSelect", "RTItemRainButton", 1, 0, 0);
    self.itemRainButton.buttonId = itemRainButtonId;
    self.itemRainButton.SetMessage(BMG_ITEM_RAIN_BUTTON);
    self.itemRainButton.SetOnClickHandler(self.onModeButtonClickHandler, 0);
    self.itemRainButton.SetOnSelectHandler(self.onButtonSelectHandler);

    self.AddControl(12, self.itemRainButtonCT, 0);
    self.itemRainButtonCT.Load(UI::buttonFolder, "XifiMenuModeSelect", "CTItemRainButton", 1, 0, 0);
    self.itemRainButtonCT.buttonId = itemRainButtonIdCT;
    self.itemRainButtonCT.SetMessage(BMG_ITEM_RAIN_BUTTON);
    self.itemRainButtonCT.SetOnClickHandler(self.onModeButtonClickHandler, 0);
    self.itemRainButtonCT.SetOnSelectHandler(self.onButtonSelectHandler);

    self.AddControl(10, self.vrButton, 0);
    ControlLoader loader(&self.vrButton);
    loader.Load(UI::buttonFolder, "VRButton", "VRButton", nullptr);

    Text::Info info;
    RKSYS::Mgr* rksysMgr = RKSYS::Mgr::sInstance;
    u32 vr = 0;
    if (rksysMgr->curLicenseId >= 0) {
        RKSYS::LicenseMgr& license = rksysMgr->licenses[rksysMgr->curLicenseId];
        vr = license.vr.points;
    }
    info.intToPass[0] = vr;
    self.ottButton.SetTextBoxMessage("go", BMG_VR_RATING, &info);
    self.twoHundredButton.SetTextBoxMessage("go", BMG_VR_RATING, &info);
    self.itemRainButton.SetTextBoxMessage("go", BMG_VR_RATING, &info);
    self.ctButton.SetTextBoxMessage("go", BMG_VR_RATING, &info);
    self.ottButtonCT.SetTextBoxMessage("go", BMG_VR_RATING, &info);
    self.twoHundredButtonCT.SetTextBoxMessage("go", BMG_VR_RATING, &info);
    self.itemRainButtonCT.SetTextBoxMessage("go", BMG_VR_RATING, &info);
}
kmCall(0x8064c294, ExpWFCModeSel::InitButton);

void ExpWFCModeSel::ClearModeContexts() {
    const u32 modeContexts[] = {
        PULSAR_RETROS,
        PULSAR_CTS,
        PULSAR_MODE_OTT,
        PULSAR_200_WW,
        PULSAR_ITEMMODERAIN,
    };

    const u32 numContexts = sizeof(modeContexts) / sizeof(modeContexts[0]);
    for (u32 i = 0; i < numContexts; ++i) {
        u32 context = modeContexts[i];
        System::sInstance->context &= ~(1 << context);
    }
}

void ExpWFCModeSel::OnModeButtonClick(PushButton& modeButton, u32 hudSlotId) {
    const u32 id = modeButton.buttonId;
    ClearModeContexts();

    if (id == ottButtonId) {
        System::sInstance->netMgr.region = 0x0B;
    } else if (id == twoHundredButtonId) {
        System::sInstance->netMgr.region = 0x0C;
    } else if (id == itemRainButtonId) {
        System::sInstance->netMgr.region = 0x0D;
    } else if (id == ctButtonId) {
        System::sInstance->netMgr.region = 0x14;
    } else if (id == ottButtonIdCT) {
        System::sInstance->netMgr.region = 0x15;
    } else if (id == twoHundredButtonIdCT) {
        System::sInstance->netMgr.region = 0x16;
    } else if (id == itemRainButtonIdCT) {
        System::sInstance->netMgr.region = 0x17;
    } else {
        System::sInstance->netMgr.region = 0x0A;
    }

    // Update contexts based on the region number
    System::sInstance->UpdateContextWrapper();

    this->lastClickedButton = id;
    WFCModeSelect::OnModeButtonClick(modeButton, hudSlotId);
}

void ExpWFCModeSel::OnActivatePatch() {
    register ExpWFCModeSel* page;
    asm(mr page, r29;);
    register Pages::GlobeSearch* search;
    asm(mr search, r30;);
    const bool isHidden = search->searchType == 1 ? false : true;  // make the button visible if continental was clicked

    // Reset game mode if worldwide is selected
    if (isHidden) {
        ClearModeContexts();
        System::sInstance->netMgr.region = 0x0A;
        page->lastClickedButton = 0;  // Reset to VS button
    }

    page->vsButton.isHidden = isHidden;
    page->vsButton.manipulator.inaccessible = isHidden;
    page->ottButton.isHidden = isHidden;
    page->ottButton.manipulator.inaccessible = isHidden;
    page->twoHundredButton.isHidden = isHidden;
    page->twoHundredButton.manipulator.inaccessible = isHidden;
    page->itemRainButton.isHidden = isHidden;
    page->itemRainButton.manipulator.inaccessible = isHidden;
    page->ctButton.isHidden = isHidden;
    page->ctButton.manipulator.inaccessible = isHidden;
    page->ottButtonCT.isHidden = isHidden;
    page->ottButtonCT.manipulator.inaccessible = isHidden;
    page->twoHundredButtonCT.isHidden = isHidden;
    page->twoHundredButtonCT.manipulator.inaccessible = isHidden;
    page->itemRainButtonCT.isHidden = isHidden;
    page->itemRainButtonCT.manipulator.inaccessible = isHidden;

    page->vsButton.SetMessage(BMG_VS_BUTTON);

    if (!isHidden) {
        bool isRetrosMode = (ExpWFCMain::lastClickedMainMenuButton == 6);

        // Show retro buttons only in retros mode
        page->vsButton.isHidden = !isRetrosMode;
        page->vsButton.manipulator.inaccessible = !isRetrosMode;
        page->ottButton.isHidden = !isRetrosMode;
        page->ottButton.manipulator.inaccessible = !isRetrosMode;
        page->twoHundredButton.isHidden = !isRetrosMode;
        page->twoHundredButton.manipulator.inaccessible = !isRetrosMode;
        page->itemRainButton.isHidden = !isRetrosMode;
        page->itemRainButton.manipulator.inaccessible = !isRetrosMode;

        // Show custom buttons only in customs mode
        page->ctButton.isHidden = isRetrosMode;
        page->ctButton.manipulator.inaccessible = isRetrosMode;
        page->ottButtonCT.isHidden = isRetrosMode;
        page->ottButtonCT.manipulator.inaccessible = isRetrosMode;
        page->twoHundredButtonCT.isHidden = isRetrosMode;
        page->twoHundredButtonCT.manipulator.inaccessible = isRetrosMode;
        page->itemRainButtonCT.isHidden = isRetrosMode;
        page->itemRainButtonCT.manipulator.inaccessible = isRetrosMode;
        if (!isRetrosMode) {
            page->ctButton.Select(0);
        }
    }

    page->battleButton.isHidden = true;
    page->battleButton.manipulator.inaccessible = true;

    Text::Info info;
    RKSYS::Mgr* rksysMgr = RKSYS::Mgr::sInstance;
    u32 vr = 0;
    if (rksysMgr->curLicenseId >= 0) {
        RKSYS::LicenseMgr& license = rksysMgr->licenses[rksysMgr->curLicenseId];
        vr = license.vr.points;
    }
    info.intToPass[0] = vr;
    page->vsButton.SetTextBoxMessage("go", BMG_VR_RATING, &info);

    page->nextPage = PAGE_NONE;
    PushButton* button = &page->vsButton;
    u32 bmgId = UI::BMG_RACE_WITH11P;
    page->lastClickedButton = 0;

    // Determine which button should be selected based on current context
    if (System::sInstance->IsContext(PULSAR_MODE_OTT) && System::sInstance->IsContext(PULSAR_RETROS)) {
        page->lastClickedButton = ottButtonId;
        button = &page->ottButton;
        bmgId = UI::BMG_OTT_WW_BOTTOM;
    } else if (System::sInstance->IsContext(PULSAR_200_WW) && System::sInstance->IsContext(PULSAR_RETROS)) {
        page->lastClickedButton = twoHundredButtonId;
        button = &page->twoHundredButton;
        bmgId = UI::BMG_200_WW_BOTTOM;
    } else if (System::sInstance->IsContext(PULSAR_ITEMMODERAIN) && System::sInstance->IsContext(PULSAR_RETROS)) {
        page->lastClickedButton = itemRainButtonId;
        button = &page->itemRainButton;
        bmgId = UI::BMG_ITEM_RAIN_WW_BOTTOM;
    } else if (System::sInstance->IsContext(PULSAR_CTS) && !System::sInstance->IsContext(PULSAR_MODE_OTT) && !System::sInstance->IsContext(PULSAR_200_WW)) {
        page->lastClickedButton = ctButtonId;
        button = &page->ctButton;
        bmgId = UI::BMG_RACE_WITH11P;
    } else if (System::sInstance->IsContext(PULSAR_MODE_OTT) && System::sInstance->IsContext(PULSAR_CTS)) {
        page->lastClickedButton = ottButtonIdCT;
        button = &page->ottButtonCT;
        bmgId = UI::BMG_OTT_WW_BOTTOM;
    } else if (System::sInstance->IsContext(PULSAR_200_WW) && System::sInstance->IsContext(PULSAR_CTS)) {
        page->lastClickedButton = twoHundredButtonIdCT;
        button = &page->twoHundredButtonCT;
        bmgId = UI::BMG_200_WW_BOTTOM;
    } else if (System::sInstance->IsContext(PULSAR_ITEMMODERAIN) && System::sInstance->IsContext(PULSAR_CTS)) {
        page->lastClickedButton = itemRainButtonIdCT;
        button = &page->itemRainButtonCT;
        bmgId = UI::BMG_ITEM_RAIN_WW_BOTTOM;
    } else if (page->lastClickedButton == 2) {
        button = &page->battleButton;
        bmgId = UI::BMG_BATTLE_WITH6P;
    }

    page->bottomText.SetMessage(bmgId);
    button->SelectInitial(0);
}
kmCall(0x8064c5f0, ExpWFCModeSel::OnActivatePatch);

void ExpWFCModeSel::OnModeButtonSelect(PushButton& modeButton, u32 hudSlotId) {
    if (modeButton.buttonId == ottButtonId) {
        this->bottomText.SetMessage(BMG_OTT_WW_BOTTOM);
    } else if (modeButton.buttonId == twoHundredButtonId) {
        this->bottomText.SetMessage(BMG_200_WW_BOTTOM);
    } else if (modeButton.buttonId == itemRainButtonId) {
        this->bottomText.SetMessage(BMG_ITEM_RAIN_WW_BOTTOM);
    } else if (modeButton.buttonId == ctButtonId) {
        this->bottomText.SetMessage(BMG_RACE_WITH11P);
    } else if (modeButton.buttonId == ottButtonIdCT) {
        this->bottomText.SetMessage(BMG_OTT_WW_BOTTOM);
    } else if (modeButton.buttonId == twoHundredButtonIdCT) {
        this->bottomText.SetMessage(BMG_200_WW_BOTTOM);
    } else if (modeButton.buttonId == itemRainButtonIdCT) {
        this->bottomText.SetMessage(BMG_ITEM_RAIN_WW_BOTTOM);
    }

    else
        WFCModeSelect::OnModeButtonSelect(modeButton, hudSlotId);
}

void ExpWFCModeSel::BeforeControlUpdate() {
    WFCModeSelect::BeforeControlUpdate();

    int numRegulars;
    int RR_num150cc, RR_num200cc, RR_numOTT, RR_numIR;
    int CT_num150cc, CT_num200cc, CT_numOTT, CT_numIR;
    PlayerCount::GetNumbersRR(RR_num150cc, RR_num200cc, RR_numOTT, RR_numIR);
    PlayerCount::GetNumbersCT(CT_num150cc, CT_num200cc, CT_numOTT, CT_numIR);
    PlayerCount::GetNumbersRegular(numRegulars);

    Pages::GlobeSearch* globeSearch = SectionMgr::sInstance->curSection->Get<Pages::GlobeSearch>();

    Text::Info info;
    if (globeSearch->searchType == 1) {
        info.intToPass[0] = RR_numIR;
        this->itemRainButton.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);

        info.intToPass[0] = RR_numOTT;
        this->ottButton.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);

        info.intToPass[0] = RR_num200cc;
        this->twoHundredButton.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);

        info.intToPass[0] = RR_num150cc;
        this->vsButton.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);

        info.intToPass[0] = CT_numIR;
        this->itemRainButtonCT.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);

        info.intToPass[0] = CT_numOTT;
        this->ottButtonCT.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);

        info.intToPass[0] = CT_num200cc;
        this->twoHundredButtonCT.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);

        info.intToPass[0] = CT_num150cc;
        this->ctButton.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);
    } else {
        info.intToPass[0] = numRegulars;
        this->vsButton.SetTextBoxMessage("go", Pulsar::UI::BMG_PLAYER_COUNT, &info);
    }

    RKSYS::Mgr* rksysMgr = RKSYS::Mgr::sInstance;
    u32 vr = 0;
    if (rksysMgr->curLicenseId >= 0) {
        RKSYS::LicenseMgr& license = rksysMgr->licenses[rksysMgr->curLicenseId];
        vr = license.vr.points;
    }

    info.intToPass[0] = vr;
    this->vrButton.SetTextBoxMessage("go", Pulsar::UI::BMG_VR_RATING, &info);

    if (!Dolphin::IsEmulator()) {
        this->vsButton.SetPaneVisibility("capsul_null", false);
        this->ottButton.SetPaneVisibility("capsul_null", false);
        this->twoHundredButton.SetPaneVisibility("capsul_null", false);
        this->ctButton.SetPaneVisibility("capsul_null", false);
        this->ottButtonCT.SetPaneVisibility("capsul_null", false);
        this->twoHundredButtonCT.SetPaneVisibility("capsul_null", false);
    }
}

}  // namespace UI
}  // namespace Pulsar

// void PatchWFCMenu_LoadButton(PushButton* _this, const char* folderName, const char* ctrName, const char* variant, u32 localPlayerBitfield, u32 r8, bool inaccessible) {
//     _this->Load(folderName, "NewWifiMenuButton", variant, localPlayerBitfield, r8, inaccessible);
// }

// void PatchWFCMenu_AddCapsule(CtrlMenuInstructionText* _this, u32 bmgId, const Text::Info* info) {
//     Pages::WFCMainMenu* wfcMenu = SectionMgr::sInstance->curSection->Get<Pages::WFCMainMenu>();

//     Text::Info ninfo;
//     wfcMenu->regionalButton.SetPaneVisibility("capsul_null", true);

//     _this->SetMessage(bmgId, info);
// }

// kmCall(0x8064bc54, PatchWFCMenu_AddCapsule);
// kmCall(0x8064ba90, PatchWFCMenu_LoadButton);