#ifndef _PULSAR_AUTOCONNECT_INTEGRATION_
#define _PULSAR_AUTOCONNECT_INTEGRATION_

#include <types.hpp>
#include <Extra/DolphinRoomConnector.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace AutoConnect {

/**
 * @brief Integration class for auto-connection system
 * 
 * This class handles the integration of the auto-connection system with
 * the PulsarEngine system initialization and scene transitions.
 */
class Integration {
public:
    /**
     * @brief Initialize the auto-connection system during PulsarSystem initialization
     * 
     * This function is called during System::Init() to initialize the
     * DolphinRoomConnector and prepare for auto-connection.
     * 
     * @param system Reference to the PulsarSystem instance
     * @return true if initialization successful, false otherwise
     */
    static bool InitializeSystem(System& system);

    /**
     * @brief Process scene transitions for auto-connection
     * 
     * This function is called during scene transitions to trigger
     * auto-connection at appropriate times.
     * 
     * @param sceneId Current scene ID
     * @return true if auto-connection was triggered, false otherwise
     */
    static bool ProcessSceneTransition(u32 sceneId);

    /**
     * @brief Check if auto-connection is enabled in settings
     * @return true if auto-connection is enabled, false otherwise
     */
    static bool IsEnabledInSettings();

    /**
     * @brief Set auto-connection enabled state in settings
     * @param enabled Whether auto-connection should be enabled
     */
    static void SetEnabledInSettings(bool enabled);

private:
    // Private constructor to prevent instantiation
    Integration() {}
    
    // Prevent copying
    Integration(const Integration&);
    Integration& operator=(const Integration&);

    // Track whether initialization has been performed
    static bool isInitialized;
    
    // Track whether auto-connection has been attempted
    static bool hasAttemptedConnection;
};

} // namespace AutoConnect
} // namespace Pulsar

#endif