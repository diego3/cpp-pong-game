#ifndef CIRCLE_RENDER_H_INCLUDED
#define CIRCLE_RENDER_H_INCLUDED

#include <SFML/Graphics.hpp>

void RenderCircle(){
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Works");
    window.setFramerateLimit(60);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    //GameLoop
    while(window.isOpen()){

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

}

#endif // CIRCLE_RENDER_H_INCLUDED
