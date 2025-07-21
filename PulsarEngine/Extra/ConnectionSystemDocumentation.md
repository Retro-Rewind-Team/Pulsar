# Connection System Documentation

## Overview

The PulsarEngine network system handles room connections through a multi-layered architecture that manages packet communication, room selection, and player coordination.

## Key Components

### 1. Network Manager (`Network::Mgr`)
- Central coordinator for network-related functionality
- Manages host context, deny types, and race configuration
- Stores last played tracks for track blocking functionality
- Located in `PulsarEngine/Network/Network.hpp`

### 2. Room Packet System (`PulROOM`)
- Handles room-level communication between host and clients
- Manages host settings, extended teams, and race configuration
- Processes START messages (messageType = 1) for race initiation
- Located in `PulsarEngine/Network/PulROOM.cpp`

### 3. Track Selection System (`PulSELECT`)
- Manages track voting and selection process
- Handles course ID conversion between vanilla and Pulsar formats
- Coordinates track decision making between host and clients
- Located in `PulsarEngine/Network/PulSELECT.hpp/cpp`

### 4. RKNet Integration
- Uses Mario Kart Wii's native RKNet system for low-level networking
- `RKNet::Controller` manages connection state and available AIDs
- `RKNet::ControllerSub` handles sub-connection details

## Connection Flow

### Room Connection Process:
1. **Initialization**: Network manager initializes with default settings
2. **Room Join**: Player attempts to join room using room ID/key
3. **Host Communication**: Exchange of PulROOM packets with host settings
4. **Track Selection**: PulSELECT packet exchange for track voting
5. **Race Start**: Final coordination and transition to race

### Packet Types:
- **PulROOM**: Room-level settings and coordination
- **PulSELECT**: Track selection and voting
- **RKNet base packets**: Low-level network communication

## Room ID System

### Current Implementation:
- Rooms use hardcoded keys (see `RoomKey.hpp`)
- Room identification through network protocols
- Host/client coordination through AID (Address ID) system

### Key Data Structures:
- `hostContext` and `hostContext2`: Bit-packed host settings
- `availableAids`: Bitmask of connected players
- `racesPerGP`: Number of races per Grand Prix

## Integration Points for Auto-Connect

### Safe Integration Areas:
1. **System Initialization**: Hook into `System::Init()` for setup
2. **Network Manager**: Extend existing `Network::Mgr` functionality
3. **Dolphin IOS**: Use existing Dolphin communication infrastructure
4. **Settings System**: Leverage existing settings framework

### Areas to Avoid Modifying:
- Core RKNet packet structures
- Mario Kart Wii headers
- Low-level network protocols
- Existing room connection logic

## Error Handling Patterns

The system uses consistent error handling:
- Boolean return values for success/failure
- Graceful degradation on errors
- Logging for debugging purposes
- Fallback to normal game flow on failures

## Thread Safety

- Network operations are generally single-threaded
- State management through central managers
- Packet processing in main game loop
- No explicit synchronization primitives used

## Performance Considerations

- Minimal memory allocation during network operations
- Efficient packet processing
- Lazy initialization of network components
- Optimized for low-latency communication