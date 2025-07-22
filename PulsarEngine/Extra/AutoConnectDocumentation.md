# Dolphin Room Auto-Connect Feature

## Overview

The Dolphin Room Auto-Connect feature allows players to automatically connect to a specific room when launching Mario Kart Wii through Dolphin emulator. By providing a Room ID as a launch parameter, the game will automatically:

1. Select an appropriate license slot
2. Log in to the selected license slot
3. Connect to the specified room

This streamlines the process of joining specific rooms without manual navigation through menus.

## Usage

### Enabling Auto-Connect

1. In the game settings, navigate to the RRHost settings page
2. Set the "Auto-Connect" option to "Enabled"

### Launching with Room ID

To use the auto-connect feature, launch Dolphin with the room parameter:

```
dolphin.exe --room-id=YOUR_ROOM_ID
```

Replace `YOUR_ROOM_ID` with the actual room ID you want to connect to.

## Technical Details

### Components

The auto-connect system consists of several components:

- **DolphinRoomConnector**: Main coordinator class that manages the auto-connection workflow
- **LicenseSlotManager**: Handles automatic license slot selection and login
- **RoomConnectionHandler**: Manages the actual room connection process
- **AutoConnectIntegration**: Integrates the auto-connection system with PulsarEngine

### Integration Points

The auto-connect system integrates with PulsarEngine at the following points:

1. **System Initialization**: During `System::Init()`, the auto-connector is initialized
2. **Scene Transitions**: During scene transitions, auto-connection is triggered at appropriate times
3. **Settings System**: Auto-connect settings are stored in the existing settings system

### Error Handling

The auto-connect system includes comprehensive error handling for various failure scenarios:

- Invalid room ID format
- Missing room parameter
- No available license slots
- Connection failures
- Network errors

If any error occurs during the auto-connection process, the system will gracefully fall back to normal game startup.

## Compatibility

The auto-connect feature is:

- Only available when running in Dolphin emulator
- Completely disabled on console hardware
- Compatible with existing network infrastructure
- Non-intrusive to normal gameplay when not in use