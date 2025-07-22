#include <Extra/EmulatorDetection.hpp>
#include <Dolphin/DolphinIOS.hpp>

namespace Pulsar {
namespace AutoConnect {

// Static instance initialization
EmulatorDetection* EmulatorDetection::sInstance = 0;

EmulatorDetection::EmulatorDetection()
    : isInitialized(false)
    , isPrimaryDetectionPositive(false)
    , isSecondaryDetectionPositive(false)
    , isIOSCommunicationReliable(false)
    , isForceDisabled(false)
    , detectionConfidence(0) {
}

EmulatorDetection& EmulatorDetection::GetInstance() {
    if (sInstance == 0) {
        sInstance = new EmulatorDetection();
    }
    return *sInstance;
}

bool EmulatorDetection::Initialize() {
    if (isInitialized) {
        return true;
    }

    // Perform primary detection (Dolphin IOS check)
    isPrimaryDetectionPositive = PerformPrimaryDetection();
    
    // If primary detection is negative, we're definitely on console
    if (!isPrimaryDetectionPositive) {
        detectionConfidence = 100; // 100% confident we're on console
        isInitialized = true;
        return true;
    }
    
    // Perform secondary detection for additional confidence
    isSecondaryDetectionPositive = PerformSecondaryDetection();
    
    // Check hardware features (will be false on emulator, true on console)
    bool hasHardwareFeatures = CheckHardwareFeatures();
    
    // Verify IOS communication reliability
    isIOSCommunicationReliable = VerifyIOSCommunication();
    
    // Calculate detection confidence
    if (isPrimaryDetectionPositive && isSecondaryDetectionPositive && !hasHardwareFeatures) {
        detectionConfidence = 95; // Very high confidence we're in Dolphin
    } else if (isPrimaryDetectionPositive && isSecondaryDetectionPositive) {
        detectionConfidence = 85; // High confidence we're in Dolphin
    } else if (isPrimaryDetectionPositive && !hasHardwareFeatures) {
        detectionConfidence = 80; // Good confidence we're in Dolphin
    } else if (isPrimaryDetectionPositive) {
        detectionConfidence = 70; // Moderate confidence we're in Dolphin
    } else {
        detectionConfidence = 0;  // Definitely not in Dolphin
    }
    
    isInitialized = true;
    return true;
}

bool EmulatorDetection::IsDolphinEmulator() const {
    if (!isInitialized) {
        // If not initialized, fall back to basic check
        return Dolphin::IsEmulator();
    }
    
    // We consider it Dolphin if primary detection is positive and confidence is high enough
    return isPrimaryDetectionPositive && (detectionConfidence >= 70);
}

bool EmulatorDetection::IsIOSCommunicationAvailable() const {
    if (!isInitialized) {
        return false;
    }
    
    // IOS communication is available if we're in Dolphin and communication is reliable
    return IsDolphinEmulator() && isIOSCommunicationReliable;
}

bool EmulatorDetection::ShouldEnableAutoConnect() const {
    if (!isInitialized) {
        return false;
    }
    
    // Don't enable if forcibly disabled
    if (isForceDisabled) {
        return false;
    }
    
    // Only enable if we're confident we're in Dolphin and IOS communication is reliable
    return IsDolphinEmulator() && IsIOSCommunicationAvailable();
}

u8 EmulatorDetection::GetDetectionConfidence() const {
    return detectionConfidence;
}

void EmulatorDetection::ForceDisableAutoConnect() {
    isForceDisabled = true;
}

bool EmulatorDetection::IsAutoConnectForcedDisabled() const {
    return isForceDisabled;
}

bool EmulatorDetection::PerformPrimaryDetection() {
    // Use the basic Dolphin IOS check as primary detection method
    return Dolphin::IsEmulator();
}

bool EmulatorDetection::PerformSecondaryDetection() {
    // Secondary detection uses additional heuristics
    
    // Check 1: Try to get Dolphin version
    char versionBuffer[32] = {0};
    bool hasVersion = Dolphin::GetVersion(versionBuffer, sizeof(versionBuffer));
    
    // Check 2: Try to get elapsed time
    u32 elapsedTime = 0;
    bool hasElapsedTime = Dolphin::GetElapsedTime(elapsedTime);
    
    // Check 3: Try to get system time
    u64 systemTime = 0;
    bool hasSystemTime = Dolphin::GetSystemTime(systemTime);
    
    // We consider secondary detection positive if at least 2 of 3 checks pass
    int passCount = 0;
    if (hasVersion) passCount++;
    if (hasElapsedTime) passCount++;
    if (hasSystemTime) passCount++;
    
    return passCount >= 2;
}

bool EmulatorDetection::CheckHardwareFeatures() {
    // This function checks for hardware features that would only be present on real console
    // For simplicity, we'll implement a basic check that always returns false
    // In a real implementation, this would check for specific hardware features
    
    // Example hardware checks could include:
    // - Checking for specific memory patterns at hardware registers
    // - Testing timing characteristics of certain operations
    // - Checking for console-specific peripherals
    
    return false;
}

bool EmulatorDetection::VerifyIOSCommunication() {
    if (!isPrimaryDetectionPositive) {
        return false;
    }
    
    // Perform multiple IOS communication tests to verify reliability
    
    // Test 1: Open and close Dolphin device
    int fd = Dolphin::OpenDolphin();
    bool openSuccess = (fd >= 0);
    Dolphin::CloseDolphin();
    
    // Test 2: Try to get version
    char versionBuffer[32] = {0};
    bool versionSuccess = Dolphin::GetVersion(versionBuffer, sizeof(versionBuffer));
    
    // Test 3: Try to get elapsed time
    u32 elapsedTime = 0;
    bool timeSuccess = Dolphin::GetElapsedTime(elapsedTime);
    
    // Consider IOS communication reliable if all tests pass
    return openSuccess && versionSuccess && timeSuccess;
}

} // namespace AutoConnect
} // namespace Pulsar