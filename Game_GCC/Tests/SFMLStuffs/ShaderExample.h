#ifndef SHADEREXAMPLE_H_INCLUDED
#define SHADEREXAMPLE_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "AssetManager.h"

int ShaderExample()
{
	sf::RenderWindow window(sf::VideoMode(640, 480), "RenderTexture");
	AssetManager m;

	if (!sf::Shader::isAvailable())
		return -1; //Shaders are not supported. Abort!

	auto shader = AssetManager::GetShader("Assets/Shader/rippleVertShader.vert", "Assets/Shader/rippleShader.frag");

	sf::Sprite sprite(AssetManager::GetTexture("Assets/Art/AAA-metal-diamond-plate-650x520.jpg"));

    shader->setParameter("uTexture", *sprite.getTexture());
	shader->setParameter("uPositionFreq", 0.1f);
	shader->setParameter("uSpeed", 10);
	shader->setParameter("uStrength", 0.03f);

    sf::Clock clock;
	while (window.isOpen())
	{
	    sf::Event event;
        while(window.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:{
                    window.close();
                    break;
                }
            }
        }

		window.clear();

        shader->setParameter("uTime", clock.getElapsedTime().asSeconds());
		window.draw(sprite, shader);

		window.display();
	}

	return 0;
}


#endif // SHADEREXAMPLE_H_INCLUDED
