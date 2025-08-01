#ifndef _RACEINTRO_
#define _RACEINTRO_
#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Page.hpp>
namespace Pages {
//_sinit_ at 808556d0
class RaceIntro : public Page {  // ID 0x8, also ID 0x7 ID 0x9 ID 0xa ID 0xb
    static const PageId id = PAGE_VS_INTRO;
    RaceIntro();  // 80624690
    ~RaceIntro() override;  // 80855644 vtable 808da568
    void OnInit() override;  // 80855200
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 808556c4
    LayoutUIControl trackName;  // 0x44
    LayoutUIControl titleText;  // 0x1b8 cup_name brlyt, "VS Race - 1 of 16"
    ManipulatorManager manipulatorManager;  // 0x32c
};  // 0x33c
size_assert(RaceIntro, 0x33c);
}  // namespace Pages
#endif