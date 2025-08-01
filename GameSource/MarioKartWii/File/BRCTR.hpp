#ifndef _BRCTR_
#define _BRCTR_
#include <kamek.hpp>

struct BRCTRAnimSubHeader {
    u16 firstGroupOffset;  // 0x0
    u16 groupsCount;  // 0x2
    u16 firstAnimOffset;  // 0x4
    u16 animationsCount;  // 0x6
};  // 0x8

struct BRCTRAnimGroup {
    u16 offsetToName;
    u16 offsetToBRLYTPaneName;
    u16 firstAnimIndex;
    u16 groupAnimationsCount;
};  // total 0x8

struct BRCTRAnimation {
    u16 offsetToName;
    u16 offsetToBRLANName;
    u16 offsetToNextAnimBRLANName;
    u16 isAnimationReversed;
    float animationSpeed;
};  // total 0xC

struct BRCTRLayoutSubHeader {
    u16 offsetToFirstVariant;
    u16 variantsCount;
    u16 offsetToFirstMessage;
    u16 messagesCount;
    u16 offsetToFirstPicture;
    u16 picturesCount;
};  // total size 0xC

struct BRCTRVariant {
    u16 offsetToName;
    u16 alpha;
    u16 isAnimated;
    u16 padding;
    float animationDelay;
    Vec3 translation43;
    Vec2 scale43;
    Vec3 translation169;
    Vec2 scale169;
    u16 firstMessageIndex;
    u16 messagesCount;
    u16 firstPictureIndex;
    u16 picturesCount;
};  // total size 0x3C

struct BRCTRMessage {
    u16 offsetToBRLYTPaneName;
    u16 offsetToName;
    u32 bmgId;
};  // total size 0x8

struct BRCTRPicture {
    u16 offsetToBRLYTPaneName;
    u16 offsetToSourceBRLYTPaneName;
};  // total size 0x4

struct BRCTRHeader {
    char magic[4];  // bctr
    u16 subHeaderCount;
    u16 offsetToBLYTName;
    u16 offsetToBMGName;
    u16 offsetToPictureBLYTName;
    u16 offsetToAnimSubHeader;
    u16 offsetToLayoutSubHeader;  // 0xe
    u16 offsetToNameTable;  // 0x10
    u8 padding[2];
};  // 0x14

typedef BRCTRHeader BRCTR;
#endif