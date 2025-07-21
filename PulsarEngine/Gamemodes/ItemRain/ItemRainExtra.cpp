#include <RetroRewind.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>

namespace Pulsar {
namespace ItemRain {

// The following patches are from the original gecko code [Unnamed, MrBean35000vr]
// Increase Item limits
kmWrite8(0x808A5D47, 0x00000022);
kmWrite8(0x808A5A3F, 0x00000022);
kmWrite8(0x808A538F, 0x00000022);
kmWrite8(0x808A56EB, 0x00000019);
kmWrite8(0x808A548B, 0x00000019);

// Allow POWs and Shocks to affect other players on their screens.
asmFunc ItemRainOnlineFix() {
    ASM(
        loc_0x0:
            addi r3, r4, 200;
            lis r12, 0x8000;
            lbz r12, 0x120C(r12);
            cmpwi r12, 0;
            beq loc_0x2C;
            lbz r3, 27(r27);
            cmpwi r3, 0x7;
            beq+ loc_0x24;
            cmpwi r3, 0x3;
            beq+ loc_0x24;
            cmpwi r3, 0x4;
            beq+ loc_0x24;
            addi r3, r4, 0xC8;
            b loc_0x2C;

        loc_0x24:
            mr r3, r4;
            b loc_0x2C;

        loc_0x2C:
    )
}
kmCall(0x8065BB40, ItemRainOnlineFix);

void ItemRainFix() {
    ItemRainOnlineFixHook = 0x00;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || 
        RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST || 
        RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_NONE) {
        if (Pulsar::System::sInstance->IsContext(PULSAR_ITEMMODESTORM) || Pulsar::System::sInstance->IsContext(PULSAR_ITEMMODERAIN)) {
            ItemRainOnlineFixHook = 0x00FF0100;
        }
    }
}
static PageLoadHook FixItemRain(ItemRainFix);

// Fix bombs not activating offline [Ro]
asmFunc ItemRainOfflineFix() {
    ASM(
        loc_0x0:
            lwz r4, 0x4(r30);
            cmpwi r4, 0x9;
            bne- loc_0x18;
            li r0, 0x12C;
            stw r0, 0x1DC(r30);
            lwz r0, -0xFC(r5);

        loc_0x18:
            stw r0, 0x170(r30);
            blr;
    )
}
kmCall(0x807A7170, ItemRainOfflineFix);

} // namespace ItemRain
} // namespace Pulsar