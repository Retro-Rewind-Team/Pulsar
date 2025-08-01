#ifndef _MIIGROUP_
#define _MIIGROUP_
#include <kamek.hpp>
#include <core/egg/mem/UnitHeap.hpp>
#include <MarioKartWii/Mii/MiiCreationParams.hpp>

class MiiTexObj {
    GX::TexObj texObj;
    bool hasMiiObj;  // 0x20
    u8 padding[3];
};  // 0x24

class MiiGroup {
   public:
    MiiGroup();  // 805f9e78 vtable 808b9f68
    virtual ~MiiGroup();  // 805f9f58 vtable 808b9f68
    void Init(u32 miiCount, u32 usedTexObjSlotsBitfield, EGG::Heap* heap);  // 805fa04c
    void DeleteMii(u8 idx);  // 805faa84
    void Reset();  // 805fa370
    void ResetCreationParams();  // 805fa3c4

    void AddMii(u8 idx, Mii* mii);  // 805fa3fc
    void LoadStoreMii(u8 idx, RFL::CreateID* createId);  // 805fa6e0 replaces idx with new mii
    void LoadOfficialMii(u8 idx, RFL::IDX rflIdx);  // 805fa678 id may be invalid
    void LoadMii(u8 idx, RFL::StoreData* data);  // 805fa610
    void LoadDefaultMii(u8 idx, RFL::IDX rflId);  // 805fa748
    void LoadMiddleDB2Mii(u8 idx, u8 creationParamsIdx);  // 805fa580

    Mii* GetMii(u8 idx);  // 805fa930
    GX::TexObj* GetMiiTexObj(u8 idx, u8 textureIdx);  // 805fa964
    bool HasMiiTexObjs(u8 idx);  // 805fa97c tests all textures
    void CopyMii(u8 srcIdx, u8 destIdx);  // 805fabf4
    void CopyMiiToOtherGroup(MiiGroup& other, u8 srcIdx, u8 destIdx);  // 805faf34
    void ReplaceMiiByPlayerMii(u8 idxToReplace, u8 playerId, u8 hudSlotId);  // 805fa8b8

    Mii** mii;  // 0x4 pointer to the array of the miis loaded
    MiiTexObj* texObj[7];  // 0x8
    // Array of pointers to MiiTexObj arrays, each of size MiiCount
    // Each TexObj array has its own role, and their attached buffers have a different size (sub struct of size 0x10 80896328 has u16 size)
    // Role yet to be determined, but idx 2 seems to be for UI, facing to the left

    void* buffer;  // 0x24 size = sum of miiCount * a size param * isUsedTexObjSlot for each 7 slots
    u32 miiCount;  // 0x28
    EGG::UnitHeap* unitHeap;  // 0x2c
    MiiCreationParams2 creationParams;  // 0x30
    MiiCreationParams2 creationParams2;  // 0x64
};  // Total Size 0x98
size_assert(MiiGroup, 0x98);

#endif