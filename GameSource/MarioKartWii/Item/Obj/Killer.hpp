#ifndef _ITEMOBJBANANA_
#define _ITEMOBJBANANA_
#include <kamek.hpp>
#include <MarioKartWii/Item/Obj/ItemObj.hpp>

//_sinit_ at 807a9c98
namespace Item {
class ObjKiller : public ObjMiddle {  // bullet
   public:
    static Obj** CreateArray(u32 count);  // 807a9a80
    void InitSelf() override;  // 0x8 807a9b40 vtable 808d1f00
    void UpdateModelPosition() override;  // 0x10 807a9b5c
};
size_assert(ObjKiller, 0x1a0);
}  // namespace Item

#endif