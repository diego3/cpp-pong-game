
#include "Networking.h"

// Really good read
//https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking

/**
Pong Protocol Specification

	code        description
	join:		Start game requested
	joiny:      Start game response success
	joinn:      Start game response fail!, Maybe there are no server listening
	end:		End game, someone win!
	pmu ID x y: paddle move up
	pmd ID x y: paddle move down
	bm x y: 	ball move by x position and y position
	ballreset   Reset ball position after someone take a point
	p1p: 		player 1 add +1 point
	p2p: 		player 2 add +1 point
	match1:		+1 match point to player 1
	match2:		+1 match point to player 2
	p1a:		player 1 want to play again
	p2a:		player 2 want to play again
	giveup:		Loser player gives up
*/

NetworkManager::NetworkManager(){
    sock = new sf::UdpSocket();
    LastError = "";
};

bool NetworkManager::ConnectUDPServer(){
    sock->setBlocking(false);//do not block the game loop :)

    //bind only the listening socket
    if(sock->bind(connectedPort, host) != sf::Socket::Done){
        hasError = false;
        LastError = "Failed to bind a client socket on a port " + connectedPort;
        return false;
    }

    proxy = true;
    std::cout << "CONNECTED ON PORT " << sock->getLocalPort() << std::endl;
    return true;
};

bool NetworkManager::ConnectUDPClient(){
    sock->setBlocking(false);

    //bind only the listening socket
    sf::Packet packet;
    std::string protocolCmd = "join";
    packet << protocolCmd;

    SendMessage(protocolCmd);

    return true;
};


bool NetworkManager::SendMessage(std::string protocolCmd){
    if(protocolCmd.size() == 0){
        LastError = "protocolCmd argument can not be empty";
        return false;
    }

    //UDP client
    if(!proxy){//player 2 sending to player 1
        sf::Packet packet;
        packet << protocolCmd;

        sf::IpAddress recipient = host;
        if (sock->send(packet, recipient, connectedPort) != sf::Socket::Done){
            hasError = true;
            LastError = "Failed to send message on a port " + TOSTRING(connectedPort) + " cmd: " + protocolCmd;
            return false;
        }
    }
    else{//player1 (UDP server)
        sf::Packet packet;
        packet << protocolCmd;
        //std::cout << "UDP server sent '" << protocolCmd << "' to " << sender <<":"<< senderPort <<"\n";
        if (sock->send(packet, sender, senderPort) != sf::Socket::Done)
        {
            hasError = true;
            LastError = "Failed to send message " + TOSTRING(protocolCmd) + " to " + sender.toString() + ":"+ TOSTRING(senderPort);
            return false;
        }
    }
    return true;
};

bool NetworkManager::SendPacket(sf::Packet packet){
    //UDP client
    if(!proxy){//player 2 sending to player 1
        sf::IpAddress recipient = host;
        if (sock->send(packet, recipient, connectedPort) != sf::Socket::Done){
            hasError = true;
            LastError = "Failed to send message on a port " + TOSTRING(connectedPort);
            return false;
        }
    }
    else{//player1 (UDP server)
        //std::cout << "UDP server sent a packet to " << sender <<":"<< senderPort <<"\n";
        if (sock->send(packet, sender, senderPort) != sf::Socket::Done)
        {
            hasError = true;
            LastError = "Failed to send a packet to " + sender.toString() + ":"+ TOSTRING(senderPort);
            return false;
        }
    }
    return true;
};

std::string NetworkManager::Receive(sf::Packet& packet, sf::Time delta){
    std::string protocolCmd = "";

    //sf::Packet packet;
    sf::IpAddress tempSenderIp;
    unsigned short tempSenderPort;
    if (sock->receive(packet, tempSenderIp, tempSenderPort) != sf::Socket::Done){
        //std::cout << "Failed to receive data\n";
        return protocolCmd;
    }
    //when the UDP server(p1) receive a message from a UDP client
    //he use its IP and port to communicate with p2
    sender     = tempSenderIp;
    senderPort = tempSenderPort;

    lastReceive = delta;
    packet >> protocolCmd;
    std::cout << "Received cmd " << protocolCmd << " from " << sender << ":" << senderPort << std::endl;
    return protocolCmd;
};

/*
bool NetworkManager::ReceivePacket(sf::Packet& packet, sf::Time delta){
    //sf::Packet packet;
    sf::IpAddress tempSenderIp;
    unsigned short tempSenderPort;
    if (sock->receive(packet, tempSenderIp, tempSenderPort) != sf::Socket::Done){
        LastError = "Failed to receive packet\n";
        return false;
    }

    sender     = tempSenderIp;
    senderPort = tempSenderPort;

    lastReceive = delta;
    return true;
};*/

void NetworkManager::Update(sf::Time delta){
    //future work: maybe we need to check a disconnection
    sf::Packet packet;
    lastCmd = Receive(packet, delta);
    if(lastCmd.length() > 0){
        //std::cout << "received from : " << sender <<":"<< senderPort << "\n";
        //parse the command and send its respective response to the other side
        //std::cout << "lastCmd: " << lastCmd << std::endl;
        lastCmd = "";
    }
};
