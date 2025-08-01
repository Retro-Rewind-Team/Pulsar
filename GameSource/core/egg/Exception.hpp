#ifndef _EGG_EXCEPTION_
#define _EGG_EXCEPTION_
#include <types.hpp>
#include <core/nw4r/db.hpp>
#include <core/egg/mem/Heap.hpp>

namespace EGG {
class Exception {
   public:
    static Exception* sInstance;  // 80386E38
    static nw4r::db::detail::ConsoleHead* console;  // 80386E3C
    static Exception* Create(u16 consoleWidth, u16 consoleHeight, u16 attributes, Heap* heap, u32 r7);  // 80226734
    Exception(u16 consoleWidth, u16 consoleHeight, u16 attributes, Heap* heap, u32 r8);
    virtual ~Exception();  // 80226800 vtable 802a2fe8
    static void SetUserCallback(void* args);  // 802267f0
};

bool ExceptionCallback(nw4r::db::detail::ConsoleHead* console, void* arg);  // 80226464

}  // namespace EGG
#endif