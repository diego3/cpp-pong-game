#include "Game_GCC/GameCodeStd.h"
#include "Game_GCC/Threads.h"
#include "Game_GCC/Sounds.h"

#include "Game_GCC/Pong/Entity.h"
#include "Game_GCC/Pong/GUI.h"
#include "Game_GCC/Pong/Networking.h"



sf::RenderWindow g_window(sf::VideoMode(SCREEN_W, SCREEN_H), "Pong - UENP - 2016");

class Paddle;
class Ball;

void RenderFrame(sf::RenderWindow* render, sf::Time DeltaTime);
void UpdateGameFrame(sf::Time DeltaTime);
void ProcessEvents();
void GameLoopStart();
void CheckCollisions();


bool g_IsProxy = false;
bool GameRunning = false;
bool ExitGame = false;
bool toasted = false;
bool toastNow = false;
bool isSlow = false;

sf::Clock toastClock;
sf::Time toastTimer = sf::Time::Zero;

sf::Time gameLifeTime = sf::Time::Zero;
sf::Time framerate;
sf::Clock slowMotionClock;

NetworkManager* netMgr = new NetworkManager();
GUI* menu = new GUI();
Paddle* p1 = new Paddle();
Paddle* p2 = new Paddle();
Ball* ball = new Ball();

sf::Text goalP1;
sf::Text goalP2;
sf::Text matchesP1;
sf::Text matchesP2;
sf::Text connectionFeedback;//instructions after click on on-line mode buttons
sf::Text debugText;
sf::Text hitCountText;
sf::RectangleShape middleLine;

sf::Thread toastThread(&PlaySoundEffect, "../Assets/Audio/toask_mk_effect.ogg");
sf::Sprite toastSprite;

int p1Points = 0;
int p2Points = 0;
int p1Matchs = 0;
int p2Matchs = 0;


void UpdateGameFrame(sf::Time DeltaTime){

    if(!GameRunning){
        sf::Packet packet;
        std::string join = netMgr->Receive(packet, DeltaTime);
        if(join == "join"){
            if(netMgr->sender != sf::IpAddress::None){
                menu->canStartGame = true;
            }
        }
    }

    if(!GameRunning && menu->canStartGame){
        if(menu->IsP1P2Game){

        }
        else if(menu->IsP1CPUGame){
            p2->AI = true;
            p2->target = ball;
            ball->p2 = p2;
        }
        else if(menu->IsProxy){
            g_IsProxy = true;
            p1->IsProxy = true;
            ball->IsProxy = true;
            ball->reseting = false;
            //p2->controlledByP1 = true;
        }
        else{
            p2->IsProxy = true;
            //p1->controlledByP1 = true;
            //send ball vX ?
        }

        GameRunning = true;
        menu->startBtnClicked = false;
        //TODO how to hide or dispose the menu?
    }

    if(!GameRunning){
        // Update SFGUI with elapsed seconds since last call.
        menu->Update(DeltaTime);
		return;
    }

    //increase ball acceleration based on player points or game time
    //that takes 1.5min to reach to the max velocity
    if(gameLifeTime > sf::milliseconds(2000)){
        if(ball->vX < ball->maxVel){
            if(ball->vX < 0){
                ball->vX -= 1;
            }else{
                ball->vX += 1;
            }
        }

        //netMgr->SendMessage("addballvel"); // it seems auto sync with the proxy

        if(ball->vX > ball->maxVel){
            ball->vX = ball->maxVel;
        }
        gameLifeTime = sf::Time::Zero;
    }

    //game is fast && ball collided top or bottom && player distance is far way then ball
    //collision with side screen
    if(ball->vX >= ball->maxVel*0.75f && (ball->hitBottom || ball->hitTop)){
        framerate = sf::seconds(1.f/30.f);
        isSlow = true;
        slowMotionClock.restart();
    }
    if(isSlow && slowMotionClock.getElapsedTime().asSeconds() > 2){
        isSlow = false;
        framerate = sf::seconds(1.f/60.f);
    }

    if(toastClock.getElapsedTime().asSeconds() >= 2){
        toastNow = false;
    }
    if(!toasted && ball->hitCount >= 8 ||
       (toasted && ball->hitCount >= 5 && ball->vX >= ball->maxVel * 0.6f)){
        toastNow = true;
        toastClock.restart();
    }
    if(toastNow && !toasted){
        toastThread.launch();
        toasted = true;
    }

    ball->Update(DeltaTime);
    p1->Update(DeltaTime);
    p2->Update(DeltaTime);

    debugText.setString(TOSTRING(ball->vX));
    //hitCountText.setString(TOSTRING(ball->hitCount));

    if(g_IsProxy){
        sf::Packet packet;
        std::string cmd = "";// = netMgr->Receive(packet, DeltaTime);
        sf::IpAddress tempSenderIp;
        unsigned short tempSenderPort;

        while(netMgr->sock->receive(packet, tempSenderIp, tempSenderPort) == sf::Socket::Done){
            packet >> cmd;
            std::cout << "RECEIVED KKK = " << cmd << "\n";
            if(cmd == "addp1"){
                break;
            }
        }

        if(cmd == "addp1"){
            p1Points += 1;
            std::cout << "entrou addp1 recebido\n";
            std::cout << "p1Points = " << p1Points << "\n";
            goalP1.setString(TOSTRING(p1Points));
        }
        else if(cmd == "addp2"){
            p2Points += 1;
            goalP2.setString(TOSTRING(p2Points));
        }
        else if(cmd == "match1"){
            p1Matchs++;
            std::cout << "entrou p1Matchs recebido\n";
            std::cout << "p1Matchs = " << p1Matchs << "\n";
            matchesP1.setString(TOSTRING(p1Matchs));
        }
        else if(cmd == "match2"){
            p2Matchs++;
            matchesP2.setString(TOSTRING(p2Matchs));
        }

        return;
    }

    //player points
    if(ball->hitLeft){
        p2Points += 1;
        if(p2Points >= POINTS_TO_WIN){
            p1Points =0;
            p2Points =0;
            p2Matchs++;
            matchesP2.setString(TOSTRING(p2Matchs));
            netMgr->SendMessage("match2");
        }else{
            netMgr->SendMessage("addp2");
        }
        goalP2.setString(TOSTRING(p2Points));
        ball->hitLeft = false;
        ball->hitCount = 0;
        toastNow = false;
        toasted = false;
    }
    if(ball->hitRight){
        p1Points += 1;
        if(p1Points >= POINTS_TO_WIN){
            p1Points =0;
            p2Points =0;
            p1Matchs++;
            matchesP1.setString(TOSTRING(p1Matchs));
            netMgr->SendMessage("match1");
        }else{
            netMgr->SendMessage("addp1");
        }

        goalP1.setString(TOSTRING(p1Points));
        ball->hitRight = false;
        ball->hitCount = 0;
        toastNow = false;
        toasted = false;
    }
}


void CheckCollisions(){
    ball->CheckCollision(p1, p2);
}

void RenderFrame(sf::RenderWindow* render, sf::Time DeltaTime){
    g_window.clear();

    if(!GameRunning){
        // Update SFGUI with elapsed seconds since last call.
        menu->Draw(render, DeltaTime);

        if(menu->IsNetworkingGame && menu->startBtnClicked){
            connectionFeedback.setString("Aguardando segundo jogador...");
            render->draw(connectionFeedback);
        }
        g_window.display();
		return;
    }

    render->draw(middleLine);
    p1->Draw(render, DeltaTime);
    p2->Draw(render, DeltaTime);
    ball->Draw(render, DeltaTime);

    render->draw(goalP1);
    render->draw(goalP2);
    render->draw(matchesP1);
    render->draw(matchesP2);
    //render->draw(debugText);

    if(isSlow){
        render->draw(hitCountText);
    }
    if(toastNow){
        render->draw(toastSprite);
    }

    g_window.display();
}

int main(){
    /* initialize random seed: */
    srand (time(NULL));

    framerate = sf::seconds(1.f / 60.f);

    //Network_ID should be unique by entity
    ball->Network_ID = 10;
    p1->Network_ID = 20;
    p2->Network_ID = 30;

    p1->rectangle.setPosition(0, 150);
    p2->rectangle.setPosition(SCREEN_W-15, 250);//50
    p2->fixedX = SCREEN_W-15;

    sf::Image icon;
    icon.loadFromFile("../Assets/Art/pong_png.png");
//	if (!diego.loadFromFile("../Assets/Art/SpriteSheet/toasty.png"));
//    {
//        std::cout << "Failed to load ICON\nPress enter to exit..." << std::endl;
//        std::cin.ignore(10000, '\n');
//        return 0;
//    }
	g_window.setIcon(128, 128, icon.getPixelsPtr());


    sf::Font font;
    if (!font.loadFromFile("../Assets/Font/sansation.ttf")){
        std::cout << "Font not found at Assets/Font/sansation.ttf" << std::endl;
        std::cout << "Press enter to exit..." << std::endl;
        std::cin.ignore(10000, '\n');
        return 0;
    }

    sf::Texture toastTexture;
    if(!toastTexture.loadFromFile("../Assets/Art/SpriteSheet/toasty.png")){
        std::cout << "Failed to load toast texture in ../Assets/Art/SpriteSheet/toasty.png" << std::endl;
        std::cout << "Press enter to exit..." << std::endl;
        std::cin.ignore(10000, '\n');
    }
    toastSprite.setTexture(toastTexture);
    toastSprite.setTextureRect(sf::IntRect(0,0, 200, 200));
    toastSprite.setPosition(SCREEN_W-200, SCREEN_H-200);

    connectionFeedback.setFont(font);
    connectionFeedback.setPosition(SCREEN_W/4, LOGINSCREEN_W+20);

    goalP1.setFont(font);
    goalP2.setFont(font);
    goalP1.setPosition(20, 3);
    goalP2.setPosition(SCREEN_W-32, 3);
    goalP1.setString("0");
    goalP2.setString("0");

    matchesP1.setFont(font);
    matchesP1.setPosition((SCREEN_W/2)-45, 3);
    matchesP1.setString("0");
    matchesP2.setFont(font);
    matchesP2.setPosition((SCREEN_W/2)+25, 3);
    matchesP2.setString("0");

    debugText.setFont(font);
    debugText.setFillColor(sf::Color::Red);
    debugText.setPosition(SCREEN_W/2, 3);
    debugText.setString("-5");

    hitCountText.setFont(font);
    hitCountText.setFillColor(sf::Color::Green);
    hitCountText.setPosition(SCREEN_W/2, SCREEN_H/2);
    hitCountText.setString("Slow");

    middleLine.setFillColor(sf::Color::White);
    middleLine.setSize(sf::Vector2f(10, SCREEN_W));
    middleLine.setPosition((SCREEN_W/2) - (middleLine.getSize().x/2), 0);//395

    ball->vX = BALL_INITIAL_VEL;//ball->maxVel;//rand() % 3 + 1;// generate random directions
    ball->vY = 7;//rand() % 3 + 1;
    ball->circle.setPosition(SCREEN_W/2, 200);

    //NetworkManager for all
    p1->netMgr = netMgr;
    p2->netMgr = netMgr;
    ball->netMgr = netMgr;
    menu->netMgr = netMgr;
    menu->Init(&g_window);

    // create a thread with func() as entry point
    sf::Thread backgroundMusicThread(&PlayMusicLooping, "../Assets/Audio/Music/OriginalTetrisTheme.ogg");
    backgroundMusicThread.launch();

    g_window.setFramerateLimit(60);
    GameLoopStart();

    delete menu;
    backgroundMusicThread.terminate();

    //delete p1;
    //delete p2;
    //delete ball;

    g_window.close();
    // Wait until the user presses 'enter' key
    std::cout << "\nPress enter to exit..." << std::endl;
    std::cin.ignore(10000, '\n');
    return 0;
}



void GameLoopStart(){

    sf::Clock clock;
    sf::Clock gameClock;
    sf::Time elapsed = sf::Time::Zero;
    while(g_window.isOpen()){

        if(ExitGame){
            break;
        }
        elapsed += clock.restart();

        if(GameRunning){
            gameLifeTime += gameClock.restart();
            //std::cout << "gameLifeTime " << gameLifeTime.asMilliseconds() << std::endl;
        }

        while(elapsed > framerate){
            ProcessEvents();

            elapsed -= framerate;

            CheckCollisions();

            UpdateGameFrame(framerate);

        }
        RenderFrame(&g_window, elapsed);
    }
}

void ProcessEvents(){
    sf::Event event;
    while(g_window.pollEvent(event)){

        if(!GameRunning){
            menu->ProcessEvents(event);
        }

        switch(event.type){
            case sf::Event::Closed:
            {
                ExitGame = true;
                break;
            }
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::Escape){
                    ExitGame = true;
                    break;
                }

                if(event.key.code == sf::Keyboard::F1){
                    framerate = sf::seconds(1.f/30.f);//slow motion :)
                }
                if(event.key.code == sf::Keyboard::F2){
                    framerate = sf::seconds(1.f/60.f);
                }

                if(g_IsProxy && event.key.code == sf::Keyboard::P){
                    ball->printPosition();
                }

                if(event.key.code == sf::Keyboard::F){
                    std::cout << "\ng_IsProxy " <<  g_IsProxy <<"\n";
                }

                if(menu->IsNetworkingGame){
                    if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up){
                        if(!g_IsProxy){
                            p1->moving = true;
                            p1->up = true;
                        }else{
                            p2->moving = true;
                            p2->up = true;
                        }
                    }
                    if(event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down){
                        if(!g_IsProxy){
                            p1->moving = true;
                            p1->up = false;
                        }else{
                            p2->moving = true;
                            p2->up = false;
                        }
                    }
                }
                else{
                    if(event.key.code == sf::Keyboard::W){
                        p1->moving = true;
                        p1->up = true;
                    }
                    if(event.key.code == sf::Keyboard::S){
                        p1->moving = true;
                        p1->up = false;
                    }
                    if(event.key.code == sf::Keyboard::Up){
                        p2->moving = true;
                        p2->up = true;
                    }
                    if(event.key.code == sf::Keyboard::Down){
                        p2->moving = true;
                        p2->up = false;
                    }
                }
                break;
            }
            case sf::Event::KeyReleased:
            {
                if(menu->IsNetworkingGame){
                    if(event.key.code == sf::Keyboard::W ||
                       event.key.code == sf::Keyboard::S ||
                       event.key.code == sf::Keyboard::Up ||
                       event.key.code == sf::Keyboard::Down){
                        if(!g_IsProxy){
                            p1->moving = false;
                        }else{
                            p2->moving = false;
                        }
                    }
                }
                else{
                    if(event.key.code == sf::Keyboard::W ||
                       event.key.code == sf::Keyboard::S){
                        p1->moving = false;
                    }
                    if(event.key.code == sf::Keyboard::Up ||
                       event.key.code == sf::Keyboard::Down){
                        p2->moving = false;
                    }
                }

//                if(event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down ||
//                   event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right){
//                    ball->moving = false;
//                }
                break;
            }
        }
    }
}


/**
   This is a simple SFML project to test Graphics, Audio, Networking, Game pad
   and others things that are specific to this framework, so, you wont
   see many OOP rules applied very well

#include "Game_GCC/GameCodeStd.h"
//#include "Game_GCC/Tests/SFMLStuffs/AssetManager.h"
//#include "Game_GCC/Tests/SFMLStuffs/ShaderExample.h"
//#include "Game_GCC/Tests/SFMLGameLoop.h"
#include "Game_GCC/Threads.h"
#include "Game_GCC/Sounds.h"

#include "Game_GCC/Pong/Entity.h"
#include "Game_GCC/Pong/GameApp.h"

sf::RenderWindow g_window(sf::VideoMode(800, 600), "Pong");

int main(){
    //ShaderExample();
    //GameLoop();
    //ThreadsInit();

    //SoundsInit("Assets/Audio/Music/TheWalkingDeadFullThemeSong.wav");
    //MusicInit("canary.wav");


    GameLoopStart();

    // Wait until the user presses 'enter' key
    std::cout << "\nPress enter to exit..." << std::endl;
    std::cin.ignore(10000, '\n');
    return 0;
}*/

