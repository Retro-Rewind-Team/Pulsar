#ifndef _PULSAR_AUTOCONNECT_TYPES_
#define _PULSAR_AUTOCONNECT_TYPES_

#include <types.hpp>

namespace Pulsar {
namespace AutoConnect {

// Maximum length for room ID strings
static const u32 MAX_ROOM_ID_LENGTH = 32;

// Connection timeout in milliseconds
static const u32 DEFAULT_CONNECTION_TIMEOUT = 30000; // 30 seconds

/**
 * @brief Connection state enumeration
 * Tracks the current state of the auto-connection process
 */
enum ConnectionState {
    CONNECTION_IDLE = 0,                // No auto-connection in progress
    CONNECTION_READING_PARAMS,          // Reading parameters from Dolphin
    CONNECTION_VALIDATING_ROOM_ID,      // Validating room ID format
    CONNECTION_SELECTING_LICENSE,       // Selecting appropriate license slot
    CONNECTION_CONNECTING,              // Attempting room connection
    CONNECTION_SUCCESS,                 // Successfully connected to room
    CONNECTION_FAILED                   // Connection failed, falling back
};

/**
 * @brief Error codes for auto-connection failures
 * Provides specific error information for debugging and logging
 */
enum AutoConnectError {
    ERROR_NONE = 0,                     // No error
    
    // Parameter reading errors
    ERROR_DOLPHIN_COMMUNICATION,        // Failed to communicate with Dolphin
    ERROR_NO_ROOM_PARAMETER,           // No room parameter provided
    ERROR_PARAMETER_READ_FAILED,       // Failed to read room parameter
    
    // Room ID validation errors
    ERROR_INVALID_ROOM_ID_FORMAT,      // Room ID format is invalid
    ERROR_ROOM_ID_TOO_LONG,            // Room ID exceeds maximum length
    ERROR_ROOM_ID_EMPTY,               // Room ID is empty or null
    
    // License slot errors
    ERROR_NO_AVAILABLE_SLOTS,          // No license slots available
    ERROR_LICENSE_SELECTION_FAILED,    // Failed to select license slot
    ERROR_AUTO_LOGIN_FAILED,           // Failed to automatically log in
    
    // Connection errors
    ERROR_ROOM_NOT_FOUND,              // Specified room does not exist
    ERROR_ROOM_FULL,                   // Room is at capacity
    ERROR_CONNECTION_TIMEOUT,          // Connection attempt timed out
    ERROR_NETWORK_ERROR,               // General network error
    ERROR_AUTHENTICATION_FAILED,       // Failed to authenticate with room
    ERROR_INCOMPATIBLE_VERSION,        // Version mismatch with room
    
    // System errors
    ERROR_NOT_IN_DOLPHIN,              // Not running in Dolphin emulator
    ERROR_SYSTEM_NOT_READY,            // System not ready for auto-connect
    ERROR_INITIALIZATION_FAILED        // Auto-connect system initialization failed
};

/**
 * @brief Room connection configuration structure
 * Contains all settings and parameters for auto-connection
 */
struct RoomConnectionConfig {
    char roomId[MAX_ROOM_ID_LENGTH];    // Room ID to connect to
    u8 targetLicenseSlot;               // License slot to use (0xFF = auto-select)
    bool isEnabled;                     // Whether auto-connect is enabled
    u32 connectionTimeout;              // Timeout for connection attempts (ms)
    bool fallbackToMenu;                // Whether to fall back to menu on failure
    
    // Constructor with default values
    RoomConnectionConfig() {
        roomId[0] = '\0';
        targetLicenseSlot = 0xFF;       // Auto-select
        isEnabled = false;
        connectionTimeout = DEFAULT_CONNECTION_TIMEOUT;
        fallbackToMenu = true;
    }
};

/**
 * @brief Connection attempt result structure
 * Contains information about the outcome of a connection attempt
 */
struct ConnectionResult {
    ConnectionState finalState;         // Final state after attempt
    AutoConnectError errorCode;         // Error code if failed
    u32 elapsedTime;                   // Time taken for attempt (ms)
    u8 selectedLicenseSlot;            // License slot that was selected
    
    // Constructor
    ConnectionResult() {
        finalState = CONNECTION_IDLE;
        errorCode = ERROR_NONE;
        elapsedTime = 0;
        selectedLicenseSlot = 0xFF;
    }
};

/**
 * @brief Room ID validation result
 * Contains validation outcome and specific error information
 */
struct RoomIdValidation {
    bool isValid;                       // Whether room ID is valid
    AutoConnectError errorCode;         // Specific validation error
    u32 actualLength;                  // Actual length of room ID
    
    // Constructor
    RoomIdValidation() {
        isValid = false;
        errorCode = ERROR_NONE;
        actualLength = 0;
    }
};

} // namespace AutoConnect
} // namespace Pulsar

#endif