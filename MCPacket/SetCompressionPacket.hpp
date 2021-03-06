//
//  SetCompressionPacket.hpp
//  MCPacket
//
//  Created by Andrew Querol on 9/2/15.
//  Copyright © 2015 Andrew Querol. All rights reserved.
//

#ifndef SetCompressionPacket_hpp
#define SetCompressionPacket_hpp

#include "Packet.hpp"

#pragma GCC visibility push(default)
namespace Cerios { namespace Server {
    class SetCompressionPacket : public Packet {
    public:
        std::int32_t compressionThreshold;
    public:
        void serializePacket(Cerios::Server::Side sideSending);
        
        static std::shared_ptr<Packet> parsePacket(Cerios::Server::Side side, std::shared_ptr<Packet> packetInProgress) { return std::static_pointer_cast<Packet>(std::shared_ptr<SetCompressionPacket>(new SetCompressionPacket(packetInProgress))); }
        static std::shared_ptr<Packet> newPacket(Cerios::Server::Side side) { return std::static_pointer_cast<Packet>(std::shared_ptr<SetCompressionPacket>(new SetCompressionPacket())); }
    protected:
        SetCompressionPacket(std::shared_ptr<Cerios::Server::Packet> packetInProgress);
        SetCompressionPacket();
    };
}}
#pragma GCC visibility pop

#endif /* SetCompressionPacket_hpp */
