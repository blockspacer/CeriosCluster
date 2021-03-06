//
//  ServerStatusRequestPacket.hpp
//  LoginHandler
//
//  Created by Andrew Querol on 8/29/15.
//  Copyright © 2015 Andrew Querol. All rights reserved.
//

#ifndef ServerStatusRequestPacket_hpp
#define ServerStatusRequestPacket_hpp

#include "Packet.hpp"

#pragma GCC visibility push(default)
namespace Cerios { namespace Server {
    class ServerStatusPacket : public Packet {
    public:
        std::string jsonEncodedServerStatus;
    public:
        void serializePacket(Cerios::Server::Side sideSending);
        
        static std::shared_ptr<Packet> parsePacket(Cerios::Server::Side side, std::shared_ptr<Packet> packetInProgress) { return std::static_pointer_cast<Packet>(std::shared_ptr<ServerStatusPacket>(new ServerStatusPacket(packetInProgress))); }
        static std::shared_ptr<Packet> newPacket(Cerios::Server::Side side) { return std::static_pointer_cast<Packet>(std::shared_ptr<ServerStatusPacket>(new ServerStatusPacket())); }
    protected:
        ServerStatusPacket(std::shared_ptr<Cerios::Server::Packet> packetInProgress);
        ServerStatusPacket();
    };
}}
#pragma GCC visibility pop

#endif /* ServerStatusRequestPacket_hpp */
