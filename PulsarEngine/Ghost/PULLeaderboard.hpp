#ifndef _PUL_LEADERBOARD_
#define _PUL_LEADERBOARD_
#include <kamek.hpp>
#include <MarioKartWii/RKSYS/RKSYSMgr.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace Ghosts {
// Implements a leaderboard file; it is divided in one sub leaderboard per TT modes which holds 11 entries (top 10 + flap)

enum EntryLaps {
    ENTRY_1ST,
    ENTRY_2ND,
    ENTRY_3RD,
    ENTRY_4TH,
    ENTRY_5TH,
    ENTRY_6TH,
    ENTRY_7TH,
    ENTRY_8TH,
    ENTRY_9TH,
    ENTRY_10TH,
    ENTRY_FLAP
};

struct PULLdbEntry {
    PULLdbEntry() {
        minutes = 0;
        seconds = 0;
        milliseconds = 0;
        rkgCRC32 = 0;
    }
    RFL::StoreData mii;
    u32 rkgCRC32;  // 0x4C
    u16 minutes;  // 0x50
    u8 seconds;  // 0x52
    u8 padding2[1];
    u16 milliseconds;  // 0x54
    bool isActive;  // 0x56
    u8 padding3[1];
    CharacterId character;  // 0x58
    KartId kart;  // 0x5C
    ControllerType controllerType;  // 0x60
};  // total size 0x64
size_assert(PULLdbEntry, 0x64);

// Should be fine having "affecting" functions public as this class can only be access through a const getter in manager (or from manager itself)
class alignas(0x20) Leaderboard {
    static const u32 fileMagic = 'PULL';
    static const u32 curVersion = 1;
    static const u32 trackNameLen = 48;  // 0x30
    static const char filePathFormat[];

   public:
    Leaderboard();
    Leaderboard(const char* folderPath, PulsarId id, bool createNew);
    void SetTrack(PulsarId id);
    // void SwapEntries(LeaderboardEntry *entry1, LeaderboardEntry *entry2);
    s32 GetPosition(const Timer& other) const;
    s8 GetRepeatCount(const RKG& rkg) const;
    void Update(u32 position, const RKSYS::LicenseLdbEntry& entry, u32 rkgCRC32);
    void Save(const char* folderPath);
    void AddTrophy() { this->hasTrophy[System::sInstance->ttMode] = true; }
    const PULLdbEntry& GetPulEntry(EntryLaps lap) const { return this->entries[System::sInstance->ttMode][lap]; }
    void EntryToTimer(Timer& dest, u8 id) const;
    void EntryToGameEntry(RKSYS::LicenseLdbEntry& dest, u8 id) const;
    void SetFavGhost(u32 fileIdx, TTMode mode, bool add);
    const char* GetFavGhost(TTMode mode) const { return this->favGhost[mode]; }
    static void CreateFile(PulsarId id);
    static const RKSYS::LicenseLdbEntry* GetEntry(u32 index);  // pointer as the game expects as such
    static int ExpertBMGDisplay();

   private:
    u32 magic;  // PULL
    u32 version;
    u32 crc32;  // of the track
    char name[trackNameLen];  // 0xC
    bool hasTrophy[4];  // 0x3c
    u8 reserved[0x10];
    PULLdbEntry entries[4][11];  // 0x50 11th = flap
    char favGhost[4][0x10];
};
static_assert(sizeof(Leaderboard) % 0x20 == 0, "Leaderboard Size Check");

PULLdbEntry* GetPULLdbEntry(u32 index);

}  // namespace Ghosts
}  // namespace Pulsar

#endif