#ifndef REC_MOVIMENTS_H_INCLUDED
#define REC_MOVIMENTS_H_INCLUDED

sf::RenderWindow window(sf::VideoMode(800, 600), "GameLoop");

void RenderFrame();
void UpdateGameFrame(sf::Time DeltaTime);
void ProcessEvents();

void GameLoop(){

    sf::Time framerate = sf::seconds(1.f / 60.f);
    std::cout << "framerate = " << framerate.asMilliseconds() << std::endl;

    sf::Clock clock;
    sf::Time elapsed = sf::Time::Zero;

    //GameLoop
    while(window.isOpen()){

        elapsed += clock.restart();
        while(elapsed > framerate){
            ProcessEvents();

            elapsed -= framerate;

            UpdateGameFrame(framerate);
        }
        RenderFrame();
    }
}

void RenderFrame(){
    window.clear();

    //draw calls
    //for each view : viewList->VRender();

    window.display();
    //std::cout << "render frame " << std::endl;
}

void UpdateGameFrame(sf::Time DeltaTime){
    //std::cout << "update frame " << std::endl;

    //GameApplication->GameLogic->Update(DeltaTime);

}

void ProcessEvents(){
    sf::Event event;
    while(window.pollEvent(event)){
        if(event.type == sf::Event::Closed){
            window.close();
        }
    }
}

#endif // REC_MOVIMENTS_H_INCLUDED
