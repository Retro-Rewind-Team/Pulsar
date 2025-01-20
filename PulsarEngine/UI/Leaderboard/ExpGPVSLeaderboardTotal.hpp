#ifndef _EXP_GPVS_LEADERBOARD_TOTAL_
#define _EXP_GPVS_LEADERBOARD_TOTAL_
#include <kamek.hpp>
#include <UI/UI.hpp>
#include <MarioKartWii/UI/Page/Leaderboard/GPVSLeaderboardTotal.hpp>

// Extends Leaderboard to add the ability to toggle between displaying times and names
namespace Pulsar {
namespace UI {

class ExpGPVSLeaderboardTotal : public Pages::GPVSLeaderboardTotal  {
public:
    void OnUpdate() override;
};


}//namespace UI
}//namespace Pulsar
#endif