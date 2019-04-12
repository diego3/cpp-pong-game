
#include "GameCodeStd.h"


int main(){
    sf::RenderWindow window(sf::VideoMode(800, 600), "SpriteSheets");
    window.setFramerateLimit(60);

    const char* DRAGON_FRAMES = "../Assets/Art/SpriteSheet/dragon_frames.png";//319 424
    const char* EXPLOSION_A = "../Assets/Art/SpriteSheet/type_A.png";//1024 50
    const char* EXPLOSION_B = "../Assets/Art/SpriteSheet/type_B.png";//12288 192
    const char* BLOOD_HIT3 = "../Assets/Art/SpriteSheet/blood_hit_03.png";//512 512
    const char* FIRE_SUBUV = "../Assets/Art/SpriteSheet/T_FireSubUV.jpg";//432 432
    const char* THE_EXPLOSION = "../Assets/Art/SpriteSheet/TheExplosion.png";//213 150

    //dragon  100
    //ex_a    50
    //ex_b    100
    //blood_hit3 128 128
    //fire_subuv
    sf::Texture texture;
    texture.loadFromFile(BLOOD_HIT3);
    int frameIndex = 1;
    int maxFrames = 14;
    float speed = 80.f;
    int perRow = 4;
    int perCol = 4;
    int width = 512;
    int height = 512;
    int width2 = width / perRow;
    int height2 = height / perCol;
    bool reversed = false;

    sf::IntRect rect(0,0, width2, height2);
    sf::Sprite sprite(texture, rect);

    sf::Clock clock;

    //GameLoop
    while(window.isOpen()){

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }


        if(clock.getElapsedTime().asMilliseconds() > speed){
            if(!reversed){
                frameIndex++;
            }else{
                frameIndex--;
            }

            if(frameIndex == maxFrames){
                frameIndex = 1;
                rect.left = 0;
                rect.top = 0;
                //reversed = true;
            }
            else if(frameIndex <=0 ){
                //frameIndex = 1;
                //rect.left = 0;
                //rect.top = 0;
                reversed = false;
            }


            if(frameIndex % perRow == 0){
                if(!reversed){
                    rect.top += height2;
                    rect.left = 0;
                }
                else{
                    rect.top -= height2;
                    rect.left = width2 * perRow;
                }
            }else{
                if(!reversed){
                    rect.left += width2;
                }
                else{
                    rect.left -= width2;
                }
            }
            sprite.setTextureRect(rect);
            clock.restart();
        }

//        if(clock.getElapsedTime().asMilliseconds() > 50.f){
//            if(rect.left >= 1024){
//                rect.left = 0;
//            }
//            else{
//                rect.left += 50;
//            }
//            sprite.setTextureRect(rect);
//            clock.restart();
//        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
