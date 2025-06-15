#include <PulsarSystem.hpp>
#include "RetroRewindChannel.hpp"

namespace Pulsar {

bool IsNewChannel() {
    return *reinterpret_cast<u32*>(RRC_SIGNATURE_ADDRESS) == RRC_SIGNATURE;
}

bool NewChannel_UseSeparateSavegame() {
    return *reinterpret_cast<u8*>(RRC_BITFLAGS_ADDRESS) & RRC_BITFLAG_SEPARATE_SAVEGAME == RRC_BITFLAG_SEPARATE_SAVEGAME;
}

void NewChannel_SetLoadedFromRRFlag() {
    *reinterpret_cast<u8*>(RRC_BITFLAGS_ADDRESS) |= RRC_BITFLAG_LOADED_FROM_RR;
}

void NewChannel_SetCrashFlag() {
    *reinterpret_cast<u8*>(RRC_BITFLAGS_ADDRESS) |= RRC_BITFLAG_RR_CRASHED;
}

}