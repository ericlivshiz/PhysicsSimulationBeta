#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// Distance = Pixels
// Velocity = Pixels/Frame
// Acceleration = Pixels/Frame^2

constexpr int GRAVITY = 9.81;

class Circle {

public:
    Circle() {
        circle.setFillColor(sf::Color::Magenta);
        circle.setRadius(15.0f);
    }

    sf::CircleShape& getShape() { return this->circle; }

    sf::Vector2f& getCurrentPosition() { return this->Pos; }

    sf::Vector2f& getCurrentVelocity() { return this->Vel; }

    sf::Vector2f& getCurrentAcceleration() { return this->Accel; }

    float& getMass() { return this->Mass; }

    void setCurrentPosition(sf::Vector2f pos) { this->Pos = pos; }

    void setCurrentVelocity(sf::Vector2f& vel) { this->Vel = vel; }

    void setCurrentAcceleration(sf::Vector2f& accel) { this->Accel = accel; }

    void setMass(float& mass) { this->Mass = mass; }



private:
    sf::CircleShape circle;

    sf::Vector2f Pos = { 5, 5 };
    sf::Vector2f Vel = { 1,1 };
    sf::Vector2f Accel = { 0, GRAVITY };

    sf::Vector2f Momentum = { 0,0 };
    float Mass = 1;
};


class Physics {

public:

    sf::Vector2f calcNextPos(sf::Vector2f& pos, sf::Vector2f& vel, sf::Vector2f& accel, float dt)
    {
        sf::Vector2f NextPosition;
        NextPosition.x = pos.x + (vel.x * dt) + (0.5 * accel.x * (dt * dt));
        NextPosition.y = pos.y + (vel.y * dt) + (0.5 * accel.y * (dt * dt));

        // debugNextPos(pos, vel, accel, dt, NextPosition);
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

    std::pair<sf::Vector2f, sf::Vector2f> VfAfterCollision(float& m1, float& m2, sf::Vector2f& vi1, sf::Vector2f& vi2)
    {
        float component = (m1 - m2) / (m1 + m2);

        // vf1 = final velocity vector of object 1...
        sf::Vector2f vf1;
        vf1.x = component * vi1.x + (2 * m2) / (m1 + m2) * vi2.x;
        vf1.y = component * vi1.y + (2 * m2) / (m1 + m2) * vi2.y;

        sf::Vector2f vf2;
        vf2.x = ((2 * m1) / (m1 + m2) * vi1.x) - (component * vi2.x);
        vf2.y = ((2 * m1) / (m1 + m2) * vi1.x) - (component * vi2.y);

        std::pair<sf::Vector2f, sf::Vector2f> FinalVelocities;

        FinalVelocities.first = vf1;
        FinalVelocities.second = vf2;

        return FinalVelocities;
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

   
    std::pair<bool, Circle> isCircleCollision(std::vector<Circle> circleObjs, int iterator)
    {
        std::pair<bool, Circle> CircleCollision;


        for (int i = iterator + 1; i < circleObjs.size(); i++) {
            // Find the delta distance vector between the two circles
            sf::Vector2f distance;
            distance.x = circleObjs[i].getShape().getPosition().x - circleObjs[iterator].getShape().getPosition().x;
            distance.y = circleObjs[i].getShape().getPosition().y - circleObjs[iterator].getShape().getPosition().y;

            // Magnitude formula (Pythagorean Theorem)
            float distanceMag = sqrt((distance.x * distance.x) + (distance.y * distance.y));


            // If distance between objects is less then there radii added up then they've collided
            if (distanceMag < (circleObjs[iterator].getShape().getRadius() + circleObjs[i].getShape().getRadius())) {
                CircleCollision.first = true;
                CircleCollision.second = circleObjs[i];
            }
        }

        //for (int i = 0; i < circleObjs.size(); i++) {
        //    for (int j = i + 1; j < circleObjs.size(); j++) {

        //        // Find the delta distance vector between the two circles
        //        sf::Vector2f distance;
        //        distance.x = circleObjs[j].getShape().getPosition().x - circleObjs[i].getShape().getPosition().x;
        //        distance.y = circleObjs[j].getShape().getPosition().y - circleObjs[i].getShape().getPosition().y;

        //        // Magnitude formula (Pythagorean Theorem)
        //        float distanceMag = sqrt((distance.x * distance.x) + (distance.y * distance.y));

        //        // If distance between objects is less then there radii added up then they've collided
        //        if (distanceMag < (circleObjs[i].getShape().getRadius() + circleObjs[j].getShape().getRadius()))
        //            return true;
        //    }
        //}

        return CircleCollision;
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


class ShapeMgr {
public:
    ShapeMgr() {
        InitializeCircles(15);
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

            sf::Vector2f nextPos = physics.calcNextPos(shapeMgr.circleObjs[i].getCurrentPosition(), shapeMgr.circleObjs[i].getCurrentVelocity(), shapeMgr.circleObjs[i].getCurrentAcceleration(), dt * 15);
            shapeMgr.circleObjs[i].getShape().setPosition(nextPos);
            shapeMgr.circleObjs[i].setCurrentPosition(nextPos);

            bool isWindowCollision = physics.isWindowCollision(shapeMgr.circleObjs[i].getCurrentPosition(), shapeMgr.circleObjs[i].getCurrentVelocity(), windowSize);
            if (isWindowCollision) {
                shapeMgr.circleObjs[i].setCurrentAcceleration(shapeMgr.circleObjs[i].getCurrentAcceleration());
            }

            sf::Vector2f nextVel = physics.calcNextVel(shapeMgr.circleObjs[i].getCurrentVelocity(), shapeMgr.circleObjs[i].getCurrentAcceleration(), dt);
            shapeMgr.circleObjs[i].setCurrentVelocity(nextVel);


            // Check for collisions starting from the current circle (i) and onwards
            std::pair<bool, Circle> collisionCircle = physics.isCircleCollision(shapeMgr.circleObjs, i);

            if (collisionCircle.first)
            {
                
                // A collision was detected, and 'collisionCircle' points to the collided circle
                // Calculate the final velocities using VfAfterCollision
                std::pair<sf::Vector2f, sf::Vector2f> finalVelocities = physics.VfAfterCollision(
                    shapeMgr.circleObjs[i].getMass(),
                    collisionCircle.second.getMass(),
                    shapeMgr.circleObjs[i].getCurrentVelocity(),
                    collisionCircle.second.getCurrentVelocity()
                );

                // Set the velocities of the current circle and the collided circle
                shapeMgr.circleObjs[i].setCurrentVelocity(finalVelocities.first);
                collisionCircle.second.setCurrentVelocity(finalVelocities.second);

            }

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