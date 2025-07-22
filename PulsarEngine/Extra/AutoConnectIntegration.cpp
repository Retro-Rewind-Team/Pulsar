#include <Extra/AutoConnectIntegration.hpp>
#include <Extra/DolphinRoomConnector.hpp>
#include <Extra/LicenseSlotManager.hpp>
#include <Extra/RoomConnectionHandler.hpp>
#include <PulsarSystem.hpp>
#include <Settings/Settings.hpp>
#include <MarioKartWii/Scene/GameScene.hpp>
#include <MarioKartWii/Scene/SceneManager.hpp>
#include <MarioKartWii/UI/SectionMgr/SectionMgr.hpp>
#include <Dolphin/DolphinIOS.hpp>

namespace Pulsar {
namespace AutoConnect {

// Initialize static members
bool Integration::isInitialized = false;
bool Integration::hasAttemptedConnection = false;

// Setting index in the RRHost settings page
const u32 AUTOCONNECT_SETTING_INDEX = SETTINGRR3_RADIO_AUTOCONNECT;

bool Integration::InitializeSystem(System& system) {
    if (isInitialized) {
        return true;
    }

    // Initialize the DolphinRoomConnector
    DolphinRoomConnector& connector = DolphinRoomConnector::GetInstance();
    bool initResult = connector.Initialize();
    
    if (!initResult) {
        // Initialization failed, but we'll continue with normal startup
        return false;
    }
    
    // Process room parameter if available
    if (connector.IsEnabled()) {
        connector.ProcessRoomParameter();
    }
    
    isInitialized = true;
    return true;
}

bool Integration::ProcessSceneTransition(u32 sceneId) {
    // Only attempt auto-connection once per session
    if (hasAttemptedConnection) {
        return false;
    }
    
    // Check if auto-connection is enabled
    if (!IsEnabledInSettings()) {
        return false;
    }
    
    // Get the connector instance
    DolphinRoomConnector& connector = DolphinRoomConnector::GetInstance();
    
    // Check if connector is enabled and has a room parameter
    if (!connector.IsEnabled()) {
        return false;
    }
    
    // Only attempt connection at the title screen
    if (sceneId != SCENE_ID_TITLE_SCREEN) {
        return false;
    }
    
    // Check if we have a valid room ID
    const RoomConnectionConfig& config = connector.GetConfig();
    if (config.roomId[0] == '\0') {
        return false;
    }
    
    // Mark that we've attempted connection
    hasAttemptedConnection = true;
    
    // Select and login to a license slot
    LicenseSlotManager& licenseManager = LicenseSlotManager::GetInstance();
    if (!licenseManager.Initialize()) {
        return false;
    }
    
    u8 selectedSlot = 0xFF;
    if (!licenseManager.SelectAvailableSlot(config.targetLicenseSlot, selectedSlot)) {
        return false;
    }
    
    if (!licenseManager.AutoLogin(selectedSlot)) {
        return false;
    }
    
    // Attempt room connection
    RoomConnectionHandler& handler = RoomConnectionHandler::GetInstance();
    if (!handler.Initialize()) {
        return false;
    }
    
    ConnectionResult result = handler.ConnectToRoom(config.roomId, config.connectionTimeout);
    
    // Return whether connection was successful
    return (result.finalState == CONNECTION_SUCCESS);
}

bool Integration::IsEnabledInSettings() {
    // Check if auto-connection is enabled in settings
    const Settings::Mgr& settings = Settings::Mgr::Get();
    return settings.GetUserSettingValue(Settings::SETTINGSTYPE_RRHOST, AUTOCONNECT_SETTING_INDEX) == AUTOCONNECT_ENABLED;
}

void Integration::SetEnabledInSettings(bool enabled) {
    // Set auto-connection enabled state in settings
    Settings::Mgr* settings = Settings::Mgr::sInstance;
    if (settings) {
        settings->SetUserSettingValue(Settings::SETTINGSTYPE_RRHOST, AUTOCONNECT_SETTING_INDEX, enabled ? AUTOCONNECT_ENABLED : AUTOCONNECT_DISABLED);
    }
}

// Hook into System::Init() to initialize auto-connection
static bool InitializeAutoConnect(System& system) {
    return Integration::InitializeSystem(system);
}

// Register hooks with the system
class AutoConnectHooks {
public:
    AutoConnectHooks() {
        // Register initialization hook
        System::Inherit::CreateFunc originalCreate = System::inherit ? System::inherit->create : 0;
        System::inherit = new System::Inherit([originalCreate]() -> System* {
            System* system = originalCreate ? originalCreate() : new System();
            InitializeAutoConnect(*system);
            return system;
        });
    }
};

// Create hooks instance to register hooks
static AutoConnectHooks autoConnectHooks;

} // namespace AutoConnect
} // namespace Pulsar