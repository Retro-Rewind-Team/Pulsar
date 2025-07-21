#ifndef _PULSAR_LICENSE_SLOT_MANAGER_
#define _PULSAR_LICENSE_SLOT_MANAGER_

#include <types.hpp>
#include <Extra/AutoConnectTypes.hpp>

namespace Pulsar {
namespace AutoConnect {

/**
 * @brief License slot management for auto-connection
 * 
 * This class handles automatic license slot selection and login
 * for the auto-connection system. It integrates with the existing
 * RKSYS license management system to find available slots and
 * automatically log in to them.
 */
class LicenseSlotManager {
public:
    /**
     * @brief Get the singleton instance of LicenseSlotManager
     * @return Reference to the singleton instance
     */
    static LicenseSlotManager& GetInstance();
    
    /**
     * @brief Initialize the license slot manager
     * @return true if initialization successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Select an available license slot for auto-connection
     * 
     * Finds the first available license slot for auto-connection.
     * If a specific slot is requested, attempts to use that slot.
     * If no slot is available, creates or uses a default slot.
     * 
     * @param preferredSlot Preferred slot ID (0xFF for auto-select)
     * @param selectedSlot Output parameter for selected slot ID
     * @return true if slot selection successful, false otherwise
     */
    bool SelectAvailableSlot(u8 preferredSlot, u8& selectedSlot);
    
    /**
     * @brief Automatically log in to the specified license slot
     * 
     * Performs automatic login to the specified license slot.
     * This includes setting the current license and initializing
     * any necessary game systems.
     * 
     * @param slotId License slot ID to log in to
     * @return true if login successful, false otherwise
     */
    bool AutoLogin(u8 slotId);
    
    /**
     * @brief Check if a specific license slot is available
     * 
     * Determines if the specified license slot is available for use.
     * A slot is considered available if it exists and is initialized.
     * 
     * @param slotId License slot ID to check
     * @return true if slot is available, false otherwise
     */
    bool IsSlotAvailable(u8 slotId) const;
    
    /**
     * @brief Get the maximum number of license slots
     * @return Maximum number of license slots
     */
    u8 GetMaxSlots() const;
    
    /**
     * @brief Get the current license slot
     * @return Current license slot ID, or 0xFF if none selected
     */
    u8 GetCurrentSlot() const;
    
    /**
     * @brief Check if any license slots are available
     * @return true if at least one slot is available, false otherwise
     */
    bool HasAvailableSlots() const;
    
private:
    // Private constructor for singleton pattern
    LicenseSlotManager();
    
    // Prevent copying
    LicenseSlotManager(const LicenseSlotManager&);
    LicenseSlotManager& operator=(const LicenseSlotManager&);
    
    /**
     * @brief Find the first available license slot
     * @return First available slot ID, or 0xFF if none available
     */
    u8 FindFirstAvailableSlot() const;
    
    /**
     * @brief Create a default license slot if none available
     * @param slotId Output parameter for created slot ID
     * @return true if slot creation successful, false otherwise
     */
    bool CreateDefaultSlot(u8& slotId);
    
    /**
     * @brief Set error state for license operations
     * @param error Error code to set
     */
    void SetError(AutoConnectError error);
    
    // Member variables
    bool isInitialized;                // Whether Initialize() has been called
    u8 currentSlot;                    // Currently selected license slot
    AutoConnectError lastError;        // Last error that occurred
    
    // Constants
    static const u8 MAX_LICENSE_SLOTS = 4;  // Maximum number of license slots
    static const u8 INVALID_SLOT = 0xFF;    // Invalid slot ID
    
    // Singleton instance
    static LicenseSlotManager* sInstance;
};

} // namespace AutoConnect
} // namespace Pulsar

#endif