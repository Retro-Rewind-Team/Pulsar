#ifndef _NW4R_G3D_COMMON_
#define _NW4R_G3D_COMMON_
#include <types.hpp>
#include <core/rvl/MEM/MEMallocator.hpp>

namespace nw4r {
namespace g3d {
typedef MEM::Allocator G3dHeap;
namespace detail {
void ZeroMemory32ByteBlocks(register void* dst, u32 size);  // 80060f20
}  // namespace detail
}  // namespace g3d
}  // namespace nw4r

#endif