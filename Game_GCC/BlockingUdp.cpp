#include "Game_GCC/GameCodeStd.h"

using namespace std;

void HandleError(sf::Socket::Status status){
    cout << "packet received failed: '";
    switch(status){
        case sf::Socket::Status::Disconnected:
            cout<< "disconnected";
            break;
        case sf::Socket::Status::Error:
            cout<< "error";
            break;
        case sf::Socket::Status::NotReady:
            cout<< "not ready";
            break;
        case sf::Socket::Status::Partial:
            cout<< "partial";
            break;
        default:
            cout << "unknown error";
            break;
    }
    cout << "'\n";
}

int main(){
    /* initialize random seed: */
    //srand (time(NULL));

    sf::UdpSocket sock;
    sf::IpAddress serverIp;
    sf::IpAddress sender;//used by the server to send messages to client
    unsigned short senderPort;

    //no blocking in while looping
    //sock.setBlocking(false);

    bool proxy = false;//server ?
    char option;
    cout << "server or client (s/c): ";
    cin >> option;

    proxy = (option == 's' || option == 'S');


    if(proxy){//server
        if(sock.bind(52255) != sf::Socket::Done){
            cout << "socket fail to bind port 52255\n";
            cin.ignore(10000, '\n');
            return 0;
        }
        cout << "waiting player2...\n\n";
    }

    if(!proxy){
        while(serverIp == sf::IpAddress::None){
            cout << "Enter the IP to connect to: ";
            cin >> serverIp;
        }
    }

    bool exit = false;
    char msgCmd;
    while(!exit){
        sf::Packet resp;
        sf::Socket::Status status = sock.receive(resp, sender, senderPort);
        if(status == sf::Socket::Done){
            cout << "received from : " << sender <<":"<< senderPort << "\n";
            string cmd;
            resp >> cmd;
            cout << "execute " << cmd << endl;
        }

        cout << "enter c : ";
        cin >> msgCmd;

        string game_event;
        switch(msgCmd){
            case 'e':{
                exit = false;
                break;
            }
            case 'c':{
                cout << "game command:";
                cin >> game_event;

                if(!proxy){//UDP server
                    sf::Packet pa;
                    pa << game_event;
                    sock.send(pa, serverIp, 52255);
                }
                else{//game server(p1) sending events to p2(UDP client)
                    sf::Packet pa;
                    pa << game_event;
                    cout << "p1 sent to " << sender <<":"<< senderPort <<"\n";
                    sock.send(pa, sender, senderPort);
                }
                break;
            }
        }
    }

    // Wait until the user presses 'enter' key
    cout << "\nPress enter to exit..." << std::endl;
    cin.ignore(10000, '\n');
    return 0;
}




