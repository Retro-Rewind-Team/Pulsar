/*
SectionMgr is the class that holds the information about the current displayed Section.
Documentation:
kHacker35000vr
Melg
*/

#ifndef _SECTIONMGR_
#define _SECTIONMGR_
#include <kamek.hpp>
#include <MarioKartWii/UI/Section/Section.hpp>
#include <MarioKartWii/UI/Section/SectionPad.hpp>
#include <MarioKartWii/UI/Section/RKSYSRequester.hpp>
#include <MarioKartWii/UI/Section/SystemBMGHolder.hpp>
#include <MarioKartWii/UI/Section/SectionParams.hpp>

class Page;

class SectionMgr {
   public:
    static SectionMgr* sInstance;  // 809c1e38
    static SectionMgr* CreateInstance();  // 80634c90
    static void DestroyInstance();  // 80634cc8

    static Pages::System* CreateSystemPages(u8 idx, SectionId id);  // 80634a64
    static int GetSectionPriority(SectionId id);  // 80634b80
    SectionMgr();  // 80634d40
    ~SectionMgr();  // 80634dc4
    void Init();  // 80634e44
    SceneId GetBootSectionSceneId();  // 80634fa0
    void LoadSection();  // 80634fbc
    void EnterSection();  // 80635080
    void ExitSection();  // 8063519c
    void RaceUpdate();  // 806354c0, if current Scene = RaceScene
    void Update();  // 8063550c
    void MenuUpdate();  // 8063583c if current Scene = MenuScene
    void Pause();  // 80635a2c for example on Home Menu
    void Resume();  // 80635a34 for example when Home Menu ends
    void SetNextSection(SectionId id, u32 animDirection);  // 80635a3c only if new priority > old priority
    void RequestSceneChange(u32 delay, u32 fadeOutColor);  // 80635ac8
    void RequestSceneReinit(u32 delay, u32 fadeOutColor);  // 80635b2c
    void RequestGoToWiiMenu();  // 80635c0c SetNextSection with "ReturnToMenu" section
    void RequestPowerOff();  // 80635c74  SetNextSection with "PowerOff" section
    const char* GetNextSectionArchiveName();  // 80635cdc
    bool NextSectionHasBackModel();  // 80635ce4
    bool NextSectionHasDriverModel();  // 80635cec
    void UpdateStatusBitfield();  // 80635cf4 inlined in update
    void UpdateDVDStatus(bool forceResume);  // 80635e08 inlined, forceResume regardless of Pages' isDvdEnabled

    Section* curSection; /* http://wiki.tockdom.com/wiki/List_of_Identifiers#Menu_Identifiers */
    SectionId bootSectionId;  // 0x4 usually set by SavedGhostHandler::Init, for example Creating new save/Corrupted save etc...
    SectionId unknownSectionId;
    SectionId nextSectionId;  // 0xc
    SectionId prevSectionId;
    u32 openingFadeAnimIdx;
    u32 fadeAnimIdx;  // 0x18
    u32 fadeDelay;  // 0x1c
    bool firstLoad;  // 0x20
    u8 unknown_0x21[3];

    u32 statusBitfield;  // 0x24 2 = homebutton allowed
    ut::Color fadeOutColor;  // 0x28
    u32 dvdRelatedFrameCounter;  // 0x2c
    u32 fadeSceneAction;  // 0x30 1 = reinit scene, 2 = change scene
    SectionPad pad;  // 0x34
    RKSYSRequester* rksysRequester;  // 0x90
    SystemBMGHolder* systemBMG;  // 0x94
    SectionParams* sectionParams;

};  // Total Size 0x9C
size_assert(SectionMgr, 0x9c);

#endif