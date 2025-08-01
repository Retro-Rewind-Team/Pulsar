#ifndef _CTRLRACE2DMAP_
#define _CTRLRACE2DMAP_
#include <kamek.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceBase.hpp>
#include <MarioKartWii/Kart/KartPlayer.hpp>

//_sinit_ at 807ebeec

// Object can refer to karts, the startline, etc...
class CtrlRace2DMapObject : public CtrlRaceBase {  // used as a base for other objects but also used for the start line as is
   public:
    ~CtrlRace2DMapObject() override;  // 807ea930 vtable 808d3900
    void InitSelf() override;  // 0x18 807eac2c
    void OnUpdate() override;  // 0x1c 807eac88
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 0x28 807ebe78
    const char* GetClassName() const override;  // 0x2c 807eac1c
    virtual void CalculatePosition(const Vec3& objectPosition, Vec2& dest, u32 r6);  // 0x50 807eacfc
    virtual void SetMapPosition(const Vec3& translation);  // 0x54 807eaa88
    virtual void SetMapRotation(const Vec3& rotation);  // 0x58 807eaaa4
    virtual char* GetPaneName() const;  // 0x5c 807eac7c
    nw4r::lyt::Pane* pane;
    Vec3 mapPosition;  // 0x19c
    Vec3 mapRotation;  // 0x1a0
};  // 0x1b4

class CtrlRace2DMapCharacter : public CtrlRace2DMapObject {
   public:
    CtrlRace2DMapCharacter();  // 807ea89c
    ~CtrlRace2DMapCharacter() override;  // 807ea98c vtable 808d389c
    void InitSelf() override;  // 0x18 807eaecc
    void OnUpdate() override;  // 0x1c 807eb250
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 0x28 807ebe6c
    const char* GetClassName() const override;  // 0x2c 807eadf0
    void CalculatePosition(const Vec3& kartPosition, Vec2& dest, u32 r6) override;  // 0x50 807eb3cc
    char* GetPaneName() const override;  // 0x5c 807ebe60
    virtual void Load(u8 index);  // 0x60 807eae00
    u8 playerId;  // 0x1b4
    u8 padding_0x1b5[3];
    nw4r::lyt::Pane* charaPane;  // 0x1b8
    nw4r::lyt::Pane* charaShadow0Pane;  // 0x1bc
    nw4r::lyt::Pane* charaShadow1Pane;  // 0x1c0
    nw4r::lyt::Pane* lightPane;  // 0x1c4
    Kart::Player* player;  // 0x1c8
};  // total size 0x1cc
size_assert(CtrlRace2DMapCharacter, 0x1cc);

class CtrlRace2DMapBossObj : public CtrlRace2DMapObject {
    CtrlRace2DMapBossObj();  // 807ea9ec
    ~CtrlRace2DMapBossObj() override;  // 807eaa28 vtable 808d3838
    void InitSelf() override;  // 0x18 807eba60
    void OnUpdate() override;  // 0x1c 807ebd50
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 0x28 807ebe54
    const char* GetClassName() const override;  // 0x2c 807eb9f8
    char* GetPaneName() const override;  // 0x5c 807ebe48
    virtual void Load(u8 index);  // 0x60 807eba08
    u8 unknown_0x1b4[0x1bc - 0x1b4];
};  // total size 0x1bc

class CtrlRace2DMap : public CtrlRaceBase {  // has children controls such as start line
    ~CtrlRace2DMap() override;  // 807ebe84 vtable 808d3960
    void InitSelf() override;  // 807eaac0
    void OnUpdate() override;  // 807eab3c
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 807ebee0
    const char* GetClassName() const override;  // 807ea270
    void Load(const char* variant);  // 807ea27c
    nw4r::lyt::TexMap* texMap;  // 0x198
    CtrlRace2DMapCharacter* characters;  // 0x19c size playerCount
    CtrlRace2DMapBossObj* boosObjs;  // 0x1a0
};  // 0x1a4
size_assert(CtrlRace2DMap, 0x1a4);

#endif