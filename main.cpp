#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <QCoreApplication>
#include <QCommandLineParser>

unsigned int SET_WIDTH = 800;
unsigned int SET_HEIGHT = 600;

bool IsDEBUG = false;

void take_screenshot(const sf::RenderTexture& renderTextrue, const std::string& filename)
{
    const sf::Texture& texture = renderTextrue.getTexture();
    if (texture.copyToImage().saveToFile(filename))
    {
        std::cout << "screenshot saved to " << filename << std::endl;
    }
    else
    {
        throw std::runtime_error("error: screenshot");
    }
}

void StartStick(sf::RenderTexture &renderTexture, float size, float k, float A, float B, float start_x, float start_y, int rotate, int n, int counter, float savealp)
{
    if(n <= counter) return;

    size *= k;

    sf::RectangleShape line(sf::Vector2f(size, 2.f));
    sf::CircleShape circle(4);
    sf::CircleShape radius( size);

    radius.setOrigin(size, size);
    line.setOrigin(0, 1);

    line.setPosition(start_x, start_y);
    circle.setPosition(start_x, start_y);
    radius.setPosition(start_x, start_y);

    if(rotate == -1)
    {
        savealp -= A;
        line.rotate(180 - savealp);

        start_x -= size * cos((360 - (savealp / A) * A) / 180.0 * M_PI);
        start_y -= size * sin((360 - (savealp / A) * A) / 180.0 * M_PI);

        if(IsDEBUG)
            std::cout << counter + 1 << " " << A << " " << savealp << " " << rotate << " " << start_x << " " << start_y << std::endl;
    }
    else if(rotate == 1)
    {
        savealp += B;
        line.rotate(180 - savealp);

        start_x -= size * cos((360 - (savealp / B) * B)/180.0 * M_PI);
        start_y -= size * sin((360 - (savealp / B) * B)/180.0 * M_PI);

        if(IsDEBUG)
            std::cout << counter + 1 << " " << B << " " << savealp<< " " << rotate << " " << start_x << " " << start_y << std::endl;
    }
    else
    {
        line.rotate(savealp);
        start_y -= size;

        if(IsDEBUG)
            std::cout << counter + 1 << " " << A << " " << B << " " << rotate << " " << start_x << " " << start_y << std::endl;
    }

    circle.setFillColor(sf::Color(100, 200, 0, 255));
    circle.setOrigin(4, 4);
    radius.setFillColor(sf::Color(100, 0, 0, 50));

    StartStick(renderTexture, size, k, A, B, start_x, start_y, -1, n, counter + 1, savealp);
    StartStick(renderTexture, size, k, A, B, start_x, start_y, 1, n, counter + 1, savealp);

    if(IsDEBUG)
    {
        renderTexture.draw(radius);
        renderTexture.draw(circle);
    }

    if(n - counter <= 4)
        line.setFillColor(sf::Color(92, 162, 4, 255));

    renderTexture.draw(line);
}

QScopedPointer<QCommandLineParser> parc_arg(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    auto *pars = new QCommandLineParser;

    pars->addHelpOption();

    pars->addOption({{"alpha","A"}, "parametr is Alpha", "NUMBER", "35"});
    pars->addOption({{"beta","B"}, "parametr is Beta", "NUMBER", "35"});
    pars->addOption({{"size","s"}, "size main stick", "NUMBER", "200"});
    pars->addOption({"k", "???", "NUMBER", "0.75"});
    pars->addOption({{"width","W"}, "size main window", "NUMBER", "800"});
    pars->addOption({{"height","H"}, "size main window", "NUMBER", "600"});
    pars->addOption({{"depth","n"}, "depth of wood", "NUMBER", "13"});
    pars->addOption({"scale", "scale of sprite", "NUMBER", "1"});
    pars->addOption({"delta", "delta of sprite", "NUMBER", "0"});
    pars->addOption({"screenshot", "flag for screenshot"});
    pars->addOption({{"DEBUG","D"}, "set mode DEBUG"});

    pars->process(app);

    if(!pars->parse(QCoreApplication::arguments()))
        throw std::runtime_error(pars->errorText().toLatin1().toStdString());

    return QScopedPointer(pars);
}

int main(int argc, char **argv)
{
    auto pars = parc_arg(argc, argv);

    double Beta = pars->value("A").toDouble();
    double Alpha = pars->value("B").toDouble();
    double size = pars->value("size").toDouble();
    double k = pars->value("k").toDouble();
    SET_WIDTH = pars->value("width").toUInt();
    SET_HEIGHT = pars->value("height").toUInt();
    int n = pars->value("n").toInt();
    double scale = pars->value("scale").toDouble();
    bool isScreen = pars->isSet("screenshot");
    IsDEBUG = pars->isSet("DEBUG");
    double delta = pars->value("delta").toDouble();

    int count = 0;
    float save_size = size;
    while(save_size*k > 4)
    {
        save_size *= k;
        count++;
    }
    std::cout << "Math: " << count << std::endl;

    if(n > count)
        n = count;

    if(k > 1.0f)
        throw std::runtime_error("k is not correct");

    sf::RenderWindow window(sf::VideoMode(SET_WIDTH,SET_HEIGHT), "Test");

    sf::RenderTexture renderTexture;

    if(!renderTexture.create(SET_WIDTH*scale, SET_HEIGHT*scale))
        throw std::runtime_error("Not create render texture");

    renderTexture.clear(sf::Color(0,0,0,255));

    //StartStick(renderTexture, 200, 0.75, 50, 35, (double)SET_WIDTH/2, SET_HEIGHT, 0, 13, 0, 270); //n = 8
    StartStick(renderTexture, size, k, Alpha, Beta, (double)SET_WIDTH/2*scale, SET_HEIGHT*scale + delta, 0, n, 0, 270);

    renderTexture.display();
    const sf::Texture &texture = renderTexture.getTexture();

    sf::Sprite textureSprite(texture);

    float Sprite_x = 0, Sprite_y = 0;
    float Sprite_scale = 1;


    if(scale >= 1.0f)
    {
        scale = (double)SET_WIDTH / textureSprite.getGlobalBounds().width;
        textureSprite.scale(scale, scale);
    }

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();

            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Right:
                            //if(textureSprite.getGlobalBounds().width + Sprite_x > SET_WIDTH)
                                Sprite_x -= 10;
                            break;

                        case sf::Keyboard::Left:
                            //if(Sprite_x + 10 <= 0)
                                Sprite_x += 10;
                            break;

                        case sf::Keyboard::Down:
                            //if(textureSprite.getGlobalBounds().height + Sprite_y > SET_HEIGHT)
                                Sprite_y -= 10;
                            break;

                        case sf::Keyboard::Up:
                            //if(Sprite_y + 10 <= 0)
                                Sprite_y += 10;
                            break;
                    }

                    textureSprite.setPosition(Sprite_x, Sprite_y);
                    break;

                case sf::Event::MouseWheelMoved:
                    if(event.mouseWheel.delta == 1)
                    {
                        if(Sprite_scale + 0.01 <= 1.2)
                        {
                            Sprite_scale += 0.01;
                            textureSprite.scale(1.01, 1.01);
                        }
                    }
                    else
                    {

                        if(Sprite_scale - 0.01 >= 0.85)
                        {
                            Sprite_scale -= 0.01;
                            textureSprite.scale(0.99, 0.99);
                        }
                    }
                    std::cout << Sprite_scale << std::endl;
                    break;
            }

        }

        window.clear();
        window.draw(textureSprite);
        window.display();
    }

    if(isScreen)
        take_screenshot(renderTexture, "screen.jpg");

    return EXIT_SUCCESS;
}
