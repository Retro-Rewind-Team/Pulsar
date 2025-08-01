#ifndef _WLWALLGC_
#define _WLWALLGC_
#include <kamek.hpp>
#include <MarioKartWii/Objects/KCL/ExternalKCL/ObjectExternKCL.hpp>

namespace Objects {
class WLwallGC : public ObjectExternKCL {  // ObjectNum 0xcb = 203
    static u32 count;  // 0x809c4760, incremented by 1 at the end of the ctor
    explicit WLwallGC(const KMP::Holder<GOBJ>& gobjHolder);  // 8086bc1c
    ~WLwallGC() override;  // 8086bde4 vtable 808dbfe0
    void OnStart() override;  // 0xC 8086be34
    void Update() override;  // 0x14 8086c108
    u32 GetPropertiesBitfield() override;  // 0x2c 8086c640
    void CalcOtherEntityDistance(CollisionInfo::ObjInfo* otherEntityObjInfo, const Vec3& otherEntityInitialPosition) override;  // 0x110 8086c280  sets initial position
    void UpdateOtherEntityPos(CollisionInfo::ObjInfo* otherEntityObjInfo, Vec3& newEntityPos) override;  // 0x114 8082c2a8
    int vf_0x11c(CollisionInfo::ObjInfo* otherEntityObjInfo) override;  // 0x11c 8086c684
    const Mtx34& GetTransformationMatrix(u32 framesOffset) override;  // 0x12c 8086bf30
    float GetPeriod() const override;  // 0x134 8086c648
    bool IsCollidingNoTriangleCheckImpl(const Vec3& pos, const Vec3& prevPos,
                                        KCLBitfield accepted, CollisionInfo* info, KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0x148 8086c328
    bool IsCollidingImpl(const Vec3& pos, const Vec3& prevPos,
                         KCLBitfield accepted, CollisionInfo* info, KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0x14c 8086c5a8
    static Vec3 CalcNextPosition(const Vec& basePos, const Vec& finalPos, float percent);  // 8086c098 (final - base) * percent + base
    u8 unknown_0x104[0x170 - 0x104];
    u32 idx;  // 0x104 809c4760, incremented by 1 at the end of the ctor
    u32 timeRetracted;  // 0x108
    u32 timeExtended;  // 0x10c
    u32 movementPeriod;  // 0x110 length setting / speed setting
    u32 goingOutFirstTime;  // 0x114
    u32 timeRetracted2;  // 0x118
    u32 timeRetractedAndMoving;  // 0x11c
    u32 timeExtendedAndMoving;  // 0x120
    u32 totalPeriod;  // 0x124
    Vec3 retractedPosition;  // 0x128
    Vec3 extendedPosition;  // 0x134 position - length * (transfo Z)
    Mtx34 transformationMtx2;  // 0x140
};  // 0x170

}  // namespace Objects
#endif