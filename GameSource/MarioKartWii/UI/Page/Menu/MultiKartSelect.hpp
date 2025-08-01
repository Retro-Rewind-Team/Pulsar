#ifndef _MULTIKARTSELECT_
#define _MULTIKARTSELECT_

#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Page.hpp>
#include <MarioKartWii/UI/Page/Menu/Menu.hpp>
#include <MarioKartWii/UI/Ctrl/Menu/CtrlMenuMachineGraph.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
#include <MarioKartWii/UI/Ctrl/ModelControl.hpp>
#include <MarioKartWii/UI/Ctrl/CountDown.hpp>
#include <MarioKartWii/UI/Ctrl/UpDown.hpp>

/*
Contributors:
-Melg
*/

//_sinit_ at 8084a9a0

class MultiKartBox : public LayoutUIControl {  // doesn't seem to have any discerneable different
    MultiKartBox();  // 8062b828
    ~MultiKartBox() override;  // 8062b864 vtable 808bd270
};

//_sinit_ at 8084a9a0
namespace Pages {
class MultiKartSelect : public MenuInteractable {  // ID 0x81
   public:
    static const PageId id = PAGE_MULTIPLAYER_KART_SELECT;
    MultiKartSelect();  // 8062b4f4
    ~MultiKartSelect() override;  // 8084a824 vtable 808d9b08
    void OnInit() override;  // 0x28 808496a4
    void OnActivate() override;  // 0x30 80849c24
    void BeforeEntranceAnimations() override;  // 0x38 8084a668
    void BeforeExitAnimations() override;  // 0x40 8084a710
    void AfterControlUpdate() override;  // 0x4c 80849d80
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;  // 0x60 8084a904
    void OnExternalButtonSelect(PushButton& button, u32 hudSlotId) override;  // 0x64 80849f60
    int GetActivePlayerBitfield() const override;  // 0x68 8084a7c0
    int GetPlayerBitfield() const override;  // 0x6C 8084a7b8
    ManipulatorManager& GetManipulatorManager() override;  // 0x70 80849e64
    UIControl* CreateExternalControl(u32 externControlId) override;  // 0x84 80849940
    UIControl* CreateControl(u32 controlId) override;  // 0x88 80849948
    void SetButtonHandlers(PushButton& button) override;  // 0x8c 8084a7c8

    void OnButtonClick(PushButton& button, u32 hudSlotId);  // 80849e6c
    void OnButtonDeselect(PushButton& button, u32 hudSlotId);  // 80849f64
    void OnBackPress(u32 hudSlotId);  // 80849f68

    void OnUpDownChange(UpDownControl& control, u32 hudSlotId, u32 optionId);  // 8084a2a4
    void OnUpDownClick(UpDownControl& control, u32 hudSlotId);  // 8084a100
    void OnTextChange(TextUpDownValueControl::TextControl& control, u32 optionId);  // 8084a2a8

    u32 FillPossibleKarts(u8 hudSlotId);  // 8084a3f8 returns number of possible karts

    static Page* GetPageById(PageId id = PAGE_MULTIPLAYER_KART_SELECT);  // 80849594

    // onButtonClick    vtable = 0x808bd06c function = 80849e6c
    // onButtonSelect   vtable = 0x808bd06c offset   = 0x64 call is virtual
    // onButtonDeselect vtable = 0x808bd06c function = 80849f64
    // onBackPress      vtable = 0x808bd060 function = 80849f68
    // onStartPress     vtable = 0x808bd060 offset   = 0x7c call is virtual

    CountDown* timer;  // 0x6c4

    CharacterId selCharacters[4];  // 0x6c8
    KartId possibleKartIds[12][4];  // 0x6d8 first idx is option Id of textcontrol, 2nd is hudSlotId
    u8 unknown_0x798[0x7c0 - 0x798];
    MultiKartBox boundingBoxes[4];  // 0x7c0
    UpDownControl arrows[4];  // 0xd90
    TextUpDownValueControl text[6];  // 0x24b0
    PtmfHolder_3A<Page, void, UpDownControl&, u32, u32>* upDownChangeHandler;  // 0x3f68 8084a2a4
    PtmfHolder_2A<Page, void, UpDownControl&, u32>* upDownClickHandler;  // 0x3f6c 8084a100
    PtmfHolder_2A<Page, void, TextUpDownValueControl::TextControl&, u32>* onTextChangeHandler;  // 0x3f70 8084a2a8
    VehicleModelControl* vehicleModels;  // 0x3f74 size local player count
    CtrlMenuMachineGraph machineGraphs[2];  // 0x3f78
};
size_assert(MultiKartSelect, 0x4280);
}  // namespace Pages
/* controlGroup:
index 0 = CharaName (MachineSelectName.brctr)
Index 1 = LayoutUIControl (MachineSelectNULL6 variant MachineSelectNULL) which contains 6 controls MachineSelectNULLX
Index 2 = LayoutUIControl (MachineSelectNULL6 variant MachineSelectNULL) which contains 6 controls MachineSelectNULLX each containing 2 controls MachineSelect2_X
*/

#endif