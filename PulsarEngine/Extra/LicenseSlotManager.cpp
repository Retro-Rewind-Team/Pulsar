#include <Extra/LicenseSlotManager.hpp>

// External RKSYS functions from the game
// These are defined in the game's code and we're using them through function pointers
extern "C" {
    // RKSYS::Mgr singleton access
    void* RKSYS_GetStaticInstance();
    
    // RKSYS::LicenseMgr functions
    void RKSYS_LicenseMgr_SetCurLicense(void* rkSystem, u8 licenseId);
    bool RKSYS_LicenseMgr_SetInitialized(void* rkSystem, u8 licenseId, bool initialized);
    
    // RKSYS::Mgr functions
    bool RKSYS_Mgr_CheckLicenseMagic(void* rkSystem, u8 licenseId);
}

// Function pointer types for the external functions
typedef void* (*GetStaticInstanceFunc)();
typedef void (*SetCurLicenseFunc)(void*, u8);
typedef bool (*SetInitializedFunc)(void*, u8, bool);
typedef bool (*CheckLicenseMagicFunc)(void*, u8);

// Function pointers to the actual functions
static GetStaticInstanceFunc getRKSystemInstance = (GetStaticInstanceFunc)0x80008e84;
static SetCurLicenseFunc setCurLicense = (SetCurLicenseFunc)0x80544cd8;
static SetInitializedFunc setLicenseInitialized = (SetInitializedFunc)0x8054454c;
static CheckLicenseMagicFunc checkLicenseMagic = (CheckLicenseMagicFunc)0x80544d10;

namespace Pulsar {
namespace AutoConnect {

// Static instance initialization
LicenseSlotManager* LicenseSlotManager::sInstance = 0;

LicenseSlotManager::LicenseSlotManager() 
    : isInitialized(false)
    , currentSlot(INVALID_SLOT)
    , lastError(ERROR_NONE) {
}

LicenseSlotManager& LicenseSlotManager::GetInstance() {
    if (sInstance == 0) {
        sInstance = new LicenseSlotManager();
    }
    return *sInstance;
}

bool LicenseSlotManager::Initialize() {
    if (isInitialized) {
        return true;
    }
    
    // Reset state
    currentSlot = INVALID_SLOT;
    lastError = ERROR_NONE;
    
    // Check if RKSYS is available
    void* rkSystem = getRKSystemInstance();
    if (rkSystem == 0) {
        SetError(ERROR_SYSTEM_NOT_READY);
        return false;
    }
    
    isInitialized = true;
    return true;
}

bool LicenseSlotManager::SelectAvailableSlot(u8 preferredSlot, u8& selectedSlot) {
    if (!isInitialized) {
        if (!Initialize()) {
            SetError(ERROR_SYSTEM_NOT_READY);
            return false;
        }
    }
    
    void* rkSystem = getRKSystemInstance();
    if (rkSystem == 0) {
        SetError(ERROR_SYSTEM_NOT_READY);
        return false;
    }
    
    // If a specific slot is requested and it's available, use it
    if (preferredSlot != INVALID_SLOT && preferredSlot < MAX_LICENSE_SLOTS) {
        if (IsSlotAvailable(preferredSlot)) {
            selectedSlot = preferredSlot;
            return true;
        }
    }
    
    // Otherwise, find the first available slot
    u8 availableSlot = FindFirstAvailableSlot();
    if (availableSlot != INVALID_SLOT) {
        selectedSlot = availableSlot;
        return true;
    }
    
    // If no slots are available, try to create a default one
    if (CreateDefaultSlot(selectedSlot)) {
        return true;
    }
    
    // No slots available and couldn't create one
    SetError(ERROR_NO_AVAILABLE_SLOTS);
    return false;
}

bool LicenseSlotManager::AutoLogin(u8 slotId) {
    if (!isInitialized) {
        if (!Initialize()) {
            SetError(ERROR_SYSTEM_NOT_READY);
            return false;
        }
    }
    
    // Validate slot ID
    if (slotId >= MAX_LICENSE_SLOTS) {
        SetError(ERROR_LICENSE_SELECTION_FAILED);
        return false;
    }
    
    // Check if the slot is available
    if (!IsSlotAvailable(slotId)) {
        SetError(ERROR_LICENSE_SELECTION_FAILED);
        return false;
    }
    
    void* rkSystem = getRKSystemInstance();
    if (rkSystem == 0) {
        SetError(ERROR_SYSTEM_NOT_READY);
        return false;
    }
    
    // Set the current license
    setCurLicense(rkSystem, slotId);
    
    // Update current slot
    currentSlot = slotId;
    
    return true;
}

bool LicenseSlotManager::IsSlotAvailable(u8 slotId) const {
    // Validate slot ID
    if (slotId >= MAX_LICENSE_SLOTS) {
        return false;
    }
    
    void* rkSystem = getRKSystemInstance();
    if (rkSystem == 0) {
        return false;
    }
    
    // Check if the license has valid magic (initialized)
    return checkLicenseMagic(rkSystem, slotId);
}

u8 LicenseSlotManager::GetMaxSlots() const {
    return MAX_LICENSE_SLOTS;
}

u8 LicenseSlotManager::GetCurrentSlot() const {
    return currentSlot;
}

bool LicenseSlotManager::HasAvailableSlots() const {
    return FindFirstAvailableSlot() != INVALID_SLOT;
}

u8 LicenseSlotManager::FindFirstAvailableSlot() const {
    void* rkSystem = getRKSystemInstance();
    if (rkSystem == 0) {
        return INVALID_SLOT;
    }
    
    // Check each slot
    for (u8 slotId = 0; slotId < MAX_LICENSE_SLOTS; ++slotId) {
        if (IsSlotAvailable(slotId)) {
            return slotId;
        }
    }
    
    return INVALID_SLOT;
}

bool LicenseSlotManager::CreateDefaultSlot(u8& slotId) {
    void* rkSystem = getRKSystemInstance();
    if (rkSystem == 0) {
        return false;
    }
    
    // Try to initialize the first slot
    for (u8 i = 0; i < MAX_LICENSE_SLOTS; ++i) {
        // If the slot doesn't exist, try to initialize it
        if (!IsSlotAvailable(i)) {
            if (setLicenseInitialized(rkSystem, i, true)) {
                slotId = i;
                return true;
            }
        }
    }
    
    return false;
}

void LicenseSlotManager::SetError(AutoConnectError error) {
    if (error != ERROR_NONE) {
        lastError = error;
    }
}

} // namespace AutoConnect
} // namespace Pulsar