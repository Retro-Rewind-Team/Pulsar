#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/Obj/Kumo.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <PulsarSystem.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Settings/SettingsParam.hpp>

namespace Pulsar {
namespace Race {

// Mega TC
void MegaTC(Kart::Movement& movement, int frames, int unk0, int unk1) {
    const System* system = System::sInstance;
    bool isMegaTC = true;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST ||
        RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        if (system->IsContext(PULSAR_THUNDERCLOUD) == THUNDERCLOUD_NORMAL) {
            isMegaTC = false;
        } else {
            isMegaTC = true;
        }
    }
    if (isMegaTC && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_VS_WW)
        movement.ActivateMega();
    else
        movement.ApplyLightningEffect(frames, unk0, unk1);
}
kmCall(0x80580630, MegaTC);

void LoadCorrectTCBRRES(Item::ObjKumo& objKumo, const char* mdlName, const char* shadowSrc, u8 whichShadowListToUse,
                        Item::Obj::AnmParam* anmParam) {
    const System* system = System::sInstance;
    bool isMegaTC = true;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST ||
        RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        if (system->IsContext(PULSAR_THUNDERCLOUD) == THUNDERCLOUD_NORMAL) {
            isMegaTC = false;
        } else {
            isMegaTC = true;
        }
    }
    if (isMegaTC && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_VS_WW && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_JOINING_WW)
        objKumo.LoadGraphics("megaTC.brres", mdlName, shadowSrc, 1, anmParam,
                             static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
    else
        objKumo.LoadGraphicsImplicitBRRES(mdlName, shadowSrc, 1, anmParam, static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr);
}
kmCall(0x807af568, LoadCorrectTCBRRES);

}  // namespace Race
}  // namespace Pulsar