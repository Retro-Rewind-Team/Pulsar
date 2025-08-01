#ifndef _RKNETSELECT_
#define _RKNETSELECT_
#include <kamek.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
// https://wiki.tockdom.com/wiki/MKWii_Network_Protocol/SELECT

namespace RKNet {
enum OnlineMode {
    ONLINEMODE_PUBLIC_VS = 0x1,
    ONLINEMODE_PUBLIC_BATTLE = 0x2,
    ONLINEMODE_PRIVATE_VS = 0x3,
    ONLINEMODE_PRIVATE_BATTLE = 0x4
};
// random probs for battle as it oris 0x8000
// 0 skip
// 1 80662290
// 2 80662290
// 3 80662588

struct SELECTPlayerData {
    u16 prevRaceRank;
    u16 sumPoints;
    u8 character;  // 0x4
    u8 kart;
    u8 courseVote;  // 0x6
    u8 starRank;  // 0x7
};  // total size 0x8
size_assert(SELECTPlayerData, 0x8);

struct SELECTPacket {
    static const u32 idx = 3;
    u64 timeSender;
    u64 timeReceived;
    SELECTPlayerData playersData[2];  // 0x10
    u32 selectId;  // 0x20
    u32 battleTypeAndTeams;  // 0x24
    // u32 teams : 24; //0x25 idk how to do an array of 2 bits variables
    u8 playerIdToAid[12];  // 0x28
    u8 winningCourse;  // 0x34
    u8 phase;  // 0x35, 0 prepare, 1 wait, 2 lottery
    u8 winningVoterAid;  // 0x36
    u8 engineClass;  // 0x37 none, 100, 150, mirror //total size 0x38
};
size_assert(SELECTPacket, 0x38);

class SELECTHandler {
   public:
    static SELECTHandler* sInstance;  // 809c2100 /!\ do not use within pulsar
    static SELECTHandler* CreateInstance(OnlineMode mode);  // 8065fe8c
    static void DestroyInstance();  // 8065ff60

    SELECTHandler();  // 8066076c inlined
    ~SELECTHandler();  // 806607f4

    void Update();  // 806602fc
    void InitPackets();  // 8065ffa4 sets datas to invalid/not selected etc... composed of a bunch of inlined functions
    u8 GetWinningVoter() const;  // 80660434
    CourseId GetWinningCourse() const;  // 80660450
    u32 GetOwnSelectId() const;  // 80660470 from sent packet
    u8 GetEngineClass() const;  // 8066048c
    u8 GetBattleType() const;  // 806604a8 returns 1 for coin
    u8* GetOwnPlayerIdToAid() const;  // 806604cc
    CharacterId GetCharacterId(u8 aid, u8 hudSlotId) const;  // 806604d4
    KartId GetkartId(u8 aid, u8 hudSlotId) const;  // 80660524
    CourseId GetCourseVote(u8 aid, u8 hudSlotId) const;  // 80660574
    SELECTPlayerData* GetPlayerData(u8 aid, u8 hudSlotId) const;  // 806605c4
    u8 GetStarRank(u8 aid, u8 hudSlotId) const;  // 8066060c
    u8 GetTeam(u8 aid, u8 hudSlotId) const;  // 80660654

    bool HasUnprocessedPackets() const;  // 8066068c
    bool IsPrepared() const;  // 80660710 checks if in phase 0
    void SetPlayerData(CharacterId character, KartId kart, CourseId courseVote, u8 hudSlotId, u8 starRank);  // 80660750

    void ResetSendPacket();  // 806608b4
    void ResetReceivedPackets();  // 80660a00
    void ResetPacket(SELECTPacket& packet);  // 80660af4 idk
    void ResetPlayerData(SELECTPlayerData& playerData);  // 80660bd0
    void HandleReceivedPacketTimes(u32 aid);  // 80660c3c
    bool IsBattle() const;  // 80660d2c
    bool IsTrackDecided() const;  // 80660d40 essentially checks if winningtrack != - 1
    bool EveryoneHasAccurateRaceSettings() const;  // 80660d5c
    bool EveryoneHasVoted() const;  // 80660dcc
    bool EveryoneHasAccurateAidPidMap() const;  // 80660e3c
    void ClearRH1ReceivedBuffers();  // 80660eb0
    void PrepareAndExportSELECTandUSER();  // 80660f1c
    void ImportNewPackets();  // 80661078
    void PrepareAndExportPacket(u32 destAid, u32 sendTimeHi, u32 sendTimeLo);  // 80661288
    void ProcessNewPacketVoting();  // 806614b0
    void DecideBattleTypeAndTeams();  // 80661934
    void GenerateOwnSELECTId();  // 80661a14
    void DecideEngineClass();  // 80661a5c
    void DecideTrack();  // 80661ce8
    void AllocatePlayerIdsToAids();  // 80662034
    bool HasMatchingPlayerIdToAid(const SELECTPacket& lhs, const SELECTPacket& rhs) const;  // 806621dc
    void DecidePublicTeams(u32& teams);  // 80662290
    void DecidePrivateTeams(u32& teams);  // 80662588

   public:
    OnlineMode mode;  // from page 0x90 OnInit SectionId Switch
    u32 unknown_0x4;

   private:
    SELECTPacket toSendPacket;  // 0x8
    SELECTPacket receivedPackets[12];  // 0x40
   public:
    u8 lastSentToAid;  // 0x2e0
    u8 unknown_0x2e4[7];
    u64 lastSentTime;  // 0x2e8
    u64 lastReceivedTimes[12];  // 0x2f0
    u64 delaysFromPredictedRecvTimes[12];  // 0x350
    u32 unknown_0x3b0[12];  // 0x3b0
    u32 hasNewSELECT;  // 0x3e0 bitflag
    u32 hasNewRACEHEADER_1;  // 0x3e4 bitflag
    u32 aidsWithAccurateRaceSettings;  // 0x3e8
    u32 aidsWithAccurateAidPidMap;  // 0x3ec
    u32 aidsThatHaveVoted;  // 0x3f0
    u8 unknown_0x3F4[4];

};  // total size 0x3F8
size_assert(SELECTHandler, 0x3F8);
}  // namespace RKNet

#endif