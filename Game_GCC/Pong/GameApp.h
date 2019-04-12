#ifndef REC_MOVIMENTS_H_INCLUDED
#define REC_MOVIMENTS_H_INCLUDED



#include "Entity.h"

sf::RenderWindow g_window(sf::VideoMode(800, 600), "Pong");

class Paddle;
class Ball;

void RenderFrame(sf::Time DeltaTime);
void UpdateGameFrame(sf::Time DeltaTime);
void ProcessEvents();

bool g_IsProxy = false;

int g_Keys[256];

Paddle p1;
Paddle p2;
Ball ball;

void GameLoopStart(){

    sf::Time framerate = sf::seconds(1.f / 60.f);
    std::cout << "framerate = " << framerate.asMilliseconds() << std::endl;

    sf::Clock clock;
    sf::Time elapsed = sf::Time::Zero;

    //GameLoop
    while(g_window.isOpen()){

        elapsed += clock.restart();
        while(elapsed > framerate){
            ProcessEvents();

            elapsed -= framerate;

            UpdateGameFrame(elapsed);
        }
        RenderFrame(elapsed);
    }
}

void RenderFrame(sf::Time DeltaTime){
    g_window.clear();

    //draw calls
    p1.Draw(DeltaTime);
    //p2->Draw(DeltaTime);
    ball.Draw(DeltaTime);

    g_window.display();
}

void UpdateGameFrame(sf::Time DeltaTime){


}

void ProcessEvents(){
    sf::Event event;
    while(g_window.pollEvent(event)){
        switch(event.type){
            case sf::Event::Closed:
            {
                //delete p1;
                //delete p2;
                //delete ball;
                g_window.close();
                break;
            }
            case sf::Event::KeyPressed:
            {

                break;
            }
            case sf::Event::KeyReleased:
            {

                break;
            }
        }
    }
}

#endif // REC_MOVIMENTS_H_INCLUDED
