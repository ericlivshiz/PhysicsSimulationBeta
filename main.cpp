#include <SFML/Graphics.hpp>
#include <iostream>

// Distance = Pixels
// Velocity = Pixels/Frame
// Acceleration = Pixels/Frame^2

constexpr int GRAVITY = 9.81;

class Physics {

public:

    sf::Vector2f calcNextPos(sf::Vector2f& pos, sf::Vector2f& vel, sf::Vector2f& accel, float dt)
    {
        sf::Vector2f calcNextPos;
        calcNextPos.x = pos.x + (vel.x * dt) + (0.5 * accel.x * (dt * dt));
        calcNextPos.y = pos.y + (vel.y * dt) + (0.5 * accel.y * (dt * dt));

        // debugNextPos(pos, vel, accel, dt, calcNextPos);
        return calcNextPos;
    }

    sf::Vector2f calcNextVel(sf::Vector2f& vel, sf::Vector2f& accel, float dt)
    {
        sf::Vector2f calcNextVel;
        calcNextVel.x = vel.x + (accel.x * dt);
        calcNextVel.y = vel.y + (accel.y * dt);
        return calcNextVel;
    }

    sf::Vector2f handleWindowCollision(sf::Vector2f& pos, sf::Vector2f& vel, sf::Vector2f& windowSize) {
        if (pos.x < 5 || pos.x > windowSize.x - 5)
            vel.x = (-1 * vel.x);

        if (pos.y < 5 || pos.y > windowSize.y - 5)
            vel.y = (-1 * vel.y);

        return vel;
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

    void setCurrentPosition(sf::Vector2f& pos) { this->Pos = pos; }

    void setCurrentVelocity(sf::Vector2f& vel) { this->Vel = vel; }



private:
    sf::CircleShape circle;
    
    sf::Vector2f Pos = { 5, 5 };
    sf::Vector2f Vel = { 2,1 };
    sf::Vector2f Accel = { 0, GRAVITY};
};



int main()
{
    sf::RenderWindow window(sf::VideoMode(1400, 1000), "SFML works!");

    Circle circle;

    sf::Clock clock;
    float dt = 0.0f;  // Initialize dt


    Physics physics;

    sf::Vector2f windowSize = { 1400, 1000 };
    

    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        dt = elapsed.asSeconds();

        sf::Vector2f nextPos = physics.calcNextPos(circle.getCurrentPosition(), circle.getCurrentVelocity(), circle.getCurrentAcceleration(), dt*50);
        circle.getShape().setPosition(nextPos);
        circle.setCurrentPosition(nextPos);

        sf::Vector2f nextVel = physics.handleWindowCollision(circle.getCurrentPosition(), circle.getCurrentVelocity(), windowSize);
        circle.setCurrentVelocity(nextVel);

        nextVel = physics.calcNextVel(circle.getCurrentVelocity(), circle.getCurrentAcceleration(), dt);
        circle.setCurrentVelocity(nextVel);


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(circle.getShape());
        window.display();
    }

    return 0;
}