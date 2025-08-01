#ifndef _PLAYER_EFFECTS_
#define _PLAYER_EFECTS_
#include <kamek.hpp>
#include <core/egg/Effect/Effect.hpp>
#include <MarioKartWii/Kart/KartPlayer.hpp>

namespace Effects {
class Player {
   public:
    struct Color : nw4r::ut::Color {
        Color(int r, int g, int b, int a);  // 806ae36c
        Color();  // 8068e844
        ~Color();  // 8068e848
        Color& operator=(Color& other);  // 806ae380
    };

    static Racedata* GetRacedata();  // 8068e830
    static CourseId* GetCourseId(Racedata* racedata);  // 8068e83c

    explicit Player(Kart::Player* kartPlayer);  // 8068df98
    virtual ~Player();  // 8068e888 vtable 808c1f64

    void Init();  // 8068e9a8
    void Reset();  // 8068ea58
    void LoadEffects();  // 8068f044 4932 lines long
    void KillEffects(bool killAll);  // 806958f4 certains effects are not killed (see func) if bool is false
    void Update();  // 80693d84 updates everything (positions but also effects)
    void UpdateValues();  // 80693e94 position, scale, speed, collision, matrixes are all stored/calc'd

    void UpdateNonItemEffects();  // 80695ba8

    void CreateAndUpdateEffectsByIdx(EGG::Effect** effectsArray, u32 firstEffectIndex, u32 lastEffectIndex,
                                     const Mtx34& playerMat2, const Vec3& wheelPos, bool updateScale);  // 806975d4
    void CreateEffectsByIdx(EGG::Effect** effectsArray, u32 firstEffectIndex, u32 lastEffectIndex);  // 8069797c
    void UpdateEffectsByIdx(EGG::Effect** effectsArray, u32 firstEffectIndex, u32 lastEffectIndex,
                            const Mtx34& playerMat2, const Vec3& wheelPos, bool r9);  // 806979f0
    void FollowFadeEffectsByIdx(EGG::Effect** effectsArray, u32 firstEffectIndex, u32 lastEffectIndex,
                                const Mtx34& playerMat2, const Vec3& wheelPos, bool updateScale);  // 80697788
    void KillEffectsByIdx(EGG::Effect** effectsArray, u32 firstEffectIndex, u32 lastEffectIndex);

    void FollowFadeEffectsByIdxVelocity(EGG::Effect** effectsArray, u32 firstEffectIndex, u32 lastEffectIndex, u32 _7ecIdx);  // 806a1b68
    void CreateAndUpdateEffectsByIdxVelocity(EGG::Effect** effectsArray, u32 firstEffectIndex, u32 lastEffectIndex, u32 _7ecIdx);  // 806a19d8
    void UpdateKartEffects();  // 80698028
    void UpdateBikeEffects();  // 80698f70

    void UpdateVehicleEffects();  // 80697ffc

    void UpdateFromWheelCollision(u16 wheelIdx, KCLBitfield kclBitfield, u32 kclFlag);  // 806a0fdc
    void UpdateFromKartCollision(u16 kartHitboxIdx, KCLBitfield kclBitfield, u32 kclFlag);  // 806a1660

    void OnBushCollision();  // 806a66a4 creates WeedWall effect, plays bushcollisionsound

    void CreateSpinEffect(Kart::Damage::SpinType spinType, u32 r5);  // 806a67f8
    // Create = called when the effect should be drawn
    // Update = called every frame and checks for whether the effect should be created
    void UpdateCollisionEffects();  // 80697be0 the ones below
    void TriggerPochaEffect();  // 806ab8a0
    void CreatePochaEffect(float height);  // 806abb5c
    void CreateSandEffect(u32 colorType);  // 806adef4
    void CreateGrassEffect(u32 colorType);  // 806ae58c
    void CreateSnowDirtEffect(u32 colorType);  // 806aeac8
    void CreateSnowRoadEffect(u32 colorType);  // 806af038
    void CreateWaterEffect(u32 colorType);  // 806af768
    void CreateMudEffect(u32 colorType);  // 806afc04
    void CreateFlowerEffect(u32 colorType);  // 806b0098
    void CreateStoneEffect(u32 colorType);  // 806b04bc

    void SetOffroadColors();  // 806b171c

    void UpdateInkEffect();  // 8069fcf0 checks for Kart::Status bitfield ink applied

    void Toggle(bool isDisabled);  // 80693d58
    void CalcPlayerEffects();  // 8067cca0

    void UpdateGasSmokeEffects();  // 8069baf8
    void UpdateStartUpEffects();  // 8069b394 the smoke under back wheels when you press A during coutndown
    void UpdateJumpSmokeEffects();  // 80699cb0
    void UpdateSlipStreamEffects();  // 8069b748
    void UpdateWaterEffects();  // 8069c7f0

    void CreateKartBrakeBackwardsEffects();  // 806a8268 bool 0x44
    void KillKartBrakeBackwardsEffects();  // 806a837c

    void CreateBikeBrakeBackwardsEffects();  // 806a8588 bool 0x44
    void KillBikeBrakeBackwardsEffects();  // 806a867c

    // not to be confused with Effects::Item, this solely refers to effects AROUND the player
    bool UpdateInBillEffects();  // 8069d120 bill effect, thunder on bill, fib if bill collides etc... returns true if player in bill
    void UpdatePowEffect();  // 806aa530
    void UpdatePow1Effect();  // 806aa784
    void CreateRKKiller(const Vec3& mtx);  // 806a8f2c
    void UpdateRKKiller();  // 806a9070 RKKiller_1T is inlined in updatebilleffects
    void UpdateThunderEffects();  // 8069dba4
    void UpdateNiseBoxBiriEffect();  // 806aaf04
    void CreateRKHitEffect(const Vec3& pos);  // 806a65ec

    void CreateStarEffect();  // 806a8ad4 also updates it
    void UpdateStarEffect();  // 806a8cb0 updates pos for when not in a star
    void UpdateItemEffect();  // 80694d0c star, thunder, pow, itembox, fib, mega, blooper

    void Create25CEffect();  // 8069e8ac
    void Create228Effect();  // 8069eb5c

    void SetGessoEffectsVisibility(bool isVisible);  // 806b3698
    void UpdateGessoEffectsVisibility();  // 806b37cc
    bool automaticDisable;  // 0x4 playerVanished, clipped off etc...
    bool isDisabled[3];  // 0x5 idk why 3

    bool was0x7TruePrev;  // 0x8
    bool unknown_0x9;
    bool was0x9TruePrev;  // 0xa

    bool hasMultipleScreens;  // 0xb ScnManager screenCount if <2 then false
    bool has3OrMoreScreens;  // 0xc
    bool isInsideDrift;  // 0xd
    bool isAutomaticDrift;  // 0xe
    bool isHumanPlayer;  // 0xF

    bool isCPUOrRealOnlineANDhasMultipleScreens;  // 0x10
    bool isLocalAndHas2Screens;  // 0x11
    bool isCPUOrRealOnlineANDHas2Screens;  // 0x12
    bool isLocalAndHas3OrMoreScreens;  // 0x13
    bool isCPUOrRealOnlineANDHas3OrMoreScreens;  // 0x14

    bool hasGottenStartBoost;  // 0x15
    u16 frameTimer;  // 0x16 up to 1000, seems to only trigger for certain effects 0x16
    u16 startBoostDuration;  // 0x18
    u16 boostDuration;  // 0x1A
    u32 frameTimer2;  // same, 0x1C
    bool isBoosting;  // 0x20 from kartStatus
    bool isOnBoostRamp;  // 0x21
    bool isOnJumpPad;  // 0x22 same
    bool hasMTBoost;  // 0x23
    bool isOverZipper;  // 0x24
    u8 unknown_0x25[0x2B - 0x25];  // 8068eb04 same as above but 1 frame delayed most likely
    bool hasMushroomBoost;  // 0x2B
    u8 unknown_0x2C[2];  // 8068eb04
    bool isInExplosion;  // 0x2E
    bool unknown_0x2F;
    u8 unknown_0x30;
    u8 unknown_0x31;  // 8068eb2c
    u8 unknown_0x32[0x34 - 0x32];
    u32 smtCharge;  // 0x34 80698650
    u8 unknown_0x38;  // mess at 8068eec4
    u8 unknown_0x39;  // mess at 8068eec4
    u8 unknown_0x3A;
    u8 unknown_0x3B;
    u16 unknown_0x3C;  // set to 0x3
    u16 unknown_0x3E;  // set to 0x28
    u16 unknown_0x40;  // 80694b58

    bool isGoingBackwards;  // 0x42
    bool unknown_0x43;  // bitfield 1 0x2000000
    bool isBrakingAndBackwards;  // 0x44
    bool unknown_0x45;  // 8068eb50
    bool isInStar;  // 0x46
    bool isInMega;  // 0x47
    bool hasTC;  // 0x48
    bool isSquishedByNotACar;  // 0x49
    u8 unknown_0x4A[0x4C - 0x4A];
    u32 unknown_0x4C;  // set to 0x2b2 8068ec18
    u32 unknown_0x50;  // set to 0x21C
    u32 frameTimer3;  // 0x54
    u32 frameTimer4;  // 0x58
    bool isInCannon;  // 0x5C
    u8 unknown_0x5D[0x60 - 0x5D];  // padding?
    u32 unknown_0x60;  // set to 0x23 8068eb54
    u32 unknown_0x64;  // set to -1
    u8 unknown_0x68;  // 806950d0
    u8 unknown_0x69;
    u8 unknown_0x6A;
    u8 unknown_0x6B;  // 8068eb5c
    u8 unknown_0x6C;  // 8068eb5c
    u8 unknown_0x6D[3];
    float unknown_0x70;  // 8068eb64
    float initialYPos;  // 0x74
    u8 unknown_0x78;
    u8 unknown_0x79;
    u8 unknown_0x7A;
    u8 unknown_0x7B;
    u8 unknown_0x7C;
    bool isInBullet;  // 0x7D
    bool wasInBulletPrevFrame;  // 0x7E
    u8 unknown_0x7F;  // padding?
    u32 unknown_0x80;  // 8068eb80
    u32 unknown_0x84;
    bool unknown_0x88;  // bitfield 1 0x20000000
    bool unknown_0x89;  // related to MT sparks 8069bad4
    u8 unknown_0x8A[0x8C - 0x8A];
    u32 unknown_0x8C;  // set to -1
    u8 unknown_0x90;  // 8068eb94
    u8 unknown_0x91;
    u8 unknown_0x92;
    u8 unknown_0x93;
    u8 unknown_0x94;
    u8 unknown_0x95;
    bool isInPreRespawn;  // 0x96
    bool isInPreRespawn1framedelayed;  // 0x97
    u8 unknown_0x98;  // 80694528
    u8 unknown_0x99[0xA0 - 0x99];
    u8 unknown_0xA0[0xA8 - 0xA0];
    u8 unknown_0xA8;  // 8068ebc4
    u8 unknown_0xA9;
    bool isManualDrifting;  // 0xAA
    bool unknown_0xAB;  // 80694ad8
    u8 unknown_0xAC;
    u8 unknown_0xAD;
    bool hasTrickBoost;  // 0xAE
    u8 unknown_0xAF;  // 8069abcc
    u8 unknown_0xB0;  // 806b3520
    u8 unknown_0xB1;
    u8 unknown_0xB2;
    u8 unknown_0xB3;
    float unknown_0xb4;
    u8 unknown_0xB8;
    bool hasHitItem;  // 0xB9
    u8 unknown_0xBA[0xC0 - 0XBA];
    bool unknown_0xC0;  // related to cameras, 80694568
    u8 unknown_0xC1[0xc4 - 0xC1];
    Mtx34 playerMat2;  // might be one frame delayed, copy of the one at 6f4 80694c6c
    u8 unknown_0xF4;  // 806945bc
    bool hasVanished;  // 80693dc0
    u8 unknown_0xF6;  // 80694660 related to cameras
    u8 unknown_0xF7[0x100 - 0xF7];
    float float_0x100;  // 8069ec94
    float float_0x104;
    float float_0x108;
    u8 unknown_0x10C;  // 8069eca8
    u8 unknown_0x10D;
    bool isAcceleratingAtStart;  // 0x10e
    u8 unknown_0x10F[0x114 - 0x10f];

    u8 playerCountCategory;  // 0x114 https://decomp.me/scratch/QTM51 playerCount / 3
    u8 playerCountCategoryParam;  // 0x115 (playerCount / 3) * 2 + (playerCount % 3 == 2 ? 1 : 0)
    u8 playerCount;  // 0x116
    u8 padding2;
    Kart::Player* kartPlayer;  // 0x118
    KartId kartId;  // 0x11c
    CharacterId characterId;  // 0x120
    u32 isBike;  // 0x124 yes it's a word
    bool isBlueFalcon;  // 0x128
    bool isLocal;  // 0x129
    bool isCPUOrRealOnline;  // 0x12a
    bool isGhost;  // 0x12b
    bool isTTReplay;  // 0x12c
    bool isRealOrNotGhost;  // 0x12D
    u8 playerId;  // 0x12e
    u8 wheelCount;  // 0x12f
    u8 unknown_0x130[0x134 - 0x130];
    u32 playerIdPlus2;  // 0x134 0x8068f064
    bool isCPURace;  // 0x138
    bool isReplay;  // 0x139
    bool isGameType2or3or4;  // 0x13a 8068e0b0
    bool isLiveView;  // 0x13B
    bool isCompetition;  // 0x13C
    bool isVsRace;  // 0x13D
    bool isDKSC;  // 0x13e
    bool isGV;
    bool isMT;
    bool isMG;
    bool isDDR;
    bool isDKM;
    bool isDC;
    bool isCM;
    bool isTF;
    bool isBC64;
    bool isBC3;
    bool isDKJP;
    bool isDelfinoPier;
    bool isGalaxyColosseum;
    bool hasPocha;  // 0x14C
    bool hasPochaYogan;
    bool hasEntry;
    bool hasEpropeller;  // 0x14F
    bool hasPseaOrVenice_Nami;  // 0x150
    bool isSL;  // 0x151
    bool hasEnvFire;  // 0x152
    bool hasKareha;
    bool isRedTeam;  // 0x154
    bool isBlueTeam;  // 0x155
    bool isBattle;  // 0x156
    bool effectHolder_0x1A;  // 0x157
    ModelDirector* pochaModel;  // 0x158
    ModelDirector* pochaYoganModel;  // 0x15C
    ModelDirector* iceModel;  // for rSL 0x160
    u32 unknown_0x164[12];  // 0x164
    Color offroadColors[16];  // 0x194 sand is 0
    Color offroadColors2[4];  // 0x1d4 graess is 0th
    Color class_0x1e4[2];
    Color class_0x1ec[1];

    EGG::Effect* rk_raceEffects[92];  // 0x1f0 use enum

    bool hasLoaded_1[0x38c - 0x360];  // 0x360 check from crash to hitA
    bool hasLoaded_2[0x39A - 0x38C];  // rk_koukasen to rk_effect_0x2D4
    bool hasLoaded_3[0x3AA - 0x39A];  // 806b0e50 check from gasSmoke to miniTurbo_4
    bool hasLoaded_4[0x3BC - 0x3AA];  // 806b0ec8 check from dirtSmoke to stone2

    float unk_emission_value_0x3BC[44];  // related from crash to hitA ends at 46c, corresponds to unknown emission value on the wiki
    float unk_emission_value_0x46C[14];  // 0x46C from rk_koukasen to rk_effect_0x2D4,
    float unk_emission_value_0x4A4[16];  // 806b0e5c related to gasSmoke ends at 4E4
    float unk_emission_value_0x4E4[18];  // related to dirtSmoke to stone2 ends at 52c

    EGG::Effect** kartEffects;  // 0x52c 8069188c 8 elements use kartEffects enum
    EGG::Effect** kartDriftEffects;  // 0x530 8069189c 36 elements use driftEffects enum
    bool hasLoaded_5[8];  // 0x534 8 elements in the 52c array
    bool hasLoaded_6[36];  // 0x53C 36 elements in the 530 array
    float unk_emission_value_0x560[8];  // ends at 580
    float unk_emission_value_0x580[36];  // ends at 610
    EGG::Effect** bikeEffects;  // 0x610
    EGG::Effect** bikeDriftEffects;  // 0x614
    bool hasLoaded_7[8];  // 0x618 for the array above 600
    bool hasLoaded_8[28];  // 0x620, for the array at 614
    float unk_emission_value_0x63C[8];  // 0x63C ends at 65c

    float unk_emission_value_0x65C[28];  // ends at 0x6CC, for the array at 614
    ExhaustParticleSet* exhaustParticleSet2;  // 0x6cc
    Vec3 playerPosition;  // 0x6d0 from kartPhysics 80693eb0
    Vec3 speed;  // 0x6dc from kartPhysics
    float speedNorm;  // 0x6e8 calculed from speed vector
    float prevSpeedNorm;  // 0x6ec
    float softSpeedLimitOr50;  // 0x6f0, set to 50.0 if softSpeedLimit inferior to it
    Mtx34 playerMat;  // 0x6f4 from kartPhysicsHolder
    u32 screenIdx;  // 0x724
    float engineSpeed;  // 0x728
    Vec3 playerScale;  // 0x72c from kartMovement
    Vec2 unknown_0x738;  // 806b3674
    Vec3 lastWheelfloorNor;  // 0x740 4th wheel for karts 80696eac, from wheelPhysics collision data
    Mtx34 unknown_0x74C;
    Vec3 unknown_kartParam;  // from 0x6CC field 8068ef9c
    Vec3 unknown_kartParam2;  // from 0x6CC field
    u8 unknown_0x794[0x79C - 0x794];
    u32 unknown_0x79C;  // set to C8 8068ed88
    u16 driftState;  // from kartMovement 7a0, delayed by one frame so that on the transition stuff happens
    u8 unknown_0x7A2[2];
    Vec3 hitboxPosition;
    u32 unknown_0x7B0;  // 8068ed90
    u8 unknown_0x7B4[0x7B9 - 0x7B4];  // all incremented until 0x64 at 806947ac
    bool isSecondWheelOnFloor;  // 0x7b9
    bool isThirdWheelOnFloor;  // 0x7ba
    Vec3 backWheelPos[2];  // 0x7bc from WheelPhysics, set at 80694040

    Vec3 prevBackWheelPos[2];  // 0x7D4 might be 1 frame delayed as it's a copy of 0x7BC

    Vec3 diffWheelPositions[2];  // 0x7ec 7d4 - 7bc
    float distanceBackWheels1Frame[2];  // 0x804 norm of difference vector
    Vec3 vector_0x80c;  // 8068ee20
    Mtx34 mat_0x818;  // 806b426 and 0x824, 834, 844 are x y z playerPos
    bool bool_0x848[4];  // related to params at 0x8cc 8068ee8c

    Vec3 vector_0x84C[4];  // 806a0e38 related to params
    Vec3 vector_0x84cPrev[4];  // 0x87c

    Mtx34 boostMat[4];  // 0x8ac
    bool isAccelerating;  // 0x96C
    u8 unknown_0x96D[0x971 - 0x96D];  // 80694960
    u8 unknown_0x971[3];
    float float_0x974[2];
    Vec3 vector_0x97C;  // 806b1340
    u16 unknown_0x988[3];  // 806b1390
    u8 unknown_0x98E[2];
    float float_0x990;
    u16 unknown_0x994[7];  // ends at 9a2
    u8 unknown_0x9B0[0x9B0 - 0x9A2];
    float float_0x9B0[14];  // 806b146c ends at 9e8
    u8 unknown_0x9E8[0x9F0 - 0x9e8];
    float float_0x9F0[22];  // ends at 0xA48
    u8 unknown_0xA48[0xA4C - 0xA48];
    u8 unknown_0xA4C;  // 806b159c
    u8 unknown_0xA4D[0xA50 - 0xA4D];
    float unknown_0xA50[5];
    u8 unknown_0xA64[0xA68 - 0xA64];
    float float_0xA68[6];
    u8 unknown_0xA80[0xA84 - 0xA80];  // 806b1670
    float float_0xA84[6];  // 806b16d0 ends at A9C
    u8 unknown_0xA9C[0xAAC - 0xA9C];
    float float_0xAAC[2];
    u16 unknown_0xAB4[3];  // 8068ed44
    u8 unknown_0xABA[2];
    float float_0xABC[5];  // 8068ec2c
    u16 unknown_0xAD0[5];  // 806b11bc
    u8 unknown_0xADA[0xAE0 - 0xADA];
    u8 unknown_0xAE0;  // 8068eb88
    u8 unknown_0xAE1[3];
    u32 unknown_0xAE4;  // 8068ebc8
};  // total size 0xAE8
size_assert(Player, 0xae8);
#endif
}  // namespace Effects
