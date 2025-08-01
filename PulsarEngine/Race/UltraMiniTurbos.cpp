#include <MarioKartWii/Kart/KartManager.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Race/UltraMiniTurbos.hpp>
#include <Sound/MiscSound.hpp>
#include <PulsarSystem.hpp>
#include <RetroRewind.hpp>
#include <MarioKartWii/File/RKG.hpp>

namespace Pulsar {
namespace Race {
static EGG::EffectResource* pulEffects = nullptr;
static EGG::EffectResource* rrEffects = nullptr;

const char* ExpPlayerEffects::UMTNames[8] = {
    "rk_driftSpark3L_Spark00",
    "rk_driftSpark3L_Spark01",
    "rk_driftSpark3R_Spark00",
    "rk_driftSpark3R_Spark01",
    "rk_purpleTurbo",
    "rk_purpleTurbo",
    "rk_purpleTurbo",
    "rk_purpleTurbo"};

const char* ExpPlayerEffects::SMTNames[8] = {
    "rk_driftSpark2L_Spark00",
    "rk_driftSpark2L_Spark01",
    "rk_driftSpark2R_Spark00",
    "rk_driftSpark2R_Spark01",
    "rk_orangeTurbo",
    "rk_orangeTurbo",
    "rk_orangeTurbo",
    "rk_orangeTurbo"};

// Needed so that other players display the correct effect
bool umtState[12];  // false = no UMT  true = UMT buff active expanding Kart::Movement just for this doesn't seem like the plan

kmWrite32(0x8057ee5c, 0x2c050004);  // changes >= 3 to >= 4 for UMT
kmWrite32(0x8057ef30, 0x2c000001);  // changes check from if != 2 to if = 1, so that when in a SMT the function keeps going
kmWrite32(0x8057ef38, 0x418200A4);  // ensure mtSpeedMultiplier gets reset when driftState = 1, by sending to where CreateUMT hooks
kmWrite32(0x8057efb4, 0x48000028);  // skips the SMT charge check and sends unconditionally to CreateUMT
void CreateUMT(Kart::Movement& movement) {
    bool isUMTs = System::sInstance->IsContext(PULSAR_UMTS);
    const s16 smtCharge = movement.smtCharge;
    if (smtCharge >= 550 && isUMTs)
        movement.driftState = 4;
    else if (smtCharge >= 300)
        movement.driftState = 3;
};
kmBranch(0x8057efdc, CreateUMT);

kmWrite32(0x80588894, 0x2c000003);  // changes >= 2 to >= 3 for SMT
kmWrite32(0x8058889c, 0x2c000003);  // changes check from if != 1 to if = 3, so that when in a MT the function keeps going
kmWrite32(0x805888a8, 0x418200A4);  // if in charge state 2, skip to SetBikeDriftTiers
kmWrite32(0x80588928, 0x60000000);  // removed fixed 270 write to mtCharge
kmWrite32(0x80588938, 0x7c040378);  // setup "charged" for next function
kmWrite32(0x8058893c, 0x48000010);  // takes MT charge check and parses it into SetBikeDriftTiers
void SetBikeDriftTiers(Kart::MovementBike& movement, bool charged) {
    bool isSMT = true;
    const RKNet::RoomType roomType = RKNet::Controller::sInstance->roomType;
    if (roomType == RKNet::ROOMTYPE_VS_WW || roomType == RKNet::ROOMTYPE_BT_WW) isSMT = false;
    if (!System::sInstance->IsContext(PULSAR_UMTS)) isSMT = false;
    if (charged) {
        movement.driftState = 2;
        KartType type = movement.GetType();
        const s16 mtCharge = movement.mtCharge;
        const GameMode gameMode = Racedata::sInstance->racesScenario.settings.gamemode;
        if (type == OUTSIDE_BIKE && isSMT == true) {
            if (mtCharge >= 570) movement.driftState = 3;
        }
    }
}
kmBranch(0x8058894c, SetBikeDriftTiers);

// Buffs MTStats and updates umtState
int BuffUMT(const Kart::Movement& movement) {
    const u8 idx = movement.GetPlayerIdx();
    u32 mtStat = movement.GetStats().mt;
    bool* state = umtState;
    if (movement.driftState == 4) state[idx] = true;
    if (state[idx] == true) mtStat = 3 * mtStat / 2;  // 50% longer
    return mtStat;
};
kmCall(0x80582fdc, BuffUMT);
kmWrite32(0x80582fe0, 0x7C601B78);
kmWrite32(0x80582fec, 0x4180003C);  // changes !=3 to <3 for UMT

// SpeedMultiplier "perk" implementation
// kmWrite32(0x80579344, 0x7F83E378); //change r3 to movement
bool UpdateSpeedMultiplier(Kart::Boost& boost, bool* boostEnded) {
    const GameMode gameMode = Racedata::sInstance->racesScenario.settings.gamemode;
    const bool isBoosting = boost.Update(boostEnded);
    register Kart::Movement* movement;
    asm(mr movement, r28;);
    const u8 id = movement->GetPlayerIdx();
    bool* state = umtState;
    const float umtMultiplier = 1.32;  // 10% faster
    const float insideDriftMultiplier = 1.236f;  // 3% faster
    const float defaultMTMultiplier = 1.2f;
    // determine default inside drift context
    bool insideAll = Pulsar::FORCE_TRANSMISSION_DEFAULT;
    bool vanilla = Pulsar::FORCE_TRANSMISSION_DEFAULT;
    bool outsideAll = Pulsar::FORCE_TRANSMISSION_DEFAULT;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST ||
        RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        insideAll = System::sInstance->IsContext(Pulsar::PULSAR_TRANSMISSIONINSIDE)
                        ? Pulsar::FORCE_TRANSMISSION_INSIDE
                        : Pulsar::FORCE_TRANSMISSION_DEFAULT;
        vanilla = System::sInstance->IsContext(Pulsar::PULSAR_TRANSMISSIONVANILLA)
                      ? Pulsar::FORCE_TRANSMISSION_VANILLA
                      : Pulsar::FORCE_TRANSMISSION_DEFAULT;
        outsideAll = System::sInstance->IsContext(Pulsar::PULSAR_TRANSMISSIONOUTSIDE)
                         ? Pulsar::FORCE_TRANSMISSION_OUTSIDE
                         : Pulsar::FORCE_TRANSMISSION_DEFAULT;
    }
    // check if a matching ghost combo is present (same kart+character)
    const RacedataScenario& scenario = Racedata::sInstance->racesScenario;
    int comboGhostIdx = -1;
    for (int pid = 0; pid < scenario.playerCount; ++pid) {
        if (scenario.players[pid].playerType == PLAYER_GHOST &&
            scenario.players[pid].kartId == scenario.players[id].kartId &&
            scenario.players[pid].characterId == scenario.players[id].characterId) {
            comboGhostIdx = pid;
            break;
        }
    }
    bool ghostCombo = comboGhostIdx >= 0;
    u32 savedTrans = 0;
    if (ghostCombo) {
        u8 offset = (scenario.players[0].playerType != PLAYER_GHOST) ? 1 : 0;
        int rkgIndex = comboGhostIdx - offset;
        if (rkgIndex >= 0) {
            RKG& ghostRkg = Racedata::sInstance->ghosts[rkgIndex];
            savedTrans = ghostRkg.header.unknown_3;
        }
    }

    // determine whether to use inside drift multiplier (respect ghost combo override if present)
    bool useInside;
    if (ghostCombo) {
        useInside = (savedTrans == Pulsar::TRANSMISSION_INSIDEALL);
    } else {
        u32 userTrans = Pulsar::Settings::Mgr::Get().GetUserSettingValue(
            static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR),
            Pulsar::SETTINGRR_RADIO_TRANSMISSION);
        useInside = ((static_cast<Pulsar::Transmission>(userTrans) == Pulsar::TRANSMISSION_INSIDEALL) || (insideAll == Pulsar::FORCE_TRANSMISSION_INSIDE)) && vanilla != Pulsar::FORCE_TRANSMISSION_VANILLA && outsideAll != Pulsar::FORCE_TRANSMISSION_OUTSIDE;
    }
    if (useInside) {
        if (!isBoosting) state[id] = false;
        if (boost.multiplier == defaultMTMultiplier || boost.multiplier == insideDriftMultiplier) {
            boost.multiplier = state[id] ? insideDriftMultiplier : defaultMTMultiplier;
        }
    } else {
        if (!isBoosting) state[id] = false;
        if (boost.multiplier == defaultMTMultiplier || boost.multiplier == umtMultiplier) {
            boost.multiplier = state[id] ? umtMultiplier : defaultMTMultiplier;
        }
    }
    return isBoosting;
}
kmCall(0x8057934c, UpdateSpeedMultiplier);

// Expanded player effect, also hijacked to add custom breff/brefts to Effects::Mgr
static void CreatePlayerEffects(Effects::Mgr& mgr) {  // adding the resource here as all other breff have been loaded at this point
    const ArchiveMgr* root = ArchiveMgr::sInstance;
    if (System::sInstance->IsContext(PULSAR_UMTS)) {
        void* breff = root->GetFile(ARCHIVE_HOLDER_COMMON, System::breff, 0);
        void* breft = root->GetFile(ARCHIVE_HOLDER_COMMON, System::breft, 0);
        EGG::EffectResource* res = new EGG::EffectResource(breff, breft);
        if (mgr.resCount != 9)
            mgr.resources[mgr.resCount] = res;
        else
            pulEffects = res;
        breff = root->GetFile(ARCHIVE_HOLDER_COMMON, "/Effect/RR.breff", 0);
        breft = root->GetFile(ARCHIVE_HOLDER_COMMON, "/Effect/RR.breft", 0);
        res = new EGG::EffectResource(breff, breft);
        if (mgr.resCount != 9)
            mgr.resources[mgr.resCount] = res;
        else
            rrEffects = res;
    }
    for (int i = 0; i < Racedata::sInstance->racesScenario.playerCount; ++i) {
        mgr.players[i] = new (ExpPlayerEffects)(Kart::Manager::sInstance->GetKartPlayer(i));
    }
}
kmCall(0x80554624, CreatePlayerEffects);

static void DeleteEffectRes(Effects::Mgr& mgr) {
    delete (pulEffects);
    pulEffects = nullptr;
    delete (rrEffects);
    rrEffects = nullptr;
    mgr.Reset();
}
kmCall(0x8051b198, DeleteEffectRes);

// Loads the custom effects
static void LoadCustomEffects(ExpPlayerEffects& effects) {
    effects.LoadEffects();
    effects.rk_purpleMT = new EGG::Effect*[ExpPlayerEffects::UmtEffectsCount];
    for (int i = 0; i < ExpPlayerEffects::UmtEffectsCount; ++i) {
        effects.rk_purpleMT[i] = new (EGG::Effect)(ExpPlayerEffects::UMTNames[i], effects.playerIdPlus2);
    }
    effects.rk_orangeMT = new EGG::Effect*[ExpPlayerEffects::SmtEffectsCount];
    for (int i = 0; i < ExpPlayerEffects::SmtEffectsCount; ++i) {
        effects.rk_orangeMT[i] = new (EGG::Effect)(ExpPlayerEffects::SMTNames[i], effects.playerIdPlus2);
    }
};
kmCall(0x8068e9c4, LoadCustomEffects);

// Left and Righ sparks when the SMT charge is over 550
void LoadLeftPurpleSparkEffects(ExpPlayerEffects& effects, EGG::Effect** effectArray, u32 firstEffectIndex, u32 lastEffectIndex, const Mtx34& playerMat2, const Vec3& wheelPos, bool updateScale) {
    const u32 smtCharge = effects.kartPlayer->pointers.kartMovement->smtCharge;
    if (smtCharge >= 550 && System::sInstance->IsContext(PULSAR_UMTS)) {
        effects.CreateAndUpdateEffectsByIdx(effects.rk_purpleMT, 0, 2, playerMat2, wheelPos, updateScale);
        effects.FollowFadeEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
    } else
        effects.CreateAndUpdateEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
};
kmCall(0x80698a94, LoadLeftPurpleSparkEffects);

void LoadRightPurpleSparkEffects(ExpPlayerEffects& effects, EGG::Effect** effectArray, u32 firstEffectIndex, u32 lastEffectIndex, const Mtx34& playerMat2, const Vec3& wheelPos, bool updateScale) {
    const u32 smtCharge = effects.kartPlayer->pointers.kartMovement->smtCharge;
    if (smtCharge >= 550 && System::sInstance->IsContext(PULSAR_UMTS)) {
        effects.CreateAndUpdateEffectsByIdx(effects.rk_purpleMT, 2, 4, playerMat2, wheelPos, updateScale);
        effects.FollowFadeEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
    } else
        effects.CreateAndUpdateEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
};
kmCall(0x80698af0, LoadRightPurpleSparkEffects);

void LoadOrangeSparkEffects(ExpPlayerEffects& effects, EGG::Effect** effectArray, u32 firstEffectIndex, u32 lastEffectIndex, const Mtx34& playerMat2, const Vec3& wheelPos, bool updateScale) {
    KartType type = effects.kartPlayer->GetType();
    const u32 mtCharge = effects.kartPlayer->pointers.kartMovement->mtCharge;
    const GameMode gameMode = Racedata::sInstance->racesScenario.settings.gamemode;
    bool isSMT = true;
    const RKNet::RoomType roomType = RKNet::Controller::sInstance->roomType;
    if (roomType == RKNet::ROOMTYPE_VS_WW || roomType == RKNet::ROOMTYPE_BT_WW) isSMT = false;
    if (!System::sInstance->IsContext(PULSAR_UMTS)) isSMT = false;
    if (mtCharge >= 570 && type == OUTSIDE_BIKE && isSMT == true) {
        effects.CreateAndUpdateEffectsByIdx(effects.rk_orangeMT, 0, 2, playerMat2, wheelPos, updateScale);
        effects.FollowFadeEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
    } else
        effects.CreateAndUpdateEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
};
kmBranch(0x806a2f60, LoadOrangeSparkEffects);
kmBranch(0x806a3004, LoadOrangeSparkEffects);

// Fade the sparks
void FadeLeftPurpleSparkEffects(ExpPlayerEffects& effects, EGG::Effect** effectArray, u32 firstEffectIndex, u32 lastEffectIndex, const Mtx34& playerMat2, const Vec3& wheelPos, bool updateScale) {
    if (System::sInstance->IsContext(PULSAR_UMTS)) effects.FollowFadeEffectsByIdx(effects.rk_purpleMT, 0, 2, playerMat2, wheelPos, updateScale);
    effects.FollowFadeEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
};
kmCall(0x80698dac, FadeLeftPurpleSparkEffects);
kmCall(0x80698228, FadeLeftPurpleSparkEffects);
kmCall(0x80698664, FadeLeftPurpleSparkEffects);
kmCall(0x80698ab4, FadeLeftPurpleSparkEffects);

void FadeRightPurpleSparkEffects(ExpPlayerEffects& effects, EGG::Effect** effectArray, u32 firstEffectIndex, u32 lastEffectIndex, const Mtx34& playerMat2, const Vec3& wheelPos, bool updateScale) {
    if (System::sInstance->IsContext(PULSAR_UMTS)) effects.FollowFadeEffectsByIdx(effects.rk_purpleMT, 2, 4, playerMat2, wheelPos, updateScale);
    effects.FollowFadeEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
};
kmCall(0x80698248, FadeRightPurpleSparkEffects);
kmCall(0x80698684, FadeRightPurpleSparkEffects);
kmCall(0x80698b10, FadeRightPurpleSparkEffects);
kmCall(0x80698dcc, FadeRightPurpleSparkEffects);

void FadeOrangeSparkEffects(ExpPlayerEffects& effects, EGG::Effect** effectArray, u32 firstEffectIndex, u32 lastEffectIndex, const Mtx34& playerMat2, const Vec3& wheelPos, bool updateScale) {
    effects.FollowFadeEffectsByIdx(effectArray, firstEffectIndex, lastEffectIndex, playerMat2, wheelPos, updateScale);
    effects.FollowFadeEffectsByIdx(effects.rk_orangeMT, 0, 2, playerMat2, wheelPos, updateScale);
};
kmBranch(0x806a31fc, FadeOrangeSparkEffects);

int PatchDriftStateCheck(const Kart::Player& kartPlayerPlayer) {
    u32 driftState = kartPlayerPlayer.GetDriftState();
    if (driftState == 4) driftState = 3;
    return driftState;
};
kmCall(0x8069807c, PatchDriftStateCheck);

// Purple boosts
// kmWrite32(0x806a3d00, 0x7FA4EB78);
// kmWrite32(0x806a3d04, 0x7FC5F378);
void PatchBoostOnUMTSpeedBoost(EGG::Effect* boostEffect) {  // have to mod loop index by 4 to get the actual index
    register u8 loopIndex;
    asm(mr loopIndex, r29;);
    register ExpPlayerEffects* effects;
    asm(mr effects, r30;);

    Kart::Movement* movement = effects->kartPlayer->pointers.kartMovement;
    if (umtState[effects->playerId]) boostEffect = effects->rk_purpleMT[rk_purpleBoost + loopIndex % 4];
    boostEffect->Create();
};
kmCall(0x806a3d08, PatchBoostOnUMTSpeedBoost);

kmWrite32(0x8069bfa0, 0x60000000);
// kmWrite32(0x8069bfdc, 0x7FA5EB78);
// kmWrite32(0x8069bfe4, 0x7FC6F378);
void PatchBoostMatrix(EGG::Effect* boostEffect, const Mtx34& boostMat) {
    if (boostEffect->effectHandle.GetPtr()) {
        boostEffect->SetMtx(boostMat);
        boostEffect->Update();
    }
    register u8 loopIndex;
    asm(mr loopIndex, r29;);
    register ExpPlayerEffects* effects;
    asm(mr effects, r30;);
    if (!effects->isBike && System::sInstance->IsContext(PULSAR_UMTS)) {
        boostEffect = effects->rk_purpleMT[rk_purpleBoost + loopIndex % 4];
        if (boostEffect->effectHandle.GetPtr()) {
            boostEffect->SetMtx(boostMat);
            boostEffect->Update();
        }
    }
}
kmCall(0x8069bff0, PatchBoostMatrix);
kmWrite32(0x8069c004, 0x60000000);

// kmWrite32(0x8069c098, 0x7FC4F378);
// kmWrite32(0x8069c09c, 0x7FE5FB78);
void PatchFadeBoost(EGG::Effect* boostEffect) {
    boostEffect->FollowFade();
    register u8 loopIndex;
    asm(mr loopIndex, r30;);
    register ExpPlayerEffects* effects;
    asm(mr effects, r31;);
    if (!effects->isBike && System::sInstance->IsContext(PULSAR_UMTS)) effects->rk_purpleMT[rk_purpleBoost + loopIndex % 4]->FollowFade();
}
kmCall(0x8069c0a4, PatchFadeBoost);

// Currently uses blue shell sounds for lack of a better one
kmWrite32(0x807095b8, 0x40A00028);  // changes beq to bge for UMT
static void PatchUMTSound(Audio::KartActor& sound, u32 soundId, Audio::Handle& handle) {
    if (sound.driftState == 4) {
        const char* seqName = "purpleMT.brseq";
        const char* labelName = "b";
        snd::SoundStartable::StartResult ret = Sound::PlayExtBRSEQ(sound, handle, seqName, labelName, true);
        if (ret == snd::SoundStartable::START_SUCCESS) return;
    }
    sound.KartActor::HoldSound(soundId, &handle);
};
kmCall(0x807095f8, PatchUMTSound);
}  // namespace Race
}  // namespace Pulsar