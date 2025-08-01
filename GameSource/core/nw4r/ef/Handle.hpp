#ifndef _NW4R_EF_HANDLE_
#define _NW4R_EF_HANDLE_
#include <types.hpp>
#include <core/nw4r/ef/LinkedObject.hpp>

namespace nw4r {
namespace ef {

struct HandleBase {
    HandleBase();  // 80037560
    HandleBase(const HandleBase& hnd);  // 80037570
    LinkedObject* GetPtr() const;  // 800375f0
    bool IsValid() const;  // 800375c0
    HandleBase& operator=(const HandleBase& other);  // 80037570
    HandleBase& operator=(LinkedObject* obj);  // 80037590
    u32 objId;
    LinkedObject* object;
};  // total size 0x8

template <class T>
class Handle : public HandleBase {};

}  // namespace ef
}  // namespace nw4r
#endif