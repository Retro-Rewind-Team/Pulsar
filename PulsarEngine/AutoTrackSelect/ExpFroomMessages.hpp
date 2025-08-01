#ifndef _EXPFROOMMESSAGES_
#define _EXPFROOMMESSAGES_
#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Other/FriendRoom.hpp>

namespace Pulsar {
namespace UI {

class ExpFroomMessages : public Pages::FriendRoomMessages {
   public:
    static bool isOnModeSelection;
    static s32 clickedButtonIdx;
    void OnModeButtonClick(PushButton& button, u32 hudSlotId);
    void OnCourseButtonClick(PushButton& button, u32 hudSlotId);
};

}  // namespace UI
}  // namespace Pulsar

#endif
