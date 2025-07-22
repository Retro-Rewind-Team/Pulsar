#ifndef _PULSAR_EMULATOR_DETECTION_
#define _PULSAR_EMULATOR_DETECTION_

#include <types.hpp>

namespace Pulsar {
namespace AutoConnect {

/**
 * @brief Class for robust emulator detection and compatibility safeguards
 * 
 * This class provides advanced detection methods to determine if the game
 * is running in Dolphin emulator or on console hardware. It implements
 * multiple detection strategies for reliability and includes safety checks
 * to ensure zero impact on console gameplay.
 */
class EmulatorDetection {
public:
    /**
     * @brief Get the singleton instance of EmulatorDetection
     * @return Reference to the singleton instance
     */
    static EmulatorDetection& GetInstance();
    
    /**
     * @brief Initialize the emulator detection system
     * 
     * Performs initial detection checks and sets up the detection system.
     * This should be called early in the application lifecycle.
     * 
     * @return true if initialization successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Check if running in Dolphin emulator
     * 
     * Performs a robust check using multiple detection methods to determine
     * if the game is running in Dolphin emulator. This is more reliable than
     * the basic Dolphin::IsEmulator() check.
     * 
     * @return true if running in Dolphin emulator, false if on console
     */
    bool IsDolphinEmulator() const;
    
    /**
     * @brief Check if IOS communication is available
     * 
     * Verifies that Dolphin IOS communication is available and working.
     * This is used to ensure that auto-connection features can function.
     * 
     * @return true if IOS communication is available, false otherwise
     */
    bool IsIOSCommunicationAvailable() const;
    
    /**
     * @brief Check if auto-connection should be enabled
     * 
     * Determines if auto-connection features should be enabled based on
     * emulator detection, IOS communication availability, and other factors.
     * 
     * @return true if auto-connection should be enabled, false otherwise
     */
    bool ShouldEnableAutoConnect() const;
    
    /**
     * @brief Get confidence level in emulator detection
     * 
     * Returns a value indicating how confident the detection system is
     * that the game is running in an emulator. Higher values indicate
     * greater confidence.
     * 
     * @return Confidence level (0-100)
     */
    u8 GetDetectionConfidence() const;
    
    /**
     * @brief Force disable auto-connection
     * 
     * Manually disables auto-connection regardless of detection results.
     * This can be used as a safety override.
     */
    void ForceDisableAutoConnect();
    
    /**
     * @brief Check if auto-connection is forcibly disabled
     * @return true if auto-connection is forcibly disabled, false otherwise
     */
    bool IsAutoConnectForcedDisabled() const;

private:
    // Private constructor for singleton pattern
    EmulatorDetection();
    
    // Prevent copying
    EmulatorDetection(const EmulatorDetection&);
    EmulatorDetection& operator=(const EmulatorDetection&);
    
    /**
     * @brief Perform primary emulator detection
     * 
     * Uses the primary detection method (Dolphin IOS check) to determine
     * if running in Dolphin emulator.
     * 
     * @return true if primary detection indicates emulator, false otherwise
     */
    bool PerformPrimaryDetection();
    
    /**
     * @brief Perform secondary emulator detection
     * 
     * Uses secondary detection methods to verify emulator status.
     * This includes timing checks, memory patterns, and other heuristics.
     * 
     * @return true if secondary detection indicates emulator, false otherwise
     */
    bool PerformSecondaryDetection();
    
    /**
     * @brief Check for hardware-specific features
     * 
     * Tests for the presence of hardware features that would only be
     * available on actual console hardware.
     * 
     * @return true if hardware features detected (console), false otherwise
     */
    bool CheckHardwareFeatures();
    
    /**
     * @brief Verify IOS communication reliability
     * 
     * Tests the reliability of IOS communication by performing
     * multiple test operations.
     * 
     * @return true if IOS communication is reliable, false otherwise
     */
    bool VerifyIOSCommunication();
    
    // Member variables
    bool isInitialized;                // Whether Initialize() has been called
    bool isPrimaryDetectionPositive;   // Result of primary detection
    bool isSecondaryDetectionPositive; // Result of secondary detection
    bool isIOSCommunicationReliable;   // Whether IOS communication is reliable
    bool isForceDisabled;              // Whether auto-connect is forcibly disabled
    u8 detectionConfidence;           // Confidence level in detection (0-100)
    
    // Singleton instance
    static EmulatorDetection* sInstance;
};

} // namespace AutoConnect
} // namespace Pulsar

#endif