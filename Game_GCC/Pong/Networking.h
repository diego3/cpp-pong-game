#ifndef NETWORKING_H_INCLUDED
#define NETWORKING_H_INCLUDED

#include "../GameCodeStd.h"

/**
  Manage a connection between 2 players.
  The game is using UDP for data exchange.

  I know, there is no "connection" concept in UDP, but it is more easy to understand this way :)
*/
class NetworkManager {

public:
    NetworkManager();

    std::string LastError;
    bool hasError = false;
    std::string lastCmd;//last received game command (protocol message), not used actually

    bool proxy  = false;

    int connectedPort; //port captured from text input
    std::string host;//host captured from text input

    sf::UdpSocket* sock;
    sf::Time lastReceive = sf::Time::Zero;

    sf::IpAddress serverIp;//?????, not used actually

    sf::IpAddress sender;//used by the UDP server to send messages to UDP client
    unsigned short senderPort;

public:
    bool ConnectUDPServer();
    bool ConnectUDPClient();

    bool SendMessage(std::string protocolCmd);
    bool SendPacket(sf::Packet packet);

    std::string Receive(sf::Packet& packet, sf::Time delta);

    void Update(sf::Time delta);

    void SetHost(std::string h){
        if(trim(h).length() == 0){
            host = "127.0.0.1";
        }
        else{
            host = h;
        }
    }
};

#endif // NETWORKING_H_INCLUDED
