#ifndef _CTRLRACEGHOSTDIFFTIME_
#define _CTRLRACEGHOSTDIFFTIME_
#include <kamek.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceBase.hpp>

//_sinit_ at 807eea20
class CtrlRaceGhostDiffTime : public CtrlRaceBase {
   public:
    CtrlRaceGhostDiffTime() {};
    ~CtrlRaceGhostDiffTime() override;  // 807ee990 vtable 808d3c78
    void Init() override;  // 0xc 807ee494
    void InitSelf() override;  // 0x18 807ec6e8
    void OnUpdate() override;  // 0x1c 807ee548
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 0x28 807eea14
    const char* GetClassName() const override;  // 0x2c 807ee488
    void Load(const char* variant);  // 807ee8e0
    static Timer AddTimers(const Timer& first, const Timer& second);  // 807ee7c0
    static Timer SubtractTimers(const Timer& minuend, const Timer& subtrahend);  // 807ee860 2 - 1

    u8 unknown_0x198;
    u8 curLap;  // 0x199
    u8 padding4[2];
    GhostData ghostData;  // 0x19c
    Timer timers[2];  // 274 1st is self, 2nd is target (ie the ghost)
};  // total size 0x28c
size_assert(CtrlRaceGhostDiffTime, 0x28c);

#endif