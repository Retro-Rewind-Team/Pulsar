#include <kamek.hpp>
#include <MarioKartWii/Audio/AudioManager.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <Sound/MiscSound.hpp>
#include <SlotExpansion/CupsConfig.hpp>
#include <SlotExpansion/UI/ExpansionUIMisc.hpp>
#include <RetroRewind.hpp>

namespace Pulsar {
namespace Sound {
// Custom implementation of music slot expansion; this would break with regs
// kmWrite32(0x8009e0dc, 0x7F87E378); //mr r7, r28 to get string length

static char pulPath[0x100];
s32 CheckBRSTM(const nw4r::snd::DVDSoundArchive* archive, PulsarId id, bool isFinalLap) {
    const char* root = archive->extFileRoot;
    const char* lapSpecifier = isFinalLap ? "_f" : "_n";
    s32 ret = -1;
    char trackName[0x100];
    UI::GetTrackBMG(trackName, id);
    snprintf(pulPath, 0x100, "%sstrm/%s%s.brstm", root, trackName, lapSpecifier);
    ret = DVD::ConvertPathToEntryNum(pulPath);
    if (ret < 0) {
        snprintf(pulPath, 0x50, "%sstrm/%d%s.brstm", root, CupsConfig::ConvertTrack_PulsarIdToRealId(id), lapSpecifier);
        ret = DVD::ConvertPathToEntryNum(pulPath);
    }
    return ret;
}

nw4r::ut::FileStream* MusicSlotsExpand(nw4r::snd::DVDSoundArchive* archive, void* buffer, int size,
                                       const char* extFilePath, u32 r7, u32 length) {
    u8 isBRSTMOn = (static_cast<Pulsar::CTMusic>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_CTMUSIC)));
    const char firstChar = extFilePath[0xC];
    const CupsConfig* cupsConfig = CupsConfig::sInstance;
    const PulsarId track = cupsConfig->GetWinning();

    if ((firstChar == 'n' || firstChar == 'S' || firstChar == 'r') && isBRSTMOn == Pulsar::CTMUSIC_ENABLED) {
        const SectionId section = SectionMgr::sInstance->curSection->sectionId;
        register SoundIDs toPlayId;
        asm(mr toPlayId, r20;);
        if (toPlayId == SOUND_ID_KC && section >= SECTION_P1_WIFI && section <= SECTION_P2_WIFI_FROOM_COIN_VOTING) {
            extFilePath = wifiMusicFile;  // guaranteed to exist because it's been checked before
        }
        if (toPlayId == SOUND_ID_KC && (section >= SECTION_SINGLE_P_FROM_MENU && section <= SECTION_SINGLE_P_LIST_RACE_GHOST || section == SECTION_LOCAL_MULTIPLAYER)) {
            extFilePath = offlineMusicFile;  // guaranteed to exist because it's been checked before
        } else if (!CupsConfig::IsReg(track)) {
            bool isFinalLap = false;
            register u32 strLength;
            asm(mr strLength, r28;);
            const char finalChar = extFilePath[strLength];
            if (finalChar == 'f' || finalChar == 'F') isFinalLap = true;

            bool found = false;
            if (CheckBRSTM(archive, track, isFinalLap) >= 0)
                found = true;
            else if (isFinalLap) {
                if (CheckBRSTM(archive, track, false) >= 0) found = true;
                if (found) Audio::Manager::sInstance->soundArchivePlayer->soundPlayerArray->soundList.GetFront().ambientParam.pitch = 1.1f;
            }
            if (found) extFilePath = pulPath;
        }
    }
    return archive->OpenExtStream(buffer, size, extFilePath, 0, length);
}
kmCall(0x8009e0e4, MusicSlotsExpand);

}  // namespace Sound
}  // namespace Pulsar