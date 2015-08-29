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

#include <memory>
#include <string>

namespace Cerios { namespace Server {
    class ServerStatusPacket : public Packet {
    public:
        std::string jsonEncodedServerStatus;
    public:
        void onReceivedBy(Cerios::Server::Client *client);
        void sendTo(Cerios::Server::Client *client);
        void serializePacket();
        
        static std::shared_ptr<Packet> parsePacket(std::shared_ptr<Packet> packetInProgress) { return std::static_pointer_cast<Packet>(std::shared_ptr<ServerStatusPacket>(new ServerStatusPacket(packetInProgress))); }
        static std::shared_ptr<Packet> newPacket() { return std::static_pointer_cast<Packet>(std::shared_ptr<ServerStatusPacket>(new ServerStatusPacket())); }
    protected:
        ServerStatusPacket(std::shared_ptr<Cerios::Server::Packet> packetInProgress);
        ServerStatusPacket();
    };
}}

#endif /* ServerStatusRequestPacket_hpp */
