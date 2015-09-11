//
//  LoginServer.cpp
//  LoginHandler
//
//  Created by Andrew Querol on 8/26/15.
//  Copyright © 2015 Andrew Querol. All rights reserved.
//

#include "LoginServer.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <chrono>

#include "Client.hpp"
#include "ClientServer.hpp"

Cerios::Server::Login::Login(unsigned short mcPort, unsigned short nodeCommsPort, bool ipv6) : service(new asio::io_service()),
clientAcceptor(std::ref(*service.get()), asio::ip::tcp::endpoint(ipv6 ? asio::ip::tcp::v6() : asio::ip::tcp::v4(), mcPort)),
clientServerAcceptor(std::ref(*service.get()), asio::ip::tcp::endpoint(ipv6 ? asio::ip::tcp::v6() : asio::ip::tcp::v4(), nodeCommsPort)),
keyPair(EVP_PKEY_new(), [=](EVP_PKEY* keyPair) { EVP_PKEY_free(keyPair); }),
certificate(X509_new(), [=](X509* cert) { X509_free(cert); }) {
    
}

void Cerios::Server::Login::listen() {
        this->service->run();

    std::cout<<"wtf";
}

void Cerios::Server::Login::asyncClientAccept() {
    std::shared_ptr<asio::ip::tcp::socket> tmpSocket = std::shared_ptr<asio::ip::tcp::socket>(new asio::ip::tcp::socket(clientAcceptor.get_executor().context()));
    clientAcceptor.async_accept(*tmpSocket, std::bind(&Cerios::Server::Login::handleClient, this, tmpSocket, std::placeholders::_1));
}

void Cerios::Server::Login::init() {
    RSA *rsa = RSA_new();
    BIGNUM *bne = BN_new();
    BN_set_word(bne, 17);
    RSA_generate_key_ex(rsa, 1024, bne, nullptr);
    BN_free(bne);
    EVP_PKEY_assign_RSA(this->keyPair.get(), rsa);
    X509_set_version(this->certificate.get(), 0);
    X509_set_pubkey(this->certificate.get(), this->keyPair.get());
    
    std::size_t neededLength = i2d_PUBKEY(this->keyPair.get(), NULL);
    std::uint8_t *tempBuffer, *tempBuffer2;
    tempBuffer = static_cast<std::uint8_t *>(malloc(neededLength));
    tempBuffer2 = tempBuffer;
    i2d_PUBKEY(this->keyPair.get(), &tempBuffer2);
    this->publicKeyString = std::string(reinterpret_cast<std::string::value_type*>(tempBuffer), neededLength);
    free(tempBuffer);

    this->clientServerHanler = std::shared_ptr<Cerios::Server::ClientServer>(new Cerios::Server::ClientServer(1338, false, std::dynamic_pointer_cast<Cerios::Server::Login>(this->shared_from_this())));
    this->asyncClientAccept();
}

void Cerios::Server::Login::handleClient(std::shared_ptr<asio::ip::tcp::socket> newClient, const asio::error_code &error) {
    if (!error) {
        std::unique_ptr<Cerios::Server::Client> client(new Cerios::Server::Client(newClient, this->shared_from_this()));
        pendingClients[newClient->native_handle()] = std::move(client);
    }
    this->asyncClientAccept();
}

std::shared_ptr<EVP_PKEY> Cerios::Server::Login::getKeyPair() {
    return this->keyPair;
}

std::shared_ptr<X509> Cerios::Server::Login::getCertificate() {
    return this->certificate;
}

std::string Cerios::Server::Login::getPublicKeyString() {
    return this->publicKeyString;
}

void Cerios::Server::Login::clientDisconnected(Cerios::Server::AbstractClient *disconnectedClient) {
    std::cout<<"Client "<<disconnectedClient->getSocket()->remote_endpoint()<<" Disconnected!"<<std::endl;
    pendingClients.erase(disconnectedClient->getSocket()->native_handle());
}

void Cerios::Server::Login::handoffClient(Cerios::Server::AbstractClient *abstractClient) {
    auto client = std::move(this->pendingClients[abstractClient->getSocket()->native_handle()]);
    if (client == nullptr) {
        return;
    }
    
    this->pendingClients.erase(client->getSocket()->native_handle());
    client->setOwner(this->clientServerHanler);
    this->clientServerHanler->addClient(std::move(client));
}

std::weak_ptr<asio::io_service> Cerios::Server::Login::getIOService() {
    return std::weak_ptr<asio::io_service>(this->service);
}

Cerios::Server::Login::~Login() {
//    if (groupSocketDiscription != nullptr) {
//        delete groupSocketDiscription;
//    }
//    if (this->broadcastSocketHandle > 0) {
//        shutdown(this->broadcastSocketHandle, SHUT_RDWR);
//        close(this->broadcastSocketHandle);
//    }
}