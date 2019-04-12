#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "../GameCodeStd.h"
#include "Networking.h"

class Entity {

public:
    //Entity();
    //virtual ~Entity();
    int Network_ID = 0;
    int width;
    int height;

    sf::Color color;

    bool IsProxy = false;

    virtual void Update(sf::Time delta) =0;
    virtual void Draw(sf::RenderWindow* render, sf::Time delta) = 0;

    NetworkManager* netMgr;
};

class Ball;//forward declaration

class Paddle : public Entity {
public:
    Paddle();
    virtual ~Paddle();


    sf::RectangleShape rectangle;
    bool moving;
    bool up;
    bool controlledByP1 = false;

    //AI stuffs
    bool AI = false;
    Ball* target;
    float AI_VEL = 0.8f;
    int dir = 1;

    int fixedX = 0;
    float speed;

    virtual bool CheckCollision(Paddle* otherTesting);
    virtual void Update(sf::Time delta);
    virtual void Draw(sf::RenderWindow* render, sf::Time delta);
};

class Ball : public Entity {
public:
    Ball();
    virtual ~Ball();


    sf::CircleShape circle;
    int vX;
    int vY;
    float maxVel;
    bool reseting;

    bool devControl;//developer is moving the ball ?
    bool moving;
    bool left;
    bool up;

    sf::Time counter;
    sf::Clock clock;
    sf::Time elapsed;

    bool collidedP1;
    bool collidedP2;
    bool hitRight;
    bool hitLeft;
    bool hitTop;
    bool hitBottom;

    Paddle* p2;
    sf::RectangleShape bb;//bounding box

    int hitCount = 0;

    virtual void CheckCollision(Paddle* p1, Paddle* p2);
    virtual void Update(sf::Time delta);
    virtual void Draw(sf::RenderWindow* render, sf::Time delta);

    void printPosition(){
        float x = bb.getPosition().x;
        float y = bb.getPosition().y;
        std::cout << "ball bb position  [x: " << x << ", y:" << y << "]\n";
    }
};
#endif // ENTITY_H_INCLUDED
