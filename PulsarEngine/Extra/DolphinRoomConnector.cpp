#include <Extra/DolphinRoomConnector.hpp>
#include <Dolphin/DolphinIOS.hpp>
#include <Extra/AutoConnectValidation.hpp>

// Simple string copy function since standard library isn't available
static void StringCopy(char* dest, const char* src, u32 maxLen) {
    if (dest == 0 || src == 0 || maxLen == 0) return;
    
    u32 i = 0;
    while (i < maxLen - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Simple string length function
static u32 StringLength(const char* str) {
    if (str == 0) return 0;
    u32 len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

namespace Pulsar {
namespace AutoConnect {

// Static instance initialization
DolphinRoomConnector* DolphinRoomConnector::sInstance = 0;

DolphinRoomConnector::DolphinRoomConnector() 
    : currentState(CONNECTION_IDLE)
    , lastError(ERROR_NONE)
    , isInitialized(false)
    , isDolphinDetected(false)
    , initializationTime(0) {
    // Initialize config with default values
    config = RoomConnectionConfig();
}

DolphinRoomConnector& DolphinRoomConnector::GetInstance() {
    if (sInstance == 0) {
        sInstance = new DolphinRoomConnector();
    }
    return *sInstance;
}

bool DolphinRoomConnector::Initialize() {
    if (isInitialized) {
        return true;
    }

    // Initializing DolphinRoomConnector

    // Reset state
    Reset();

    // Check if running in Dolphin emulator
    isDolphinDetected = Dolphin::IsEmulator();
    
    if (!isDolphinDetected) {
        // Not running in Dolphin emulator - auto-connect disabled
        config.isEnabled = false;
        isInitialized = true;
        return true;
    }

    // Dolphin emulator detected - auto-connect available
    
    // Enable auto-connection for Dolphin
    config.isEnabled = true;
    
    // Record initialization time
    if (!Dolphin::GetElapsedTime(initializationTime)) {
        initializationTime = 0;
    }

    isInitialized = true;
    
    // DolphinRoomConnector initialized successfully
    return true;
}

bool DolphinRoomConnector::ProcessRoomParameter() {
    if (!IsEnabled()) {
        SetError(ERROR_NOT_IN_DOLPHIN);
        return false;
    }

    if (!IsSystemReady()) {
        SetError(ERROR_SYSTEM_NOT_READY);
        return false;
    }

    SetConnectionState(CONNECTION_READING_PARAMS);

    // Check if room parameter is available
    if (!Dolphin::HasRoomParameter()) {
        SetError(ERROR_NO_ROOM_PARAMETER);
        SetConnectionState(CONNECTION_IDLE);
        return false;
    }

    // Read room parameter from Dolphin
    char roomIdBuffer[MAX_ROOM_ID_LENGTH];
    if (!Dolphin::GetRoomParameter(roomIdBuffer, sizeof(roomIdBuffer))) {
        SetError(ERROR_PARAMETER_READ_FAILED);
        SetConnectionState(CONNECTION_FAILED);
        return false;
    }

    // Validate room ID format
    SetConnectionState(CONNECTION_VALIDATING_ROOM_ID);
    RoomIdValidation validation = ValidateRoomId(roomIdBuffer);
    
    if (!validation.isValid) {
        SetError(validation.errorCode);
        SetConnectionState(CONNECTION_FAILED);
        return false;
    }

    // Store validated room ID in configuration
    StringCopy(config.roomId, roomIdBuffer, MAX_ROOM_ID_LENGTH);

    SetConnectionState(CONNECTION_IDLE);
    return true;
}

bool DolphinRoomConnector::IsEnabled() const {
    return isInitialized && isDolphinDetected && config.isEnabled;
}

bool DolphinRoomConnector::IsConnectionInProgress() const {
    return currentState != CONNECTION_IDLE && 
           currentState != CONNECTION_SUCCESS && 
           currentState != CONNECTION_FAILED;
}

void DolphinRoomConnector::Reset() {
    currentState = CONNECTION_IDLE;
    lastError = ERROR_NONE;
    
    // Clear room ID but preserve other config settings
    config.roomId[0] = '\0';
    config.targetLicenseSlot = 0xFF; // Auto-select
}

void DolphinRoomConnector::SetConnectionState(ConnectionState newState) {
    if (currentState != newState) {
        currentState = newState;
    }
}

void DolphinRoomConnector::SetError(AutoConnectError error) {
    if (error != ERROR_NONE) {
        lastError = error;
        
        // Transition to failed state for most errors
        if (currentState != CONNECTION_IDLE) {
            SetConnectionState(CONNECTION_FAILED);
        }
    }
}



bool DolphinRoomConnector::IsSystemReady() const {
    // Check if we're initialized
    if (!isInitialized) {
        return false;
    }

    // Check if we're in Dolphin
    if (!isDolphinDetected) {
        return false;
    }

    // System is ready if we've passed basic initialization
    return true;
}

} // namespace AutoConnect
} // namespace Pulsar