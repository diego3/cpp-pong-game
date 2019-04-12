
#include "Entity.h"


Paddle::Paddle(){
    width = 15;
    height = 150;
    color = sf::Color::White;
    rectangle = sf::RectangleShape(sf::Vector2f(width, height));
    moving = false;
    up = false;
    speed = 1.5f;
}

Paddle::~Paddle(){

}

bool Paddle::CheckCollision(Paddle* oth){
    bool col = oth->rectangle.getGlobalBounds().contains(rectangle.getPosition());
    std::cout << "collide status " << col << std::endl;
};

void Paddle::Update(sf::Time delta){
    if(AI){
        float oldY = rectangle.getPosition().y;
        oldY += (dir * AI_VEL * delta.asMilliseconds());

        if(oldY+height >= SCREEN_H){
            dir = -1;
        }
        else if(oldY < 0){
            dir = 1;
        }
        rectangle.setPosition(fixedX, oldY);
        return;
    }

    if(IsProxy){
        sf::Packet packet;
        std::string cmd = netMgr->Receive(packet, delta);
        int id = 0;
        int serverX = 0;
        float serverY = 0;
        packet >> id >> serverX >> serverY;

        float oldX = rectangle.getPosition().x;
        float oldY = rectangle.getPosition().y;

        if(cmd == "pmu" && id == Network_ID){

            //float newY = serverY -(speed * delta.asMilliseconds());

            rectangle.setPosition(fixedX, serverY);
        }
        if(cmd == "pmd" && id == Network_ID){

            //float newY = serverY + (speed * delta.asMilliseconds());

            rectangle.setPosition(fixedX, serverY);
        }
        return;
    }

    if(moving){
        float myX = rectangle.getPosition().x;
        float myY = rectangle.getPosition().y;
        if(up){
            if(rectangle.getPosition().y <= 0){
                return;
            }
            //send my position over network to player 2

            float oldY = rectangle.getPosition().y;
            float newY = oldY -(speed * delta.asMilliseconds());

            sf::Packet movePacket;
            std::string cmd = "pmu";
            movePacket << cmd << Network_ID << myX << newY;
            netMgr->SendPacket(movePacket);
            //rectangle.move(0, -speed);

            rectangle.setPosition(fixedX, newY);
        }
        else{
            if(rectangle.getPosition().y+height >= SCREEN_H){
                return;
            }
            //send my position over network to player 2

            float oldY = rectangle.getPosition().y;
            float newY = oldY + (speed * delta.asMilliseconds());

            sf::Packet movePacket;
            std::string cmd = "pmd";
            movePacket << cmd << Network_ID << myX << newY;
            netMgr->SendPacket(movePacket);
            //rectangle.move(0, speed);

            rectangle.setPosition(fixedX, newY);
        }
    }
    else{
        //rectangle.move(0, 0);
    }
}

void Paddle::Draw(sf::RenderWindow* render, sf::Time delta){
    rectangle.setFillColor(color);
    render->draw(rectangle);
}


Ball::Ball(){
    width = 10;
    height = 50;
    color = sf::Color::White;
    circle = sf::CircleShape(10);
    vX = 1;//velocity for x direction
    vY = 1;//velocity for y direction
    maxVel = BALL_MAX_VEL;

    counter = sf::milliseconds(2000);//2 seconds to reset
    reseting = true;
    elapsed = sf::Time::Zero;

    collidedP1 = false;
    collidedP2 = false;
    hitRight = false;
    hitLeft = false;
    hitTop = false;
    hitBottom = false;
    bb = sf::RectangleShape(sf::Vector2f(20,20));

    devControl=false;
    moving = false;
    up=false;
    left=false;
}

Ball::~Ball(){

}

void Ball::CheckCollision(Paddle* p1, Paddle* p2){
    if(IsProxy){
        return;
    }
    collidedP1 = false;
    collidedP2 = false;

    bool c1 = p1->rectangle.getGlobalBounds().intersects(bb.getGlobalBounds());
    bool c2 = p2->rectangle.getGlobalBounds().intersects(bb.getGlobalBounds());

    collidedP1 = c1;
    collidedP2 = c2;
}

void Ball::Update(sf::Time delta){
    if(IsProxy){
        sf::Packet packet;
        std::string cmd = netMgr->Receive(packet, delta);

        if(cmd == "addp1"){
            std::cout << "BOSTA recebeu addp1\n";
        }
        if(cmd == "p1p" || cmd == "p2p"){
            reseting = true;
            circle.setPosition(SCREEN_W/2, SCREEN_H/2);
        }

        if(cmd == "reset_negation"){
            reseting = false;
            int x = 0;
            int y = 0;
            packet >> x >> y;
            vX = x;
            vY = y;
        }
        if(reseting){
            return;
        }

        if(cmd == "ball_boff"){
            int y = 0;
            packet >> y;
            vY = y;
        }
        if(cmd == "ball_toff"){
            int y = 0;
            packet >> y;
            vY = y;
        }
        if(cmd == "ball_col1" || cmd == "ball_col2"){
            int x = 0;
            packet >> x;
            vX = x;
        }

        circle.move(vX, vY);
        return;
    }

    if(reseting){
        elapsed += clock.restart();
        if(elapsed >= counter){
            reseting = false;
            vX = BALL_INITIAL_VEL * -1;
            vY = vY * -1;//rand() % 10 + 7;
            elapsed = sf::Time::Zero;

            //p2->AI_VEL = rand() % 3 + 1;

            sf::Packet resetPack;
            std::string msg = "reset_negation";
            int x = vX;
            int y = vY;
            resetPack << msg << x << y;
            netMgr->SendPacket(resetPack);
        }
        return;
    }

    //collision was here
    if(collidedP1){
        vX = -vX;
        hitCount++;
        sf::Packet collidePackage;
        std::string command = "ball_col1";
        int x = vX;
        collidePackage << command << x;
        netMgr->SendPacket(collidePackage);
    }
    else if(collidedP2){
        vX = -vX;//vX * -1;
        hitCount++;
        sf::Packet collidePackage;
        std::string command = "ball_col2";
        int x = vX;
        collidePackage << command << x;
        netMgr->SendPacket(collidePackage);
    }

    hitRight  = false;
    hitLeft   = false;
    hitTop    = false;
    hitBottom = false;
    float left = circle.getPosition().x;
    float top  = circle.getPosition().y;

    if(!collidedP1 && !collidedP2){
        if(left >= SCREEN_W + 20){//right off screen
            //+1 point to p2
            reseting = true;
            elapsed = sf::Time::Zero;
            hitRight = true;
            hitLeft = false;

            circle.setPosition(SCREEN_W/2, SCREEN_H/2);

            netMgr->SendMessage("p2p");
        }
        if(left <= 0){//left off screen
            //+1 point to p1
            reseting = true;
            elapsed = sf::Time::Zero;
            hitLeft = true;
            hitRight = false;
            circle.setPosition(SCREEN_W/2,SCREEN_H/2);

            netMgr->SendMessage("p1p");
        }
    }

    if(top >= SCREEN_H){//bottom off screen
        hitBottom = true;
        vY = -vY;
        sf::Packet collidePackage;
        std::string command = "ball_boff";
        int y = vY;
        collidePackage << command << y;
        netMgr->SendPacket(collidePackage);
    }
    if(top <= 0){//top off screen
        hitTop = true;
        vY = vY * -1;
        sf::Packet collidePackage;
        std::string command = "ball_toff";
        int y = vY;
        collidePackage << command << y;
        netMgr->SendPacket(collidePackage);
    }

    circle.move(vX, vY);
}

void Ball::Draw(sf::RenderWindow* render, sf::Time delta){
    //bounding box visualize helper. Should be remove in the final version
    //bb.setFillColor(sf::Color::Red);
    bb.setPosition(circle.getPosition());
    //render->draw(bb);

    circle.setFillColor(color);
    render->draw(circle);
}
