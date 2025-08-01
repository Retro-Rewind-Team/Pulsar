#ifndef _TWISTEDWAY_
#define _TWISTEDWAY_
#include <kamek.hpp>
#include <MarioKartWii/Objects/KCL/ObjectKCL.hpp>

namespace Objects {

class TwistedWay : public ObjectKCL {  // ObjectNum 0x1fe = 510
    explicit TwistedWay(const KMP::Holder<GOBJ>& gobj);  // 80813bd4
    ~TwistedWay();  // 80814918 vtable 808d62f8
    void OnStart() override;  // 0xC 80813c40
    void Update() override;  // 0x14 80813cfc
    u32 GetPropertiesBitfield() override;  // 0x2c 80814910
    void LoadAnimations() override;  // 0x5c 80814900
    void LoadCollision() override;  // 0x60 8081490c
    void UpdateCollision() override;  // 0x74 80814908
    float GetCollisionDiameter() const override;  // 0xa0 808148f8

    void vf_0xb4() override;  // 0xb4 80814904 just a blr

    bool vf_0xc0(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                 KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xc0 808148b8
    bool vf_0xc4(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                 KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xc4 808148c8
    bool vf_0xc8(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                 KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xc8 808148d8
    bool vf_0xcc(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                 KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xcc 808148e8

    bool IsCollidingNoTerrainInfoNoTriangleCheck(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                                                 KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xd0 808148a8
    bool IsCollidingAddEntryNoTerrainInfoNoTriangleCheck(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                                                         KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xd4 808148ac
    bool IsColliding(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                     KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xd8 808148b0
    bool IsCollidingNoTriangleCheck(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                                    KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xdc 808148b4

    bool IsCollidingNoTerrainInfo(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                                  KCLTypeHolder* ret) override;  // 0xe4 80814868
    bool IsCollidingAddEntryNoTerrainInfo(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                                          KCLTypeHolder* ret) override;  // 0xe8 80814878
    bool vf_0xec(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                 KCLTypeHolder* ret) override;  // 0xec 80814888
    bool IsCollidingAddEntry(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                             KCLTypeHolder* ret) override;  // 0xf0 80814898

    bool IsCollidingNoTerrainInfo(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                                  KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xf4 80814858
    bool IsCollidingAddEntryNoTerrainInfo(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                                          KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xf8 8081485c
    bool vf_0xfc(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                 KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0xfc 80814860
    bool IsCollidingAddEntry(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                             KCLTypeHolder* ret, u32 initialTime, float radius) override;  // 0x100 80814864

    bool AreItemsAllowed() override;  // 0x104 80814850
    float vf_0x108() override;  // 0x108 80814844
    void UpdateOtherEntityPos(CollisionInfo::ObjInfo* otherEntityObjInfo, Vec3& newEntityPos) override;  // 0x110 80814428
    void UpdateOtherEntityPosWithRot(CollisionInfo::ObjInfo* otherEntityObjInfo, Vec3& newEntityPos, Vec3& newEntityDir) override;  // 0x118 808145d8
    int vf_0x11c(CollisionInfo::ObjInfo* otherEntityObjInfo) override;  // 0x11c 8081483c

    bool IsCollidingImpl(const Vec3& pos, const Vec3& prevPos, KCLBitfield accepted, CollisionInfo* info,
                         KCLTypeHolder* ret, u32 initialTime, float radius);  // 80815d64
    u32 someFrameCount;  // 0xac init at 1000
    float unknown_0xb0[4];
    u8 unknown_0xc0[0x10c - 0xc0];
};  // 0x10c

}  // namespace Objects

#endif