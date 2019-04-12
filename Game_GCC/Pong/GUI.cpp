

#include "GUI.h"
#include "Networking.h"

void GUI::Init(sf::RenderWindow* render){

	// Create a simple button and connect the click signal.
	startButton = sfg::Button::Create( "Connect Game as Host" );
	startButton->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &GUI::OnStartButtonClick, this ) );

	joinButton = sfg::Button::Create( "Join Game as Client" );
	joinButton->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &GUI::OnJoinButtonClick, this ) );

	p1p2Button = sfg::Button::Create( "P1 vs P2 Local Game" );
	p1p2Button->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &GUI::OnP1P2ButtonClick, this ) );

    auto p1CPUButton = sfg::Button::Create( "P1 vs CPU Local Game" );
	p1CPUButton->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &GUI::Onp1CPUButtonClick, this ) );

    auto testPacketButton = sfg::Button::Create( "Test Packet" );
    testPacketButton->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &GUI::OnTestPacketButtonClick, this ) );

	// Create a vertical box layouter with 5 pixels spacing and add the component to it
	//sfg::Label::Ptr hostLabel = sfg::Label::Create( "Host:" );
	auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.0f );
	hostText = sfg::Entry::Create("127.0.0.1");
	portText = sfg::Entry::Create("6060");

	//box->Pack(hostLabel);
	box->Pack(hostText, false);
	box->Pack(portText, false);
	box->Pack(p1CPUButton, false);
	box->Pack(p1p2Button, false );
	box->Pack(startButton, false);
	box->Pack(joinButton, false );
	box->Pack(testPacketButton, false );



	// Create a window and add the box layouter to it. Also set the window's title.
	auto window = sfg::Window::Create();
	window->SetTitle( "Configurações" );
	window->Add( box );

	sf::FloatRect wPos((SCREEN_W/2)-(LOGINSCREEN_W/2), (SCREEN_H/2)-LOGINSCREEN_H, LOGINSCREEN_W, LOGINSCREEN_H);
	window->SetAllocation(wPos);

	desktop.Add( window );

	// We're not using SFML to render anything in this program, so reset OpenGL
	// states. Otherwise we wouldn't see anything.
	render->resetGLStates();
};

void GUI::Onp1CPUButtonClick(){
    IsP1CPUGame = true;
    canStartGame = true;
};

void GUI::OnTestPacketButtonClick(){
    sf::Packet ballPos;
    std::string msg = "bm";
    int x = -8;
    int y = 5;
    ballPos << msg << x << y;
    netMgr->SendPacket(ballPos);
};

void GUI::OnStartButtonClick(){
    startBtnClicked = true;
    IsNetworkingGame = true;

    if(netMgr->sender != sf::IpAddress::None){
        canStartGame = true;
    }

    //connect when we are a local player
    //so, our game logic will act as a server, sending game events over the network
    //to the remote player
    joinButton->SetState(sfg::Widget::State::INSENSITIVE);

    std::string portStr = portText->GetText();
    netMgr->connectedPort = STRING2INT(portStr);

    std::string hostStr = hostText->GetText();
    netMgr->SetHost(hostStr);


    bool connected = netMgr->ConnectUDPServer();
    if(!connected){
        std::cout << "client socket connection failed\n";
        std::cout << "neterr: " << netMgr->LastError << std::endl;
    }
    else{
        std::cout << "CONNECTED : LOCAL : " << netMgr->host
        << ":" << netMgr->connectedPort << "\n";
        std::cout << "waiting for a second player...\n";
    }
};

void GUI::OnJoinButtonClick(){
    joinBtnClicked = true;
    IsNetworkingGame = true;
    std::cout << "join button clicked" << std::endl;

    startButton->SetState(sfg::Widget::State::INSENSITIVE);
    p1p2Button->SetState(sfg::Widget::State::INSENSITIVE);

    std::string portStr = portText->GetText();
    netMgr->connectedPort = STRING2INT(portStr);

    std::string hostStr = hostText->GetText();
    netMgr->SetHost(hostStr);

    bool connected = netMgr->ConnectUDPClient();
    if(!connected){
        std::cout << "join game connection failed\n";
        std::cout << "neterr: " << netMgr->LastError << std::endl;
    }
    else{
        std::cout << "CONNECTED : REMOTE : " << netMgr->sender
        << ":" <<  netMgr->senderPort <<"\n";
        IsProxy = true;//UDP Client connected
        canStartGame = true;
    }
};

void GUI::OnP1P2ButtonClick(){
    IsP1P2Game = true;
    canStartGame = true;
};

void GUI::Draw(sf::RenderWindow* render, sf::Time delta){
    m_sfgui.Display( *render );
};

void GUI::ProcessEvents(sf::Event evt){
    desktop.HandleEvent(evt);
};


void GUI::Update(sf::Time delta){
    desktop.Update( delta.asSeconds());
};
