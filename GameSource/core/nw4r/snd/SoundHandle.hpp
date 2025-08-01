#ifndef _NW4R_SNDSOUNDHANDLE_
#define _NW4R_SNDSOUNDHANDLE_
#include <types.hpp>

namespace nw4r {
namespace snd {
namespace detail {
class BasicSound;
}
class SoundHandle {
   public:
    void detail_AttachSound(detail::BasicSound* sound);  // 800a2e30
    void detail_AttachSoundAsTempHandle(detail::BasicSound* sound);  // 800a2db0
    detail::BasicSound* basicSound;

};  // 0x4
}  // namespace snd
}  // namespace nw4r
#endif
