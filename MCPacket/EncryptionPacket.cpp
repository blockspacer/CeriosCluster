//
//  EncryptionPacket.cpp
//  MCPacket
//
//  Created by Andrew Querol on 8/31/15.
//  Copyright © 2015 Andrew Querol. All rights reserved.
//

#include "EncryptionPacket.hpp"
#include "AbstractClient.hpp"
#include <openssl/ssl.h>

Cerios::Server::EncryptionPacket::EncryptionPacket(Cerios::Server::Side side, std::shared_ptr<Cerios::Server::Packet> packetInProgress) : Packet(packetInProgress), serverId("") {
    if (side == Side::SERVER) { // If packet is from the server going to the client, not priority to finish client side receipt processing.
        // Not complete
        std::int32_t stringLength;
        Cerios::Server::Packet::readVarIntFromBuffer(&stringLength, &this->rawPayload, true);
        if (this->rawPayload.size() >= stringLength) {
            this->serverId = std::string(this->rawPayload.begin(), this->rawPayload.begin() + stringLength);
        }
        this->rawPayload.clear();
    } else {
        // From the client received on the server
        std::int32_t sharedSecretLength;
        Cerios::Server::Packet::readVarIntFromBuffer(&sharedSecretLength, &this->rawPayload, true);
        std::copy(reinterpret_cast<std::uint8_t *>(this->rawPayload.data()), reinterpret_cast<std::uint8_t *>(this->rawPayload.data()) + sharedSecretLength, std::back_inserter(this->sealedSharedSecret));
        this->rawPayload.erase(this->rawPayload.begin(), this->rawPayload.begin() + sharedSecretLength);
        
        std::int32_t verifyTokenLength;
        Cerios::Server::Packet::readVarIntFromBuffer(&verifyTokenLength, &this->rawPayload, true);
        std::copy(reinterpret_cast<std::uint8_t *>(this->rawPayload.data()), reinterpret_cast<std::uint8_t *>(this->rawPayload.data()) + verifyTokenLength, std::back_inserter(this->sealedVerifyToken));
    }
    this->rawPayload.clear();
}

Cerios::Server::EncryptionPacket::EncryptionPacket(Cerios::Server::Side side) : Packet(0x01), serverId("") {
}

void Cerios::Server::EncryptionPacket::serializePacket(Cerios::Server::Side sideSending) {
    Packet::serializePacket(sideSending);
    if (sideSending == Cerios::Server::Side::SERVER) {
        this->writeVarIntToBuffer(static_cast<std::int32_t>(this->serverId.size()));
        std::copy(this->serverId.data(), this->serverId.data() + this->serverId.size(), std::back_inserter(this->rawPayload));
        std::size_t neededLength = i2d_PUBKEY(this->keyPair.get(), NULL);
        this->writeVarIntToBuffer(static_cast<std::int32_t>(neededLength));
        std::uint8_t *tempBuffer, *tempBuffer2;
        tempBuffer = static_cast<std::uint8_t *>(malloc(neededLength));
        tempBuffer2 = tempBuffer;
        i2d_PUBKEY(this->keyPair.get(), &tempBuffer2);
        std::copy(tempBuffer, tempBuffer + neededLength, std::back_inserter(this->rawPayload));
        free(tempBuffer);
        this->writeVarIntToBuffer(static_cast<std::int32_t>(this->clearVerifyToken.size()));
        std::copy(reinterpret_cast<std::uint8_t *>(this->clearVerifyToken.data()), reinterpret_cast<std::uint8_t *>(this->clearVerifyToken.data()) + this->clearVerifyToken.size(), std::back_inserter(this->rawPayload));
    }
}