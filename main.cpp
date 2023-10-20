#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// Distance = Pixels
// Velocity = Pixels/Frame
// Acceleration = Pixels/Frame^2

constexpr int GRAVITY = 9.81;


class Physics {

public:

    sf::Vector2f calcNextPos(sf::Vector2f& pos, sf::Vector2f& vel, sf::Vector2f& accel, float dt)
    {
        sf::Vector2f NextPosition;
        NextPosition.x = pos.x + (vel.x * dt) + (0.5 * accel.x * (dt * dt));
        NextPosition.y = pos.y + (vel.y * dt) + (0.5 * accel.y * (dt * dt));

        // debugNextPos(pos, vel, accel, dt, calcNextPos);
        return NextPosition;
    }

    sf::Vector2f calcNextVel(sf::Vector2f& vel, sf::Vector2f& accel, float dt)
    {
        sf::Vector2f NextVelocity;
        NextVelocity.x = vel.x + (accel.x * dt);
        NextVelocity.y = vel.y + (accel.y * dt);
        return NextVelocity;
    }

    sf::Vector2f calcMomentum(float& mass, sf::Vector2f& vel) {
        sf::Vector2f Momentum;
        Momentum.x = mass * vel.x;
        Momentum.y = mass * vel.y;
        return Momentum;
    }

    sf::Vector2f VfFromMomentum(float& m1, float& m2, sf::Vector2f& iVel1, sf::Vector2f& fVel1, sf::Vector2f& iVel2)
    {
        // Conservation of Momentum Formula:
        //  M1V1i + M2V2i = M1V1f + M2V2f
        // (M1V1i + M2V2i - M1V1f)/M2 = V2f

        sf::Vector2f vel2;
        vel2.x = ((m1 * iVel1.x) + (m2 * iVel2.x) - (m1 * fVel1.x) / m2);
        vel2.y = ((m1 * iVel1.y) + (m2 * iVel2.y) - (m1 * fVel1.y) / m2);
        return vel2;
    }

    // Only function which actually changes velocity rather then return a number
    bool isWindowCollision(sf::Vector2f& pos, sf::Vector2f& vel, sf::Vector2f& windowSize) {
        if (pos.x < 5) {
            vel.x = (abs(vel.x));
            return true;
        }

        else if (pos.x > windowSize.x - 5) {
            vel.x = -(abs(vel.x));
            return true;
        }

        else if (pos.y < 5) {
            vel.y = (abs(vel.y));
            return true;
        }

        else if (pos.y > windowSize.y - 5) {
            vel.y = -(abs(vel.y));
            return true;
        }

        return false;
    }

private:

    void debugNextPos(sf::Vector2f& pos, sf::Vector2f& vel, sf::Vector2f& accel, float dt, sf::Vector2f& calcNextPos) {
        std::cout << "X pos: " << pos.x << std::endl;
        std::cout << "X vel: " << vel.x << std::endl;
        std::cout << "X Accel: " << accel.x << std::endl;
        std::cout << "X Next Pos: " << calcNextPos.x << std::endl;


        std::cout << std::endl << std::endl;

        std::cout << "Time: " << dt << std::endl;

        std::cout << std::endl << std::endl;


        std::cout << "Y pos: " << pos.y << std::endl;
        std::cout << "Y vel: " << vel.y << std::endl;
        std::cout << "Y Accel: " << accel.y << std::endl;
        std::cout << "Y Next Pos: " << calcNextPos.y << std::endl;

        std::cout << std::endl << std::endl;
    }

};

class Circle {

public:
    Circle() {
        circle.setFillColor(sf::Color::Magenta);
        circle.setRadius(5.0f);
    }

    sf::CircleShape& getShape() { return this->circle; }

    sf::Vector2f& getCurrentPosition() { return this->Pos; }

    sf::Vector2f& getCurrentVelocity() { return this->Vel; }

    sf::Vector2f& getCurrentAcceleration() { return this->Accel; }

    void setCurrentPosition(sf::Vector2f pos) { this->Pos = pos; }

    void setCurrentVelocity(sf::Vector2f& vel) { this->Vel = vel; }

    void setCurrentAcceleration(sf::Vector2f& accel) { this->Accel = accel; }



private:
    sf::CircleShape circle;

    sf::Vector2f Pos = { 5, 5 };
    sf::Vector2f Vel = { 1,1 };
    sf::Vector2f Accel = { 0, GRAVITY };

    sf::Vector2f Momentum = { 0,0 };
    float Mass = 5;
};


class ShapeMgr {
public:
    ShapeMgr() {
        InitializeCircles(1);
    }


    
    void InitializeCircles(int amount) {
        for (int i = 0; i < amount; i++) {
            float randomX = static_cast<float>(rand() % 800); // Random X position between 0 and 800
            float randomY = static_cast<float>(rand() % 600); // Random Y position between 0 and 600
            Circle circle;
            circle.setCurrentPosition(sf::Vector2f{ randomX, randomY });
            circleObjs.push_back(circle);
        }
    }

public:
    std::vector<Circle> circleObjs;
};



int main()
{
    sf::RenderWindow window(sf::VideoMode(1400, 1000), "SFML works!");



    sf::Clock clock;
    float dt = 0.0f;  // Initialize dt


    Physics physics;

    sf::Vector2f windowSize = { 1400, 1000 };
    
    ShapeMgr shapeMgr;


    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        dt = elapsed.asSeconds();

        for (int i = 0; i < shapeMgr.circleObjs.size(); i++)
        {
            sf::Vector2f nextPos = physics.calcNextPos(shapeMgr.circleObjs[i].getCurrentPosition(), shapeMgr.circleObjs[i].getCurrentVelocity(), shapeMgr.circleObjs[i].getCurrentAcceleration(), dt * 125);
            shapeMgr.circleObjs[i].getShape().setPosition(nextPos);
            shapeMgr.circleObjs[i].setCurrentPosition(nextPos);

            // sf::Vector2f nextVel = physics.handleWindowCollision(shapeMgr.circleObjs[i].getCurrentPosition(), shapeMgr.circleObjs[i].getCurrentVelocity(), windowSize);
            // shapeMgr.circleObjs[i].setCurrentVelocity(nextVel);

            bool isWindowCollision = physics.isWindowCollision(shapeMgr.circleObjs[i].getCurrentPosition(), shapeMgr.circleObjs[i].getCurrentVelocity(), windowSize);
            if (isWindowCollision) {
                shapeMgr.circleObjs[i].setCurrentAcceleration(shapeMgr.circleObjs[i].getCurrentAcceleration());
            }

            sf::Vector2f nextVel = physics.calcNextVel(shapeMgr.circleObjs[i].getCurrentVelocity(), shapeMgr.circleObjs[i].getCurrentAcceleration(), dt);
            shapeMgr.circleObjs[i].setCurrentVelocity(nextVel);

        }

        


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        for (int i = 0; i < shapeMgr.circleObjs.size(); i++)
        {
            window.draw(shapeMgr.circleObjs[i].getShape());

        }
        window.display();
    }

    return 0;
}