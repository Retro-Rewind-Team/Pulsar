
#include <MarioKartWii/UI/Page/Menu/CupSelect.hpp>
#include <MarioKartWii/UI/Page/Menu/CourseSelect.hpp>
#include <MarioKartWii/UI/Page/Other/Votes.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>
#include <SlotExpansion/CupsConfig.hpp>

namespace Pulsar {

// TrackSlot
// Patches course buttons IDs so that it goes vertical than horizontal rather than the opposite
kmWrite32(0x807e525c, 0x937403CC);  // Course buttons ids

// CourseId of 1st button is 4*CupID due to reordering
kmWrite32(0x8084182c, 0x5400103A);
kmWrite32(0x80841830, 0x60000000);

// CourseSelect::LoadNextPage patch mentioned above
int UpdateSlot(const Pages::CourseSelect* page, const CtrlMenuCourseSelectCourse* control, const PushButton* button) {
    CupsConfig* cupsConfig = CupsConfig::sInstance;
    cupsConfig->SaveSelectedCourse(*button);
    return cupsConfig->GetCorrectTrackSlot();
}

asmFunc UpdateSlotWrapper() {
    ASM(
        nofralloc;
        mflr r31;
        bl UpdateSlot;
        mtlr r31;
        mr r31, r3;
        blr;)
}
kmCall(0x80840858, UpdateSlotWrapper);

void SetVotedTrack(Pages::Vote* vote) {  // cast because we actually want to transmit a pulsarId
    CupsConfig* config = CupsConfig::sInstance;
    PulsarId id = config->GetSelected();
    vote->SetVotedCourseId(static_cast<CourseId>(id));
}
kmCall(0x8084099c, SetVotedTrack);

// CtrlMenuCupSelectCup::OnCupButtonClick patch that updates lastSelectCup so that the game remembers it in btw races
void UpdateLastSelCup(Pages::CupSelect* page, CtrlMenuCupSelectCup& cups, PushButton& button, u32 hudSlotId) {
    CupsConfig* cupsConfig = CupsConfig::sInstance;
    if (button.buttonId != cupsConfig->lastSelectedCup) {
        cupsConfig->lastSelectedCup = static_cast<PulsarCupId>(button.buttonId);
        cupsConfig->SetSelected(cupsConfig->ConvertTrack_PulsarCupToTrack(cupsConfig->lastSelectedCup, 0));
    }
    PushButton** buttons = reinterpret_cast<PushButton**>(cups.childrenGroup.controlArray);
    for (int i = 0; i < 8; ++i)
        if (buttons[i] == &button) cupsConfig->lastSelectedCupButtonIdx = i;
    page->LoadNextPage(cups, button, hudSlotId);
    Racedata::sInstance->menusScenario.settings.cupId = cupsConfig->lastSelectedCup % 8;
}
kmCall(0x807e5da8, UpdateLastSelCup);

// Loads correct file
static void FormatTrackPath(char* path, u32 length, const char* format, const char* fileName) {
    const CupsConfig* cupsConfig = CupsConfig::sInstance;
    PulsarId pulsarId = cupsConfig->GetWinning();  // fileName already set through racedata's courseId, which has been set to slot before
    if (IsBattle() || CupsConfig::IsReg(pulsarId))
        snprintf(path, 0x80, format, fileName);
    else {
        const u8 variantIdx = cupsConfig->GetCurVariantIdx();
        const char* format;
        if (variantIdx == 0)
            format = "Race/Course/%d";
        else
            format = "Race/Course/%d_%d";

        if (cupsConfig->HasOddCups() && pulsarId >= (cupsConfig->GetCtsTrackCount() - 4)) pulsarId = static_cast<PulsarId>(pulsarId % 4);
        snprintf(path, 0x80, format, CupsConfig::ConvertTrack_PulsarIdToRealId(pulsarId), variantIdx);
    }
}
kmCall(0x80540820, FormatTrackPath);
kmCall(0x80540870, FormatTrackPath);
kmCall(0x80541bc4, FormatTrackPath);
kmWrite32(0x80531fbc, 0x38800000);  // fix incorrect courseId array read
kmWrite32(0x805407d4, 0x48000020);  // prevent reuse of szs if same courseId

// Fixes GP since it usually uses racedata's courseId which only holds the slot
RacedataScenario* UseCorrectCourse(RacedataScenario* scenario) {
    CupsConfig* cupsConfig = CupsConfig::sInstance;

    cupsConfig->SetWinning(cupsConfig->ConvertTrack_PulsarCupToTrack(cupsConfig->lastSelectedCup, scenario->settings.raceNumber));
    scenario->settings.courseId = cupsConfig->GetCorrectTrackSlot();
    return scenario;
};
kmWrite32(0x8052f220, 0x60000000);

// Wrapper around function above
asmFunc UseCorrectCourseWrapper() {
    ASM(
        nofralloc;
        mflr r0;
        stw r0, 0x8(sp);
        bl UseCorrectCourse;
        lwz r0, 0x8(sp);
        mtlr r0;
        blr;)
}
kmBranch(0x8052f224, UseCorrectCourseWrapper);
kmPatchExitPoint(UseCorrectCourseWrapper, 0x8052f228);

// Badly written, but does the job even though it can in theory hang forever, as unlikely as it is
static void VSRaceRandomFix(SectionParams* params) {  // properly randomizes tracks and sets the first one
    params->vsRaceLimit = 32;
    CupsConfig* cupsConfig = CupsConfig::sInstance;
    Random random;
    PulsarId id;
    bool isRepeat;
    for (int i = 0; i < 32; ++i) {
        do {
            id = cupsConfig->RandomizeTrack();
            isRepeat = false;
            for (int j = 0; j < i; ++j) {
                if (params->vsTracks[j] == id) {
                    isRepeat = true;
                    break;
                }
            }
        } while (isRepeat);
        params->vsTracks[i] = static_cast<CourseId>(id);
    }

    cupsConfig->SetWinning(static_cast<PulsarId>(params->vsTracks[0]));
    Racedata::sInstance->menusScenario.settings.courseId = cupsConfig->GetCorrectTrackSlot();
};
kmBranch(0x805e32ec, VSRaceRandomFix);
kmWrite32(0x8084e5e4, 0x60000000);  // nop racedata courseId store since it's done in the function

// Same as GP, racedata only ever has courseId
static void VSRaceOrderedFix(SectionParams* params) {
    register const Pages::CourseSelect* course;
    asm(mr course, r29;);
    u32 rowIdx = 0;
    for (int i = 0; i < 4; ++i) {
        const CourseButton& cur = course->CtrlMenuCourseSelectCourse.courseButtons[i];
        if (cur.IsSelected()) {
            rowIdx = i;
            break;
        }
    }
    params->vsRaceLimit = 32;
    const CupsConfig* cupsConfig = CupsConfig::sInstance;
    // const PulsarId initial = cupsConfig->winningCourse;
    PulsarCupId cupId = cupsConfig->lastSelectedCup;
    u32 idx = 0;
    while (idx < 32) {
        params->vsTracks[idx] = static_cast<CourseId>(cupsConfig->ConvertTrack_PulsarCupToTrack(cupId, rowIdx));
        ++idx;
        ++rowIdx;
        if (rowIdx == 4) {
            cupId = cupsConfig->GetNextCupId(cupId, 1);
            rowIdx = 0;
        }
    }
};
kmCall(0x80840a24, VSRaceOrderedFix);

CourseId VSNextTrackFix(PulsarId pulsarId) {  // properly sets the next track
    CupsConfig* cupsConfig = CupsConfig::sInstance;
    cupsConfig->SetWinning(pulsarId);
    return cupsConfig->GetCorrectTrackSlot();
}
kmBranch(0x808606cc, VSNextTrackFix);

kmWrite32(0x8085a944, 0x48000018);
static void DemoFix(register Racedata* raceData) {
    register CourseId id;
    asm(mr id, r0;);
    asm(stw r0, 0x1758(raceData););
    CupsConfig::sInstance->SetWinning(static_cast<PulsarId>(id));
}
kmCall(0x8085a95c, DemoFix);

}  // namespace Pulsar