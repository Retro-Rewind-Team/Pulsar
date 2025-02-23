#include <UI/ExtendedTeamSelect/ExtendedTeamManager.hpp>
#include <MarioKartWii/UI/Page/Other/FriendRoom.hpp>
#include <MarioKartWii/UI/Page/Other/SELECTStageMgr.hpp>

namespace Pulsar {
namespace UI {

ExtendedTeamManager* ExtendedTeamManager::sInstance = nullptr;

ExtendedTeamManager::ExtendedTeamManager() {
    this->ResetPlayers();
    this->isHost = false;
    this->status = STATUS_NONE;
}

void ExtendedTeamManager::CreateInstance(ExtendedTeamManager* obj) {
    sInstance = obj;
}

void ExtendedTeamManager::DestroyInstance() {
    if (sInstance) {
        sInstance = nullptr;
    }
}

void ExtendedTeamManager::SendStartRacePacket() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    if (sub->localAid == sub->hostAid) {
        friendRoomManager->lastMessageId++;

        RKNet::ROOMPacket packet;
        packet.messageType = MSG_TYPE_START_RACE;
        packet.message = 0;
        packet.unknown_0x3 = friendRoomManager->lastMessageId;
            
         for (int i = 0; i < 12; ++i) {
            if (i != sub->localAid) {
                RKNet::ROOMHandler::sInstance->toSendPackets[i] = packet;
            }
        }

        friendRoomManager->networkManager.lastSentPacket = packet;
        friendRoomManager->networkManager.localAid = sub->localAid;

        if (this->status == STATUS_SELECTING) {
            this->waitingTimer.SetInitial(2.5f);
            this->waitingTimer.isActive = true;

            this->status = STATUS_WAITING_POST;
        }
    }
}

void ExtendedTeamManager::SendUpdateTeamsPacket() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    if (sub->localAid == sub->hostAid) {
        friendRoomManager->lastMessageId++;

        RKNet::ROOMPacket packet;
        packet.messageType = MSG_TYPE_UPDATE_TEAMS;
        packet.message = 0;
        packet.unknown_0x3 = friendRoomManager->lastMessageId;
            
        for (int i = 0; i < 12; ++i) {
            if (i != sub->localAid) {
                RKNet::ROOMHandler::sInstance->toSendPackets[i] = packet;
            }
        }

        friendRoomManager->networkManager.lastSentPacket = packet;
        friendRoomManager->networkManager.localAid = sub->localAid;
    }
}

void ExtendedTeamManager::SendPingPacket() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    friendRoomManager->lastMessageId++;

    RKNet::ROOMPacket packet;
    packet.messageType = MSG_TYPE_PING;
    packet.message = 0;
    packet.unknown_0x3 = friendRoomManager->lastMessageId;

    RKNet::ROOMHandler::sInstance->toSendPackets[sub->hostAid] = packet;

    friendRoomManager->networkManager.lastSentPacket = packet;
    friendRoomManager->networkManager.localAid = sub->localAid;
}

void ExtendedTeamManager::SendAckStartRacePacket() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    friendRoomManager->lastMessageId++;

    RKNet::ROOMPacket packet;
    packet.messageType = MSG_TYPE_ACK_START_RACE;
    packet.message = 0;
    packet.unknown_0x3 = friendRoomManager->lastMessageId;

    RKNet::ROOMHandler::sInstance->toSendPackets[sub->hostAid] = packet;

    friendRoomManager->networkManager.lastSentPacket = packet;
    friendRoomManager->networkManager.localAid = sub->localAid;
}

bool ExtendedTeamManager::AreAllOtherPlayersActive(u8 localAid) {
    for (int i = 0; i < 12; i++) {
        if (this->players[i].playerIdx != 0xFF && this->players[i].aid != localAid && !this->players[i].active) {
            return false;
        }
    }
    return true;
}

bool ExtendedTeamManager::AreAllOtherPlayersDone(u8 localAid) {
    for (int i = 0; i < 12; i++) {
        if (this->players[i].playerIdx != 0xFF && this->players[i].aid != localAid && !this->players[i].done) {
            return false;
        }
    }
    return true;
}

void ExtendedTeamManager::Update() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    this->isHost = RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST;
    u8 localAid = RKNet::Controller::sInstance->subs[RKNet::Controller::sInstance->currentSub].localAid;
    if (!this->isHost) {
        if (this->status == STATUS_NONE) {
            if (friendRoomManager->friendRoomIsEnding) {
                this->status = STATUS_SELECTING;
            }
        } else if (this->status == STATUS_SELECTING) {
            if (!this->lastUpdateTimer.isActive) {
                this->lastUpdateTimer.SetInitial(0.666f);
                this->lastUpdateTimer.isActive = true;

                this->SendPingPacket();
            }
        } else if (this->status == STATUS_DONE) {
            if (!this->lastUpdateTimer.isActive) {
                this->lastUpdateTimer.SetInitial(0.666f);
                this->lastUpdateTimer.isActive = true;

                this->SendAckStartRacePacket();
            }
        }
    } else {
        if (this->status == STATUS_NONE) {
            if (friendRoomManager->friendRoomIsEnding) {
                this->status = STATUS_WAITING_PRE;
                this->waitingTimer.SetInitial(5.0f);
                this->waitingTimer.isActive = true;
            }
        } else if (this->status == STATUS_WAITING_PRE) {
            if (!this->waitingTimer.isActive) {
                this->status = STATUS_SELECTING;
            } else {
                if (this->AreAllOtherPlayersActive(localAid)) {
                    this->status = STATUS_SELECTING;
                    this->waitingTimer.isActive = false;
                }
            }
        } else if (this->status == STATUS_SELECTING) {
            // ...
        } else if (this->status == STATUS_WAITING_POST) {
            if (!this->waitingTimer.isActive) {
                this->status = STATUS_DONE;
            } else {
                if (this->AreAllOtherPlayersDone(localAid)) {
                    this->status = STATUS_DONE;
                    this->waitingTimer.isActive = false;
                }
            }

            if (!this->lastUpdateTimer.isActive) {
                this->lastUpdateTimer.SetInitial(0.666f);
                this->lastUpdateTimer.isActive = true;

                this->SendStartRacePacket();
            }
        }
    }

    this->lastUpdateTimer.Update();
    this->waitingTimer.Update();
}

void ExtendedTeamManager::VotePageSync() {
    ExtendedTeamPlayer newPlayers[12];
    for (int i = 0; i < 12; i++) {
        newPlayers[i].playerIdx = 0xFF;
        newPlayers[i].miiIdx = 0xFF;
        newPlayers[i].aid = 0xFF;
        newPlayers[i].playerIdOnConsole = 0xFF;
        newPlayers[i].team = static_cast<ExtendedTeamID>(i % TEAM_COUNT);
    }

    Pages::SELECTStageMgr* voteMgr = SectionMgr::sInstance->curSection->Get<Pages::SELECTStageMgr>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;

    for (int i = 0; i < 12; i++) {
        u8 aid = controller->aidsBelongingToPlayerIds[i];
        if (aid == 0xFF) {
            continue;
        }
    
        u8 localPlayerId;
        if (i < 1) {
            localPlayerId = 0;
        } else {
            u8 aid2P = controller->aidsBelongingToPlayerIds[i - 1];
            if (aid != aid2P) {
                localPlayerId = 0;
            } else {
                localPlayerId = 1;
            }
        }

        for (int j = 0; j < 12; j++) {
            if (voteMgr->infos[j].aid == aid && voteMgr->infos[j].hudSlotid == localPlayerId) {
                newPlayers[i].playerIdx = i;
                newPlayers[i].miiIdx = aid * 2 + localPlayerId;
                newPlayers[i].aid = aid;
                newPlayers[i].playerIdOnConsole = localPlayerId;
                newPlayers[i].team = this->GetPlayerTeamByAID(aid, localPlayerId);
                break;
            }
        }
    }

    memcpy(this->players, newPlayers, sizeof(ExtendedTeamPlayer) * 12);
}

void ExtendedTeamManager::Reset() {
    this->status = STATUS_NONE;
    this->isHost = false;
    this->lastUpdateTimer.isActive = false;
    this->waitingTimer.isActive = false;
}

void ExtendedTeamManager::ResetPlayers() {
    for (int i = 0; i < 12; i++) {
        this->players[i].playerIdx = 0xFF;
        this->players[i].miiIdx = 0xFF;
        this->players[i].aid = 0xFF;
        this->players[i].playerIdOnConsole = 0xFF;
        this->players[i].team = static_cast<ExtendedTeamID>(i % TEAM_COUNT);
    }
}

} // namespace UI
} // namespace Pulsar