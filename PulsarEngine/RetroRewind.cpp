#include <MarioKartWii/Race/Racedata.hpp>
#include <SlotExpansion/CupsConfig.hpp>
#include <Settings/UI/SettingsPanel.hpp>
#include <Settings/Settings.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <RetroRewind.hpp>

namespace RetroRewind {
Pulsar::System *System::Create() {
    return new System(); //now Pulsar::sInstance is of type RetroRewind
}
Pulsar::System::Inherit CreateRetroRewind(System::Create);

bool System::Is500cc() {
    return Racedata::sInstance->racesScenario.settings.engineClass == CC_50;
}

System::WeightClass System::GetWeightClass(const CharacterId id){
    switch (id)
    {
        case BABY_MARIO:
        case BABY_LUIGI:
        case BABY_PEACH:
        case BABY_DAISY:
        case TOAD:
        case TOADETTE:
        case KOOPA_TROOPA:
        case DRY_BONES:
            return LIGHTWEIGHT;
        case MARIO:
        case LUIGI:
        case PEACH:
        case DAISY:
        case YOSHI:
        case BIRDO:
        case DIDDY_KONG:
        case BOWSER_JR:
            return MEDIUMWEIGHT;
        case WARIO:
        case WALUIGI:
        case DONKEY_KONG:
        case BOWSER:
        case KING_BOO:
        case ROSALINA:
        case FUNKY_KONG:
        case DRY_BOWSER:
            return HEAVYWEIGHT;
        default:
            return MIIS;
    }
}

//Force 30 FPS [Vabold]
kmWrite32(0x80554224, 0x3C808000);
kmWrite32(0x80554228, 0x88841204);
kmWrite32(0x8055422C, 0x48000044);

void FPSPatch() {
  FPSPatchHook = 0x00;
  if (static_cast<Pulsar::FPS>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR2), Pulsar::SETTIGNRR2_RADIO_FPS)) == Pulsar::FPS_HALF) {
      FPSPatchHook = 0x00FF0100;
  }
}
static PageLoadHook PatchFPS(FPSPatch);

//Item Box Respawn Modifier [Unnamed]
asmFunc GetItemBoxRespawn() {
    ASM(
loc_0x0:
  stw       r0, 0xB0(r27);
  lis       r12, 0x8000;
  lbz       r12, 0x1206(r12);
  cmpwi     r12, 0;
  beq       end;
  li        r12, 0x5A;
  stw       r12, 0xB8(r27);
  stw       r0, 0xB0(r27);

end:
  blr;
    )
}
kmCall(0x80828EDC, GetItemBoxRespawn);

void ItemBoxPatch() {
  bool isFastRespawn = Pulsar::System::sInstance->IsContext(Pulsar::PULSAR_ITEMBOXRESPAWN) ? Pulsar::ITEMBOX_DEFAULTRESPAWN : Pulsar::ITEMBOX_FASTRESPAWN;
  ItemBoxHook = 0x00;
  if (isFastRespawn == Pulsar::ITEMBOX_DEFAULTRESPAWN) {
    ItemBoxHook = 0x00FF0100;
  }
}
static PageLoadHook PatchItemBox(ItemBoxPatch);

//No Disconnect from being Idle [Bully]
asmFunc GetNoDC() {
    ASM(
loc_0x0:
  addi      r0, r3, 1;
  lis       r12, 0x8000;
  lbz       r12, 0x1208(r12);
  cmpwi     r12, 0;
  beq       end;
  li        r0, 0;

end:
  blr;
    )
}
kmCall(0x80521408, GetNoDC);
kmCall(0x8053EF6C, GetNoDC);
kmCall(0x8053F0B4, GetNoDC);
kmCall(0x8053F124, GetNoDC);

void NoDCPatch() {
  DCHook = 0x00;
  if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST) {
    DCHook = 0x00FF0100;
  }
}
static PageLoadHook PatchNoDC(NoDCPatch);

asmFunc ItemVanish() {
    ASM(
        nofralloc;
loc_0x0:
  lfs f0, 0x0(r31);
  fcmpo cr0, f1, f0;
  lis r12, 0x8000;
  lbz r12, 0x120A(r12);
  cmpwi cr7, r12, 0;
  beqlr cr7;
  cmpwi r0, 0xA;

end:
  blr;
    )
}
kmCall(0x8079F748, ItemVanish);

asmFunc InstantFinish() {
    ASM(
  lis       r12, 0x3;
  ori       r12, r12, 0x387;
  stw       r12, 0x24(r30);
  lha       r3, 0x24(r30);
    )
}
kmCall(0x80535508, InstantFinish);

void OTTPatch() {
  OTTFixHook = 0x00;
  if (Pulsar::System::sInstance->IsContext(Pulsar::PULSAR_MODE_OTT)) {
      OTTFixHook = 0x00FF0100;
  }
}
static PageLoadHook PatchOTT(OTTPatch);

//Simple Cheat code crash [Cats4Life]
void CodeCrash() {
    if(DolphinCheat == 0x00000001) Pulsar::Debug::FatalError("Please disable all cheat codes.");
    else if(AntiCheat1 != 0x00000000000000000000000000000000) Pulsar::Debug::FatalError("Please check your codehandler.");
    else if(AntiCheat2 != 0x00000000000000000000000000000000) Pulsar::Debug::FatalError("Please check your codehandler.");
    else if(AntiCheat3 != 0x00000000000000000000000000000000) Pulsar::Debug::FatalError("Please check your codehandler.");
    else if(AntiCheat4 != 0x00000000000000000000000000000000) Pulsar::Debug::FatalError("Please check your codehandler.");
    else if(AntiCheat5 != 0x00000000000000000000000000000000) Pulsar::Debug::FatalError("Please check your codehandler.");
    else if(AntiCheat6 != 0x00000000000000000000000000000000) Pulsar::Debug::FatalError("Please check your codehandler.");
    else if(AntiCheat7 != 0x00000000000000000000000000000000) Pulsar::Debug::FatalError("Please check your codehandler.");
    else if(MainDolCheat != 0x00000000000000000000000000000000) Pulsar::Debug::FatalError("Please disable all cheat codes.");
    
}
static PageLoadHook CRASH_HOOK(CodeCrash);

//GeckoLoader Patch [Toadette Hack Fan]
void GeckoLoader() {
    if(GeckoLoader1 != 0x00000000) Pulsar::Debug::FatalError("Please reinstall the distribution.");
}
BootHook GECKOLOADERDETECTION(GeckoLoader, 2);

} // namespace RetroRewind