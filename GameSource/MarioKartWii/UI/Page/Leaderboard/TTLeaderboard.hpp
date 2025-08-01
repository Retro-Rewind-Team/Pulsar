#ifndef _TT_LEADERBOARD_
#define _TT_LEADERBOARD_
#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Page.hpp>
#include <MarioKartWii/UI/Page/Leaderboard/GPVSLeaderboardUpdate.hpp>
#include <MarioKartWii/System/Timer.hpp>

namespace Pages {
class TTLeaderboard : public GPVSLeaderboardUpdate {  // unsure why it inherits from VSLeaderboardUpdate, might be wrong and could be an intermediate class
   public:
    static const PageId id = PAGE_TT_LEADERBOARDS;  // ID 0x2E
    TTLeaderboard();  // 80625134
    ~TTLeaderboard() override;  // 8085e438 vtable 808daabc
    PageId GetNextPage() const override;  // 0x10 8085dc44
    void OnInit() override;  // 0x28 8085d564
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 0x60 8085e4b8
    void FillRows() override;  // 0x68 8085d78c
    int GetRowCount() const override;  // 0x70 8085d784 returns 6

    Timer fastLapPB;  // 0x1A8 fetched at 8085d5c8
    u32 staffGhostState;  // 1 = easy; 2 = expert unlocked 0x1B4
    LayoutUIControl ghostMessage;  // 0x1B8 "You've unlocked an Expert Staff Ghost Data record!"
};  // total size 0x32C
size_assert(TTLeaderboard, 0x32c);
}  // namespace Pages

// 8085d8d0 updates TTLeaderboardPage
#endif