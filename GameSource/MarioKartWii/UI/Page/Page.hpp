/*
Credits:
kHacker35000vr and the Sanbo team (dolphin map, PageText)
stebler (names for some of the classes)
Melg
*/

#ifndef _PAGE_
#define _PAGE_
#include <kamek.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>
#include <MarioKartWii/UI/Ctrl/Manipulator.hpp>

/*
See Section.hpp for the hierarchical superior of sections and UIControl.hpp for the subordinate

How a page works:
->The page is built via its ctor, but every control is loaded via OnInit (called right after the ctor)
->When the page's section is exiting, vf OnDispose is called right before the dtor is called

Page::UpdateState checks for the updateState bool and simply changes the state according to the current state:
-It goes DEACTIVATED->ACTIVATING->ENTERING->ACTIVE->EXITING->EXITED->DEACTIVATED

-> Initially, a page is DEACTIVATED, no virtual functions are called
->The page is activated either because it was added as a layer on top of another page,
or because it was set as the next page (via vf GetNextPage)
The game calls Page::Activate, which changes the state to ACTIVATING, and calls the vf OnActivate (a single time)
->After that function has ran, the state is again changed to ENTERING. Three functions are called once:
-vf BeforeEntranceAnimations
-then Page::AnimateControls that makes the controls look like they're coming into the screen (because the state is 3)
-vf AfterEntranceAnimations, but only after the animations have been performed

->The state is then changed to Active which is the "idle" state of a running page:
-All controls are updated every frame via Page::UpdateControls:
This calls vf BeforeControlUpdate, then ControlGroup::Update, then vf AfterControlUpdate and only then AnimateControls

->A page ends because a back button was pressed, or the page ended by itself or a timer made the page end:
-Any of these situations culminate in Page::EndState being called, which simply sets the updateState bool to true
-Page::StartExit is called which sets the state to EXITING and  calls vf BeforeExitAnimations
-vf StartExitAnimations is called, but only after the animations have been performed

->Because the state is now EXITED, the page is automatically deactivated via Page::Deactivate which calls vf OnDeactivate
and sets the state to DEACTIVATED

->If a page is added as a layer (instead of just replacing the previous page), for example like a pause page,
then once it ends, the page "under" is resumed via vf OnResume

->While a page's state is ENTERING, ACTIVE, EXITING or EXITED, vf OnUpdate is called every frame

*/

enum PageState {
    STATE_UNLOADED = 0,
    STATE_DEACTIVATED = 1,
    STATE_ACTIVATING = 2,
    STATE_ENTERING = 3,
    STATE_ACTIVE = 4,
    STATE_EXITING = 5,
    STATE_EXITED = 6
};

class Page {
   public:
    Page();  // 8060197c
    virtual ~Page();  // 80601a04 vtable 808ba5c0
    virtual const char* GetClassName() const;  // 0xC 805bb25c returns "(unknown page)"
    virtual PageId GetNextPage() const;  // 0x10 805bb278 returns -1
    virtual int IsHomeMenuWorking() const;  // 0x14 805bb254 returns 3 if homebutton is not allowed
    virtual bool IsDVDEnabled();  // 0x18 805bb24c
    virtual void ChangeSectionBySceneChange(SectionId sectionId, u32 animDirection, float animLength);  // 0x1c 806024b0, will initiate a scene change
    virtual void ChangeSectionByReinit();  // 0x20 80602530, reinits scene, so can lead to a crash if changing to section of != sceneId
    virtual void AddPageLayer(PageId id, u32 animDirection);  // 0x24 806025b0
    virtual void OnInit();  // 0x28 80601ad8 just a blr
    virtual void OnDispose();  // 0x2c 805bb248 just a blr
    virtual void OnActivate();  // 0x30 805c9888 just a blr
    virtual void OnDeactivate();  // 0x34 805bb244 just a blr
    virtual void BeforeEntranceAnimations();  // 0x38 805bb240 just a blr
    virtual void AfterEntranceAnimations();  // 0x3c 805bb23c just a blr
    virtual void BeforeExitAnimations();  // 0x40 805bb238 just a blr
    virtual void AfterExitAnimations();  // 0x44 805bb234 just a blr
    virtual void BeforeControlUpdate();  // 0x48 805bb230 just a blr
    virtual void AfterControlUpdate();  // 0x4c 805bf2d8 just a blr
    virtual void OnUpdate();  // 0x50 805bb22c just a blr
    virtual void OnResume();  // 0x54 805bb228 just a blr called when a layer on top of the page is removed
    virtual void OnSectionChange();  // 0x58 805bb224 just a blr
    virtual void func_0x5C();  // 0x5c 805bb220 just a blr
    virtual const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const;  // 0x60 805bed68 returns 809C1d10

    void Init(PageId pageId);  // 80601a60
    void Dispose();  // 80601adc
    void Activate();  // 80601aec
    void Deactivate();  // 80601c08
    void IncrementDuration();  // 80601c48
    void CheckActions();  // 80601c64
    bool UpdateState();  // 80601d24 returns true if the state was effectively changed
    void Exit();  // 80601f44
    void Enter();  // 80602144
    void UpdateControls();  // 806022cc
    void Update();  // 80602338
    void Draw(u32 curZIdx);  // 8060235c
    void ResetManipulatorManager();  // 80602378
    void Resume();  // 80602390
    void HandleSectionChange();  // 80602428 calls OnSectionChange
    void SetAnimDirection(u32 animDirection);  // 8060244c
    void InitControlGroup(u32 controlCount);  // 8060245C
    void AddControl(u8 controlIdx, UIControl& control, u32 zIdx);  // 8060246c
    void SetManipulatorManager(ManipulatorManager& manager);  // 00602474
    void EndState();  // 8060247c
    void EndStateAnimated(u32 animDirection, float animLength);  // 80602488
    void EndExitState();  // 806024a4
    void AnimateControls();  // 8060265c
    void StartEntrance();  // 80602970 inlined
    void EndEntrance();  // 806029f4
    void StartExit();  // 80602b04
    void EndExit();  // 80602c40 inlined in Exit
    void PlaySound(SoundIDs soundId, u32 r5);  // 80602cf8
    void SetTransitionSound(u32 entranceSoundId, u32 exitSoundId);  // 80602d20
    // static Page* GetPageById(PageId type);

    PageId pageId;
    PageState currentState;  // 0x8
    bool updateState;  // 0xc
    u8 padding[3];
    u32 animationDirection;
    float animationDelay;  // 0x14
    float exitAnimDelay;
    u32 curStateDuration;  // 0x1C
    u32 duration;  // 0x20
    ControlGroup controlGroup;  // 0x24
    ManipulatorManager* manipulatorManager;  // 0x38
    u32 entranceSoundId;
    u32 exitSoundId;

    class ControlAnimator : ControlGroupAction {
        virtual void Calc(UIControl* control);  // 806027ac vtable 808ba63c
        u32 animationDirection;  // 0x4
        PageState pageState;  // 0x8
        float animationDelay;  // 0xC
        float direction;  // 0x10 = 1/-1  if animdirection = 0/1 and state = activating or active
        float directionOneTenth;  // 0x14
        float negatedDirection;  // just direction * -1
        float delay;  // 0x1c
        bool hasAnimatedEnded;  // gets filled by Calc
        u8 padding[4];
    };  // total size 0x24

    class OnStateChangeControlAnimator : ControlGroupAction {
        void Calc(UIControl* control) override;  // 808ba624 80602960 resets initial position on load/unload for example
        float direction;  // 0x4 = 1/-1  if animdirection = 0/1 and state = activating or exiting
    };

    class MaxAnimDelayGetter : ControlGroupAction {
        void Calc(UIControl* control) override;  // 808ba630 80602920
        float delay;  // filled by calc from the control
    };
    static float transitionDelay;

};  // Total Size 0x44
size_assert(Page, 0x44);

#endif