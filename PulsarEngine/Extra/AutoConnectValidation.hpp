#ifndef _PULSAR_AUTOCONNECT_VALIDATION_
#define _PULSAR_AUTOCONNECT_VALIDATION_

#include <Extra/AutoConnectTypes.hpp>

namespace Pulsar {
namespace AutoConnect {

/**
 * @brief Validates room ID format and content
 * @param roomId The room ID string to validate
 * @return RoomIdValidation structure with validation results
 */
RoomIdValidation ValidateRoomId(const char* roomId);

/**
 * @brief Checks if a room ID contains only valid characters
 * @param roomId The room ID string to check
 * @return True if all characters are valid, false otherwise
 */
bool IsValidRoomIdCharacters(const char* roomId);

/**
 * @brief Checks if room ID length is within acceptable bounds
 * @param roomId The room ID string to check
 * @param actualLength Output parameter for actual length
 * @return True if length is valid, false otherwise
 */
bool IsValidRoomIdLength(const char* roomId, u32& actualLength);

/**
 * @brief Sanitizes room ID by removing invalid characters
 * @param roomId Input/output room ID string to sanitize
 * @param maxLength Maximum length of the buffer
 * @return True if sanitization was successful, false if too many invalid chars
 */
bool SanitizeRoomId(char* roomId, u32 maxLength);

/**
 * @brief Converts error code to human-readable string
 * @param error The error code to convert
 * @return Pointer to error description string
 */
const char* GetErrorDescription(AutoConnectError error);

/**
 * @brief Converts connection state to human-readable string
 * @param state The connection state to convert
 * @return Pointer to state description string
 */
const char* GetStateDescription(ConnectionState state);

} // namespace AutoConnect
} // namespace Pulsar

#endif