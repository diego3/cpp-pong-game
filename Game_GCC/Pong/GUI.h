#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "../GameCodeStd.h"
#include "Networking.h"

class GUI {

public:

    void Init(sf::RenderWindow* render);
    void ProcessEvents(sf::Event evt);
    void Draw(sf::RenderWindow* render, sf::Time DeltaTime);
    void Update(sf::Time DeltaTime);

    //callbacks
    void OnStartButtonClick();
    void OnJoinButtonClick();
    void OnP1P2ButtonClick();
    void Onp1CPUButtonClick();
    void OnTestPacketButtonClick();

    sfg::SFGUI m_sfgui;
    sfg::Desktop desktop;
    sfg::Entry::Ptr portText;
    sfg::Entry::Ptr hostText;
    sfg::Button::Ptr startButton;
    sfg::Button::Ptr joinButton;
    sfg::Button::Ptr p1p2Button;

    bool startBtnClicked = false;
    bool joinBtnClicked = false;

    //true when remote player has connected to the game with success
    bool canStartGame = false;
    bool IsProxy = false;
    bool IsP1P2Game = false;//p1 vs p2 game local
    bool IsP1CPUGame = false;//p1 vs CPU game local
    bool IsNetworkingGame = false;//p1 vs p2 game online

    NetworkManager* netMgr;
};

#endif // GUI_H_INCLUDED
