#include <Extra/EmulatorDetection.hpp>
#include <Extra/DolphinRoomConnector.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace AutoConnect {

/**
 * @brief Initialize auto-connect system during PulsarEngine startup
 * 
 * This function is called during PulsarEngine initialization to set up
 * the auto-connect system. It initializes the emulator detection and
 * room connector components.
 * 
 * @return true if initialization successful, false otherwise
 */
bool InitializeAutoConnectSystem() {
    // Initialize emulator detection first
    EmulatorDetection& emulatorDetection = EmulatorDetection::GetInstance();
    if (!emulatorDetection.Initialize()) {
        // Failed to initialize emulator detection
        return false;
    }
    
    // Check if we're running on console hardware
    if (!emulatorDetection.IsDolphinEmulator()) {
        // Running on console hardware - completely disable auto-connect
        // No need to initialize other components
        return true;
    }
    
    // Check if IOS communication is available
    if (!emulatorDetection.IsIOSCommunicationAvailable()) {
        // IOS communication not available - disable auto-connect
        return true;
    }
    
    // Initialize room connector
    DolphinRoomConnector& connector = DolphinRoomConnector::GetInstance();
    if (!connector.Initialize()) {
        // Failed to initialize room connector
        return false;
    }
    
    // Process room parameter if available
    if (connector.IsEnabled()) {
        connector.ProcessRoomParameter();
    }
    
    return true;
}

/**
 * @brief Hook function called during system initialization
 * 
 * This function is called by the PulsarEngine initialization system
 * to set up auto-connect functionality.
 */
void OnSystemInit() {
    InitializeAutoConnectSystem();
}

/**
 * @brief Register auto-connect system with PulsarEngine
 * 
 * This function registers the auto-connect system with PulsarEngine
 * so that it's properly initialized during startup.
 */
void RegisterAutoConnectSystem() {
    // Register initialization function with PulsarEngine
    System::RegisterInitCallback(OnSystemInit);
}

// Automatically register the auto-connect system when this file is loaded
static bool sAutoConnectRegistered = (RegisterAutoConnectSystem(), true);

} // namespace AutoConnect
} // namespace Pulsar