#ifndef __KAMEK_BASE_HOOKS_H
#define __KAMEK_BASE_HOOKS_H
#include <types.hpp>
// allow Kamek hooks to be defined from C++ source files
#pragma section ".kamek"

// hook type IDs _must_ match what's in the Kamek source!
#define kctWrite 1
#define kctConditionalWrite 2
#define kctRegionWrite 3
#define kctInjectBranch 4
#define kctInjectCall 5
#define kctRegionInjectBranch 6
#define kctRegionInjectCall 7
#define kctPatchExit 8

#define kmIdentifier(key, counter) \
    _k##key##counter
#ifndef __INTELLISENSE__
#define kmHookInt(counter) \
    __declspec(section ".kamek") static const unsigned int kmIdentifier(Hook, counter)
#else
#define kmHookInt(counter) \
    static const unsigned int kmIdentifier(Hook, counter)
#endif

// general hook definition macros
// TODO: debugging data (file, line, ...) for diagnostic use by Kamek maybe? :3
#define kmHook0(type) \
    kmHookInt(__COUNTER__)[2] = {0, (type)}
#define kmHook1(type, arg0) \
    kmHookInt(__COUNTER__)[3] = {1, (type), (unsigned int)(arg0)}
#define kmHook2(type, arg0, arg1) \
    kmHookInt(__COUNTER__)[4] = {2, (type), (unsigned int)(arg0), (unsigned int)(arg1)}
#define kmHook3(type, arg0, arg1, arg2) \
    kmHookInt(__COUNTER__)[5] = {3, (type), (unsigned int)(arg0), (unsigned int)(arg1), (unsigned int)(arg2)}
#define kmHook4(type, arg0, arg1, arg2, arg3) \
    kmHookInt(__COUNTER__)[6] = {4, (type), (unsigned int)(arg0), (unsigned int)(arg1), (unsigned int)(arg2), (unsigned int)(arg3)}

// kmCondWrite
//   Write value to address, conditionally
#define kmCondWritePointer(addr, original, value) kmHook4(kctConditionalWrite, 1, (addr), (value), (original))
#define kmCondWrite32(addr, original, value) kmHook4(kctConditionalWrite, 2, (addr), (value), (original))
#define kmCondWrite16(addr, original, value) kmHook4(kctConditionalWrite, 3, (addr), (value), (original))
#define kmCondWrite8(addr, original, value) kmHook4(kctConditionalWrite, 4, (addr), (value), (original))

// kmWrite
//   Write value to address
#define kmWritePointer(addr, ptr) kmHook3(kctWrite, 1, (addr), (ptr))
#define kmWrite32(addr, value) kmHook3(kctWrite, 2, (addr), (value))
#define kmWrite16(addr, value) kmHook3(kctWrite, 3, (addr), (value))
#define kmWrite8(addr, value) kmHook3(kctWrite, 4, (addr), (value))
#define kmWriteNop(addr) kmWrite32(addr, 0x60000000)

// kmRegionWrite 'E', 'P', 'J', 'K'
#define kmRegionWritePointer(addr, ptr, region) kmHook4(kctRegionWrite, 1, (addr), (ptr), (region))
#define kmRegionWrite32(addr, value, region) kmHook4(kctRegionWrite, 2, (addr), (value), (region))
#define kmRegionWrite16(addr, value, region) kmHook4(kctRegionWrite, 3, (addr), (value), (region))
#define kmRegionWrite8(addr, value, region) kmHook4(kctRegionWrite, 4, (addr), (value), (region))

// kmRegionWrite 'E', 'P', 'J', 'K'
#define kmRegionBranch(addr, ptr, region) kmHook3(kctRegionInjectBranch, (addr), (ptr), (region))
#define kmRegionCall(addr, ptr, region) kmHook3(kctRegionInjectCall, (addr), (ptr), (region))

// kmPatchExitPoint
//   Force the end of a Kamek function to always jump to a specific address
//   (if the address is 0, the end remains as-is (i.e. blr))
#define kmPatchExitPoint(funcStart, dest) kmHook2(kctPatchExit, (funcStart), (dest))

// kmBranch, kmCall
//   Set up a branch from a specific instruction to a specific address
#define kmBranch(addr, ptr) kmHook2(kctInjectBranch, (addr), (ptr))
#define kmCall(addr, ptr) kmHook2(kctInjectCall, (addr), (ptr))

// kmBranchDefCpp, kmBranchDefAsm
//   Set up a branch (b) from a specific instruction to a function defined
//   directly underneath. If exitPoint is not nullptr, the function will
//   branch to exitPoint when done; otherwise, it executes blr as normal
#define kmBranchDefInt(counter, addr, exitPoint, returnType, ...)   \
    static returnType kmIdentifier(UserFunc, counter)(__VA_ARGS__); \
    kmBranch(addr, kmIdentifier(UserFunc, counter));                \
    kmPatchExitPoint(kmIdentifier(UserFunc, counter), exitPoint);   \
    static returnType kmIdentifier(UserFunc, counter)(__VA_ARGS__)

#define kmBranchDefCpp(addr, exitPoint, returnType, ...) \
    kmBranchDefInt(__COUNTER__, addr, exitPoint, returnType, __VA_ARGS__)
#define kmBranchDefAsm(addr, exitPoint) \
    kmBranchDefInt(__COUNTER__, addr, exitPoint, asm void, )

// kmCallDefCpp, kmCallDefAsm
//   Set up a branch with link (bl) from a specific instruction to a function
//   defined directly underneath.
#define kmCallDefInt(counter, addr, returnType, ...)                \
    static returnType kmIdentifier(UserFunc, counter)(__VA_ARGS__); \
    kmCall(addr, kmIdentifier(UserFunc, counter));                  \
    static returnType kmIdentifier(UserFunc, counter)(__VA_ARGS__)

#define kmCallDefCpp(addr, returnType, ...) \
    kmCallDefInt(__COUNTER__, addr, returnType, __VA_ARGS__)
#define kmCallDefAsm(addr) \
    kmCallDefInt(__COUNTER__, addr, asm void, )

#define kmOnLoad(func) \
    static int kmIdentifier(Int, counter) = func()

#define kmOnLoadDefInt(counter, returnType)              \
    static returnType kmIdentifier(UserFunc, counter)(); \
    kmOnLoad(kmIdentifier(UserFunc, counter));           \
    static returnType kmIdentifier(UserFunc, counter)()

#define kmOnLoadDefCpp() \
    kmOnLoadDefInt(__COUNTER__, int)

#define kmOnLoadDefAsm() \
    kmOnLoadDefInt(__COUNTER__, asm int)

#define kmWrite24(address, instruction)                  \
    kmWrite16(address, (instruction & 0xFFFF00) >> 0x8); \
    kmWrite8(address + 0x2, instruction & 0xFF);

#endif
