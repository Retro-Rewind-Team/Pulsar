#ifndef _GPVS_LEADERBOARDTOTAL_PAGE_
#define _GPVS_LEADERBOARDTOTAL_PAGE_
#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Page.hpp>
#include <MarioKartWii/UI/Page/Leaderboard/GPVSLeaderboardUpdate.hpp>

//_sinit_ at 8085e644
namespace Pages {
class GPVSLeaderboardTotal : public GPVSLeaderboardUpdate {
   public:
    static const PageId id = PAGE_GPVS_TOTAL_LEADERBOARDS;  // ID 0x30
    GPVSLeaderboardTotal();  // 806252a8
    ~GPVSLeaderboardTotal() override;  // 8085e540 vtable 808daba4
    PageId GetNextPage() const override;  // 0x10 8085cc40
    void OnInit() override;  // 0x28 8085c9b0
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 0x60 8085e5b0
    void FillRows() override;  // 0x68 8085cae8

    static int ComparePlayers(const Player* first, const Player* second);  // 8085cc84 qsort func
};  // total size 0x1A8
size_assert(GPVSLeaderboardTotal, 0x1A8);
}  // namespace Pages
#endif