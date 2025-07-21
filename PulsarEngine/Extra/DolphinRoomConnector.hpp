#ifndef _PULSAR_DOLPHIN_ROOM_CONNECTOR_
#define _PULSAR_DOLPHIN_ROOM_CONNECTOR_

#include <types.hpp>
#include <Extra/AutoConnectTypes.hpp>

namespace Pulsar {
namespace AutoConnect {

/**
 * @brief Main coordinator class for Dolphin room auto-connection
 * 
 * This class manages the complete workflow of automatically connecting to rooms
 * when launched through Dolphin emulator with room parameters. It coordinates
 * parameter reading, license selection, and room connection processes.
 */
class DolphinRoomConnector {
public:
    /**
     * @brief Get the singleton instance of DolphinRoomConnector
     * @return Reference to the singleton instance
     */
    static DolphinRoomConnector& GetInstance();

    /**
     * @brief Initialize the auto-connection system
     * 
     * Sets up the auto-connector during system initialization.
     * Checks if running in Dolphin and prepares for auto-connection.
     * 
     * @return true if initialization successful, false otherwise
     */
    bool Initialize();

    /**
     * @brief Process room parameter from Dolphin
     * 
     * Reads and validates room ID from Dolphin emulator parameters.
     * Updates internal state based on parameter availability and validity.
     * 
     * @return true if parameter processed successfully, false otherwise
     */
    bool ProcessRoomParameter();

    /**
     * @brief Check if auto-connection is enabled and available
     * 
     * Performs Dolphin detection and checks if auto-connection should be active.
     * This includes checking emulator status and system readiness.
     * 
     * @return true if auto-connection is enabled and available, false otherwise
     */
    bool IsEnabled() const;

    /**
     * @brief Get current connection state
     * @return Current ConnectionState
     */
    ConnectionState GetConnectionState() const { return currentState; }

    /**
     * @brief Get current configuration
     * @return Reference to current RoomConnectionConfig
     */
    const RoomConnectionConfig& GetConfig() const { return config; }

    /**
     * @brief Get last error code
     * @return Last AutoConnectError that occurred
     */
    AutoConnectError GetLastError() const { return lastError; }

    /**
     * @brief Check if auto-connection attempt is in progress
     * @return true if connection attempt is active, false otherwise
     */
    bool IsConnectionInProgress() const;

    /**
     * @brief Reset the connector to idle state
     * 
     * Clears current state and prepares for new connection attempt.
     * Used for cleanup after connection completion or failure.
     */
    void Reset();

private:
    // Private constructor for singleton pattern
    DolphinRoomConnector();
    
    // Prevent copying
    DolphinRoomConnector(const DolphinRoomConnector&);
    DolphinRoomConnector& operator=(const DolphinRoomConnector&);

    /**
     * @brief Update internal connection state
     * @param newState New state to transition to
     */
    void SetConnectionState(ConnectionState newState);

    /**
     * @brief Set error code and update state accordingly
     * @param error Error code to set
     */
    void SetError(AutoConnectError error);



    /**
     * @brief Check if system is ready for auto-connection
     * @return true if system is ready, false otherwise
     */
    bool IsSystemReady() const;

    // Member variables
    RoomConnectionConfig config;        // Current connection configuration
    ConnectionState currentState;       // Current state of connection process
    AutoConnectError lastError;         // Last error that occurred
    bool isInitialized;                // Whether Initialize() has been called
    bool isDolphinDetected;            // Whether Dolphin emulator was detected
    u32 initializationTime;            // Time when initialization occurred
    
    // Singleton instance
    static DolphinRoomConnector* sInstance;
};

} // namespace AutoConnect
} // namespace Pulsar

#endif