#include <BLIB/Media/Graphics/Animation.hpp>
#include <SFML/Window.hpp>

int main() {
    sf::Clock timer;
    sf::RenderWindow window(
        sf::VideoMode(800, 800, 32), "Animation Example", sf::Style::Titlebar | sf::Style::Close);

    bl::gfx::AnimationData src("resources/animation.anim");
    bl::gfx::Animation anim(src);

    anim.setIsCentered(true);
    anim.setPosition(sf::Vector2f(400, 400));
    anim.setIsLoop(true);
    anim.setRotation(30);
    anim.play();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        anim.update(timer.getElapsedTime().asSeconds());
        timer.restart();

        window.clear(sf::Color::White);
        window.draw(anim);
        window.display();
        sf::sleep(sf::milliseconds(10));
    }
    return 0;
}
