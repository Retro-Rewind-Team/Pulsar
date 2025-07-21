#include <Extra/AutoConnectValidation.hpp>

// Simple string length function since standard library isn't available
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

RoomIdValidation ValidateRoomId(const char* roomId) {
    RoomIdValidation result;
    
    // Check for null pointer
    if (roomId == 0) {
        result.isValid = false;
        result.errorCode = ERROR_ROOM_ID_EMPTY;
        result.actualLength = 0;
        return result;
    }
    
    // Check length
    if (!IsValidRoomIdLength(roomId, result.actualLength)) {
        result.isValid = false;
        if (result.actualLength == 0) {
            result.errorCode = ERROR_ROOM_ID_EMPTY;
        } else {
            result.errorCode = ERROR_ROOM_ID_TOO_LONG;
        }
        return result;
    }
    
    // Check characters
    if (!IsValidRoomIdCharacters(roomId)) {
        result.isValid = false;
        result.errorCode = ERROR_INVALID_ROOM_ID_FORMAT;
        return result;
    }
    
    // All checks passed
    result.isValid = true;
    result.errorCode = ERROR_NONE;
    return result;
}

bool IsValidRoomIdCharacters(const char* roomId) {
    if (roomId == 0) {
        return false;
    }
    
    for (u32 i = 0; roomId[i] != '\0'; ++i) {
        char c = roomId[i];
        
        // Allow alphanumeric characters, hyphens, and underscores
        bool isAlphaNumeric = (c >= 'A' && c <= 'Z') || 
                             (c >= 'a' && c <= 'z') || 
                             (c >= '0' && c <= '9');
        bool isValidSpecial = (c == '-') || (c == '_');
        
        if (!isAlphaNumeric && !isValidSpecial) {
            return false;
        }
    }
    
    return true;
}

bool IsValidRoomIdLength(const char* roomId, u32& actualLength) {
    if (roomId == 0) {
        actualLength = 0;
        return false;
    }
    
    actualLength = StringLength(roomId);
    
    // Room ID must be between 1 and MAX_ROOM_ID_LENGTH-1 characters
    // (leaving space for null terminator)
    return (actualLength > 0) && (actualLength < MAX_ROOM_ID_LENGTH);
}

bool SanitizeRoomId(char* roomId, u32 maxLength) {
    if (roomId == 0 || maxLength == 0) {
        return false;
    }
    
    u32 writePos = 0;
    u32 readPos = 0;
    
    // Copy valid characters to the beginning of the string
    while (roomId[readPos] != '\0' && writePos < maxLength - 1) {
        char c = roomId[readPos];
        
        // Check if character is valid
        bool isAlphaNumeric = (c >= 'A' && c <= 'Z') || 
                             (c >= 'a' && c <= 'z') || 
                             (c >= '0' && c <= '9');
        bool isValidSpecial = (c == '-') || (c == '_');
        
        if (isAlphaNumeric || isValidSpecial) {
            roomId[writePos] = c;
            ++writePos;
        }
        
        ++readPos;
    }
    
    // Null terminate
    roomId[writePos] = '\0';
    
    // Return true if we have at least one valid character
    return writePos > 0;
}

const char* GetErrorDescription(AutoConnectError error) {
    switch (error) {
        case ERROR_NONE:
            return "No error";
            
        // Parameter reading errors
        case ERROR_DOLPHIN_COMMUNICATION:
            return "Failed to communicate with Dolphin emulator";
        case ERROR_NO_ROOM_PARAMETER:
            return "No room parameter provided to Dolphin";
        case ERROR_PARAMETER_READ_FAILED:
            return "Failed to read room parameter from Dolphin";
            
        // Room ID validation errors
        case ERROR_INVALID_ROOM_ID_FORMAT:
            return "Room ID contains invalid characters";
        case ERROR_ROOM_ID_TOO_LONG:
            return "Room ID exceeds maximum length";
        case ERROR_ROOM_ID_EMPTY:
            return "Room ID is empty or null";
            
        // License slot errors
        case ERROR_NO_AVAILABLE_SLOTS:
            return "No license slots available";
        case ERROR_LICENSE_SELECTION_FAILED:
            return "Failed to select license slot";
        case ERROR_AUTO_LOGIN_FAILED:
            return "Failed to automatically log in";
            
        // Connection errors
        case ERROR_ROOM_NOT_FOUND:
            return "Specified room does not exist";
        case ERROR_ROOM_FULL:
            return "Room is at maximum capacity";
        case ERROR_CONNECTION_TIMEOUT:
            return "Connection attempt timed out";
        case ERROR_NETWORK_ERROR:
            return "Network communication error";
        case ERROR_AUTHENTICATION_FAILED:
            return "Failed to authenticate with room";
        case ERROR_INCOMPATIBLE_VERSION:
            return "Version mismatch with room";
            
        // System errors
        case ERROR_NOT_IN_DOLPHIN:
            return "Not running in Dolphin emulator";
        case ERROR_SYSTEM_NOT_READY:
            return "System not ready for auto-connection";
        case ERROR_INITIALIZATION_FAILED:
            return "Auto-connect system initialization failed";
            
        default:
            return "Unknown error";
    }
}

const char* GetStateDescription(ConnectionState state) {
    switch (state) {
        case CONNECTION_IDLE:
            return "Idle - no auto-connection in progress";
        case CONNECTION_READING_PARAMS:
            return "Reading parameters from Dolphin";
        case CONNECTION_VALIDATING_ROOM_ID:
            return "Validating room ID format";
        case CONNECTION_SELECTING_LICENSE:
            return "Selecting appropriate license slot";
        case CONNECTION_CONNECTING:
            return "Attempting room connection";
        case CONNECTION_SUCCESS:
            return "Successfully connected to room";
        case CONNECTION_FAILED:
            return "Connection failed, falling back to normal flow";
        default:
            return "Unknown state";
    }
}

} // namespace AutoConnect
} // namespace Pulsar