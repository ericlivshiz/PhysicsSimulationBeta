#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <chrono>

// Distance = Pixels
// Velocity = Pixels/Frame
// Acceleration = Pixels/Frame^2

constexpr int GRAVITY = 9.81;

struct GameState {
    bool isObjectOnFloor = false;
};

class Circle {

public:
    Circle() {
        circle.setFillColor(sf::Color::Magenta);
        circle.setRadius(7.0f);
    }

    sf::CircleShape& getShape() { return this->circle; }

    sf::Vector2f& getCurrentPosition() { return this->Pos; }

    sf::Vector2f& getCurrentVelocity() { return this->Vel; }

    sf::Vector2f& getCurrentAcceleration() { return this->Accel; }

    float& getMass() { return this->Mass; }

    void setCurrentPosition(sf::Vector2f pos) { this->Pos = pos; }

    void setCurrentVelocity(sf::Vector2f vel) { this->Vel = vel; }

    void setCurrentAcceleration(sf::Vector2f accel) { this->Accel = accel; }

    void setMass(float mass) { this->Mass = mass; }



private:
    sf::CircleShape circle;

    sf::Vector2f Pos = { 5, 5 };
    sf::Vector2f Vel;
    sf::Vector2f Accel = { 0, GRAVITY };

    sf::Vector2f Momentum = { 0,0 };
    float Mass = 60;
};


class Physics {

public:

    Physics(GameState& gameState)
        : gameState(gameState)
    {}

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
        vf1.x = component * vi1.x + ((2 * m2) / (m1 + m2)) * vi2.x;
        vf1.y = component * vi1.y + ((2 * m2) / (m1 + m2)) * vi2.y;

        sf::Vector2f vf2;
        vf2.x = ((2 * m1) / (m1 + m2)) * vi1.x - (component * vi2.x);
        vf2.y = ((2 * m1) / (m1 + m2)) * vi1.y - (component * vi2.y);

        std::pair<sf::Vector2f, sf::Vector2f> FinalVelocities;

        FinalVelocities.first = vf1;
        FinalVelocities.second = vf2;

        return FinalVelocities;
    }

    sf::Vector2f applyFriction(Circle& circle) {
        float frictionCoeff = 0.000325;
        float normalForce = circle.getMass();
        sf::Vector2f vel = circle.getCurrentVelocity();
        sf::Vector2f friction = -(vel * normalForce * frictionCoeff);

        return friction;
    }

    // Only function which actually changes velocity rather then return a number
    bool isWindowCollision(sf::Vector2f& pos, sf::Vector2f& vel, float radius, sf::Vector2f& windowSize) {
        bool collided = false;
        int radiusbyconst = radius * 2.35;

        if (pos.x < 0) {
            pos.x = 0;
            vel.x = abs(vel.x);  // Reverse the x velocity to bounce off the wall
            collided = true;
        }
        else if (pos.x + radiusbyconst > windowSize.x) {
            pos.x = windowSize.x - radiusbyconst;
            vel.x = -abs(vel.x);  // Reverse the x velocity to bounce off the wall
            collided = true;
        }

        if (pos.y < 0) {
            pos.y = 0;
            vel.y = abs(vel.y);  // Reverse the y velocity to bounce off the ceiling
            collided = true;
        }
        else if (pos.y + radiusbyconst > windowSize.y) {
            pos.y = windowSize.y - radiusbyconst;
            vel.y = -abs(vel.y);  // Reverse the y velocity to bounce off the floor
            gameState.isObjectOnFloor = true;
            collided = true;
        }

        return collided;
    }


   
    std::pair<bool, Circle> isCircleCollision(std::vector<Circle>& circleObjs, int& iterator)
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

    GameState& gameState;

};


class ShapeMgr {
public:
    ShapeMgr() {

        // Initialize random seed
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        InitializeCircles(3);
        randomColor();
        setRadiusByMass();
    }


    
    void InitializeCircles(int amount) {
        for (int i = 0; i < amount; i++) {
            float randomX = static_cast<float>(rand() % 800); // Random X position between 0 and 800
            float randomY = static_cast<float>(rand() % 600); // Random Y position between 0 and 600
            //float randomX = 50;
            //float randomY = 50;
            Circle circle;
            circle.setCurrentPosition(sf::Vector2f{ randomX, randomY });
            circle.setCurrentVelocity(sf::Vector2f{ 5,0 });
            circleObjs.push_back(circle);
        }
    }

    // This function was not written by me! Chat GPT wrote this for me to change colors of circle based on its velocity (more red more velocity)
    void colorByVelocity() {
        for (int i = 0; i < circleObjs.size(); i++) {
            // Calculate the magnitude of the velocity
            float velocityMagnitude = std::sqrt(circleObjs[i].getCurrentVelocity().x * circleObjs[i].getCurrentVelocity().x +
                circleObjs[i].getCurrentVelocity().y * circleObjs[i].getCurrentVelocity().y);

            // Interpolate between blue (low velocity) and red (high velocity) based on velocity
            float t = std::min(velocityMagnitude / 7, 1.0f);  // Normalize velocity to [0, 1]

            // Interpolate RGB values
            sf::Color color = lerp(sf::Color::Blue, sf::Color::Red, t);

            // Set the color of the circle
            circleObjs[i].getShape().setFillColor(color);
        }
    }

    void randomColor() {
        for (int i = 0; i < circleObjs.size(); i++)
        {
            circleObjs[i].getShape().setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));

        }
    }

    void setRadiusByMass() {

        for (int i = 0; i < circleObjs.size(); i++) {
            if (i % 2 == 0)
                circleObjs[i].setMass(30);
            // Set the radius based on mass, you can adjust the factor as needed
            float mass = circleObjs[i].getMass();
            float radius = std::sqrt(mass) * 3; // Adjust the factor (5) as needed
            circleObjs[i].getShape().setRadius(radius);
        }

    }

    sf::Color lerp(const sf::Color& start, const sf::Color& end, float t) {
        sf::Uint8 r = static_cast<sf::Uint8>(start.r + t * (end.r - start.r));
        sf::Uint8 g = static_cast<sf::Uint8>(start.g + t * (end.g - start.g));
        sf::Uint8 b = static_cast<sf::Uint8>(start.b + t * (end.b - start.b));
        return sf::Color(r, g, b);
    }

    

public:
    std::vector<Circle> circleObjs;
};



int main()
{
    sf::RenderWindow window(sf::VideoMode(1400, 1000), "SFML works!");

    sf::Clock clock;
    float dt = 0.0f;  // Initialize dt

    GameState gameState;

    Physics physics(gameState);

    sf::Vector2f windowSize = { 1400, 1000 };
    
    ShapeMgr shapeMgr;



    const float fixedTimeStep = 0.0069444f;  // Target time step (e.g., 144 FPS)
    sf::Clock frameClock;
    float accumulatedTime = 0.0f;

    while (window.isOpen())
    {
        float deltaTime = frameClock.restart().asSeconds();
        accumulatedTime += deltaTime;

        //shapeMgr.randomColor();

        while (accumulatedTime >= fixedTimeStep)
        {
            for (int i = 0; i < shapeMgr.circleObjs.size(); i++)
            {
                //shapeMgr.colorByVelocity();

                Circle& currCircle = shapeMgr.circleObjs[i];

                sf::Vector2f nextPos = physics.calcNextPos(
                    currCircle.getCurrentPosition(),
                    currCircle.getCurrentVelocity(),
                    currCircle.getCurrentAcceleration(),
                    fixedTimeStep * 100
                );

                currCircle.getShape().setPosition(nextPos);
                currCircle.setCurrentPosition(nextPos);

                physics.isWindowCollision(currCircle.getCurrentPosition(), currCircle.getCurrentVelocity(), currCircle.getShape().getRadius(), windowSize);
 
               /* if (gameState.isObjectOnFloor)
                    currCircle.setCurrentVelocity(currCircle.getCurrentVelocity() + physics.applyFriction(currCircle));*/

                // bool holds true or false for collision, if true then Circle holds the circle that collided with currCircle
                std::pair<bool, Circle> collisionCircle = physics.isCircleCollision(shapeMgr.circleObjs, i);

                if (collisionCircle.first)
                {
                    std::pair<sf::Vector2f, sf::Vector2f> finalVelocities = physics.VfAfterCollision(
                        currCircle.getMass(),
                        collisionCircle.second.getMass(),
                        currCircle.getCurrentVelocity(),
                        collisionCircle.second.getCurrentVelocity()
                    );


                    // Update velocity for the object in shapeMgr.circleObjs
                    int index = -1;
                    for (int j = 0; j < shapeMgr.circleObjs.size(); j++) {
                        if (shapeMgr.circleObjs[j].getCurrentPosition() == collisionCircle.second.getCurrentPosition()) {
                            index = j;
                            break;
                        }
                    }

                    if (index != -1) {
                        shapeMgr.circleObjs[index].setCurrentVelocity(finalVelocities.second);
                        shapeMgr.circleObjs[index].setCurrentPosition(collisionCircle.second.getCurrentPosition() + finalVelocities.second);

                    }

                    currCircle.setCurrentVelocity(finalVelocities.first);
                    currCircle.setCurrentPosition(shapeMgr.circleObjs[i].getCurrentPosition() + finalVelocities.first);

                    

                }

            }
            accumulatedTime -= fixedTimeStep;
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