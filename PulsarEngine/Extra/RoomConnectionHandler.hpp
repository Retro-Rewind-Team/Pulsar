#ifndef _PULSAR_ROOM_CONNECTION_HANDLER_
#define _PULSAR_ROOM_CONNECTION_HANDLER_

#include <types.hpp>
#include <Extra/AutoConnectTypes.hpp>

namespace Pulsar {
namespace AutoConnect {

/**
 * @brief Handles room connection process for auto-connection
 * 
 * This class manages the actual room connection process using the existing
 * network infrastructure. It validates room IDs, initiates connections,
 * and processes connection results.
 */
class RoomConnectionHandler {
public:
    /**
     * @brief Get the singleton instance of RoomConnectionHandler
     * @return Reference to the singleton instance
     */
    static RoomConnectionHandler& GetInstance();
    
    /**
     * @brief Initialize the room connection handler
     * @return true if initialization successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Connect to a room using the specified room ID
     * 
     * Attempts to connect to the specified room using the existing
     * network infrastructure. This includes validating the room ID,
     * initiating the connection, and handling the result.
     * 
     * @param roomId Room ID to connect to
     * @param timeout Connection timeout in milliseconds
     * @return ConnectionResult containing the outcome of the connection attempt
     */
    ConnectionResult ConnectToRoom(const char* roomId, u32 timeout = DEFAULT_CONNECTION_TIMEOUT);
    
    /**
     * @brief Validate a room ID format
     * 
     * Checks if the specified room ID has a valid format.
     * This includes checking length and allowed characters.
     * 
     * @param roomId Room ID to validate
     * @return RoomIdValidation containing validation result
     */
    RoomIdValidation ValidateRoomId(const char* roomId);
    
    /**
     * @brief Handle the result of a connection attempt
     * 
     * Processes the outcome of a connection attempt, including
     * updating state, logging, and error handling.
     * 
     * @param result Connection result to handle
     * @return true if connection was successful, false otherwise
     */
    bool HandleConnectionResult(const ConnectionResult& result);
    
    /**
     * @brief Check if a connection attempt is in progress
     * @return true if connection is in progress, false otherwise
     */
    bool IsConnecting() const;
    
    /**
     * @brief Cancel an in-progress connection attempt
     * @return true if cancellation successful, false otherwise
     */
    bool CancelConnection();
    
    /**
     * @brief Get the last connection result
     * @return Last ConnectionResult
     */
    const ConnectionResult& GetLastResult() const;
    
private:
    // Private constructor for singleton pattern
    RoomConnectionHandler();
    
    // Prevent copying
    RoomConnectionHandler(const RoomConnectionHandler&);
    RoomConnectionHandler& operator=(const RoomConnectionHandler&);
    
    /**
     * @brief Set error state for connection operations
     * @param error Error code to set
     * @param result ConnectionResult to update
     */
    void SetError(AutoConnectError error, ConnectionResult& result);
    
    /**
     * @brief Check if the network system is ready for connection
     * @return true if network system is ready, false otherwise
     */
    bool IsNetworkSystemReady() const;
    
    /**
     * @brief Initiate actual room connection using network infrastructure
     * 
     * Uses the existing Network::Mgr and RKNet infrastructure to
     * initiate a connection to the specified room.
     * 
     * @param roomId Room ID to connect to
     * @param result ConnectionResult to update
     * @return true if connection initiation successful, false otherwise
     */
    bool InitiateRoomConnection(const char* roomId, ConnectionResult& result);
    
    /**
     * @brief Wait for connection completion or timeout
     * 
     * Waits for the connection attempt to complete or timeout.
     * Updates the connection result accordingly.
     * 
     * @param timeout Connection timeout in milliseconds
     * @param result ConnectionResult to update
     * @return true if connection completed successfully, false otherwise
     */
    bool WaitForConnection(u32 timeout, ConnectionResult& result);
    
    // Member variables
    bool isInitialized;                // Whether Initialize() has been called
    bool isConnecting;                 // Whether a connection is in progress
    ConnectionResult lastResult;       // Last connection result
    u32 connectionStartTime;          // Time when connection attempt started
    
    // Singleton instance
    static RoomConnectionHandler* sInstance;
};

} // namespace AutoConnect
} // namespace Pulsar

#endif