#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <time.h>   

struct Platform
{
    sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(80, 15));
    sf::Texture platformTexture;
    int type = 0;
    float breakTime = 0;
    bool runTimer = false;
    bool enabled = true;

    Platform(int type = 0, float x = 0, float y = 0)
    {
        rect.setPosition(x, y);
        this->type = type;
        switch (type)
        {
        case 0:
            rect.setFillColor(sf::Color::Green);
            break;
        case 1:
            rect.setFillColor(sf::Color(80, 80, 80));
            break;
        case 2:
            rect.setFillColor(sf::Color::Magenta);
            break;
        case 3:
            rect.setFillColor(sf::Color::Red);
            break;
        }
    }
};


void makePlatform(std::vector<Platform>& platforms, int type, float x = 500, float y = (std::rand() % 20) * 20 + 10)
{
    platforms.push_back(Platform(type, x, y));
}

void resetPlatforms(std::vector<Platform>& platforms)
{
    platforms.clear();
    makePlatform(platforms, 0, 500, 250);
    makePlatform(platforms, 0, 450, 250);
    makePlatform(platforms, 0, 400, 250);
    makePlatform(platforms, 0, 350, 250);
    makePlatform(platforms, 0, 300, 250);
    makePlatform(platforms, 0, 250, 250);
    makePlatform(platforms, 0, 200, 250);
    makePlatform(platforms, 0, 150, 250);
}

bool playerOnPlatform(sf::RectangleShape player, std::vector<Platform> platforms, float velY = 1)
{
    for (int i = 0; i < platforms.size(); i++)
    {
        if (player.getPosition().y + player.getSize().y > platforms[i].rect.getPosition().y - 1 && player.getPosition().y + player.getSize().y < platforms[i].rect.getPosition().y &&
            player.getPosition().x + player.getSize().x > platforms[i].rect.getPosition().x && player.getPosition().x < platforms[i].rect.getPosition().x + platforms[i].rect.getSize().x)
        {
            if (platforms[i].enabled && velY >= 0)
            {
                return true;
            }
        }
    }
    return false;
}


int getPLatformPlayerOnIndex(sf::RectangleShape player, std::vector<Platform> platforms){
    for (int i = 0; i < platforms.size(); i++)
    {
        if (player.getPosition().y + player.getSize().y > platforms[i].rect.getPosition().y - 1 && player.getPosition().y + player.getSize().y < platforms[i].rect.getPosition().y &&
            player.getPosition().x + player.getSize().x > platforms[i].rect.getPosition().x && player.getPosition().x < platforms[i].rect.getPosition().x + platforms[i].rect.getSize().x)
        {
            return i;
        }
    }
    return -1;
}


int main()
{
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");

    std::vector<Platform> platforms;

    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text timerText;
    timerText.setFont(font);
    float timer = 0;

    sf::Texture heartTexture;
    heartTexture.loadFromFile("heart.png");
    sf::RectangleShape heartRect(sf::Vector2f(50, 50));
    heartRect.setPosition(400, 0);
    heartRect.setTexture(&heartTexture);

    sf::Text livesText;
    livesText.setFont(font);
    livesText.setPosition(450, 0);

    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("background.png");
    sf::Sprite back1;
    sf::Sprite back2;
    back1.setTexture(backgroundTexture);
    back2.setTexture(backgroundTexture);
    back1.setPosition(0, 0);
    back2.setPosition(768, 0);

    sf::RectangleShape player(sf::Vector2f(20, 20));
    player.setPosition(sf::Vector2f(225, 1));
    player.setFillColor(sf::Color::Blue);

    bool canJump = false;

    float velY = 0;
    int livesLeft = 3;

    resetPlatforms(platforms);

    sf::Clock deltaClock;

    while (window.isOpen())
    {
        sf::Event event;
        double dt = deltaClock.restart().asSeconds();
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        timer += dt;
        timerText.setString(std::to_string((int)std::floor(timer)));

        livesText.setString(std::to_string(livesLeft));

        //Move Backround
        back1.setPosition(back1.getPosition() + sf::Vector2f(dt*-50, 0));
        back2.setPosition(back2.getPosition() + sf::Vector2f(dt*-50, 0));
        if (back1.getPosition().x < -768) { back1.setPosition(768, 0);}
        if (back2.getPosition().x < -768) { back2.setPosition(768, 0);}


        //move player up/down then update gravity
        player.setPosition(player.getPosition() + sf::Vector2f(0, velY));
        velY += dt*0.8;


        for (int i = 0; i < platforms.size(); i++)
        {
            //moving platforms and replacing them if there at the end of the camera veiw
            platforms[i].rect.setPosition(platforms[i].rect.getPosition() + sf::Vector2f(dt*-200, 0));
            if (platforms[i].rect.getPosition().x + platforms[i].rect.getSize().x < 0) {
                platforms.erase(platforms.begin() + i);
                
                int rnd = std::rand() % 100;

                std::vector<std::vector<int>> platformValues = { {0, 60}, {1, 25}, {2, 10}, {3, 5} };

                int currentPos = 0;
                std::vector<int> vecToChoose;
                for (int i = 0; i < platformValues.size(); i++)
                {
                    for (int a = currentPos; a < platformValues[i][1];  a++)
                    {
                        vecToChoose.push_back(platformValues[i][0]);
                    }
                }
                makePlatform(platforms, vecToChoose[rnd]);
            }

            //if the platform has a timer on it check if its below 0 then deactivate it
            if (platforms[i].runTimer)
            {
                if (platforms[i].breakTime <= 0)
                {
                    platforms[i].rect.setFillColor(sf::Color::Black);
                    platforms[i].enabled = false;
                }
                    platforms[i].breakTime -= dt;
               
            }
        }

        //if up key is not pressed enable jumping
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            canJump = true;
        }

        //if platform player is on a platform then check the type and do stuff acordingly
        if (playerOnPlatform(player, platforms, velY) && getPLatformPlayerOnIndex(player, platforms) != -1)
        {
            int platformI = getPLatformPlayerOnIndex(player, platforms);
            Platform& platform = platforms[platformI];

            switch (platforms[platformI].type)
            {
            case 0:
                break;
            case 1:
                if (!platform.runTimer)
                {
                    platform.runTimer = true;
                    platform.breakTime = 0.2;
                }
                break;
            case 2:
                velY = -velY;
                break;
            case 3:
                resetPlatforms(platforms);
                player.setPosition(sf::Vector2f(225, 1));
                velY = 0;
                timer = 0;
                break;
            }
        }

        //if player is omn the platform set vel to 0 if player is moving down and not holding down key
        if (playerOnPlatform(player, platforms) && velY > 0 && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            velY = 0;
        }

        //reset level if lives are at 0
        if (livesLeft <= 0)
        {
            livesLeft = 3;
            resetPlatforms(platforms);
            player.setPosition(sf::Vector2f(225, 1));
            velY = 0;
            timer = 0;
        }

        //making sure player doesnt fall below level
        if (player.getPosition().y > 500-20)
        {
            velY = 0;
            player.setPosition(player.getPosition().x, 500 - 20);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && playerOnPlatform(player, platforms) && canJump) {
            velY = -0.3;
            canJump = false;
        }

        //moving player along with platform
        if (playerOnPlatform(player, platforms))
        {
            player.setPosition(player.getPosition() + sf::Vector2f(dt * -200, 0));
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.setPosition(player.getPosition() + sf::Vector2f(dt*-200, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.setPosition(player.getPosition() + sf::Vector2f((playerOnPlatform(player, platforms)) ? dt*400 : dt*200, 0));
        }


        if (player.getPosition().y > 450) {
            velY = -0.4;
            livesLeft--;
            std::cout << "heart GONE\n";
        }







        window.clear();
        window.draw(back1);
        window.draw(back2);
        for(Platform platform : platforms)
        {
            if (platform.enabled)
            {
                window.draw(platform.rect);
            }
        }
        window.draw(player);
        window.draw(timerText);
        window.draw(livesText);
        window.draw(heartRect);
        window.display();
    }

    return 0;
}
